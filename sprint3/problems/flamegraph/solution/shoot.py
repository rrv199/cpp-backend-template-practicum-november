#!/usr/bin/env python3
import argparse
import shlex
import subprocess
import time
import random
import signal
import os
import tempfile

# Список запросов для обстрела сервера
AMMUNITION = [
    "/api/v1/maps",
    "/api/v1/maps/map1",
    "/api/v1/maps/map2",
    "/api/v1/maps/map3",
    "/index.html",
    "/game.html",
    "/about.html",
    "/images/cube.svg",
]

def parse_args():
    parser = argparse.ArgumentParser(description='Shoot the server and generate flamegraph')
    parser.add_argument('server_cmd', type=str, help='Command to start the server')
    return parser.parse_args()

def start_server(cmd_str):
    """Запускает сервер в фоновом процессе"""
    cmd = shlex.split(cmd_str)
    proc = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return proc

def stop_server(proc):
    """Останавливает сервер"""
    proc.terminate()
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait()

def start_perf(pid, output_file="perf.data"):
    """Запускает perf record для процесса с указанным PID"""
    cmd = ["perf", "record", "-g", "-o", output_file, "-p", str(pid), "sleep", "10"]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return proc

def stop_perf(proc):
    """Останавливает perf record"""
    proc.terminate()
    proc.wait()

def make_shots(shots=50):
    """Выполняет запросы к серверу"""
    for i in range(shots):
        target = random.choice(AMMUNITION)
        cmd = ["curl", "-s", "-o", "/dev/null", "-w", "%{http_code}", f"http://127.0.0.1:8080{target}"]
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
            print(f"Shot {i+1}: {target} -> {result.stdout}")
        except subprocess.TimeoutExpired:
            print(f"Shot {i+1}: {target} -> TIMEOUT")
        time.sleep(0.1)

def generate_flamegraph(perf_data="perf.data", output_svg="graph.svg"):
    """Генерирует флеймграф из perf.data"""
    if not os.path.exists(perf_data) or os.path.getsize(perf_data) == 0:
        print(f"Error: {perf_data} is empty or does not exist")
        return False
    
    # Запускаем perf script и передаём в FlameGraph скрипты
    cmd_perf_script = ["perf", "script", "-i", perf_data]
    cmd_stackcollapse = ["./FlameGraph/stackcollapse-perf.pl"]
    cmd_flamegraph = ["./FlameGraph/flamegraph.pl", "-o", output_svg]
    
    p1 = subprocess.Popen(cmd_perf_script, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    p2 = subprocess.Popen(cmd_stackcollapse, stdin=p1.stdout, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    p1.stdout.close()
    
    with open(output_svg, "wb") as f:
        subprocess.run(cmd_flamegraph, stdin=p2.stdout, stdout=f, stderr=subprocess.DEVNULL)
    
    p2.stdout.close()
    return os.path.exists(output_svg) and os.path.getsize(output_svg) > 0

def main():
    args = parse_args()
    
    print(f"Starting server: {args.server_cmd}")
    server_proc = start_server(args.server_cmd)
    
    # Даём серверу время на запуск
    time.sleep(3)
    
    # Проверяем, что сервер запущен
    if server_proc.poll() is not None:
        print("Server failed to start!")
        return
    
    print(f"Server PID: {server_proc.pid}")
    
    print("Starting perf record...")
    perf_proc = start_perf(server_proc.pid)
    
    # Даём perf время на начало записи
    time.sleep(1)
    
    print("Shooting the server...")
    make_shots(shots=50)
    
    print("Stopping perf record...")
    stop_perf(perf_proc)
    
    # Проверяем, что perf.data создан
    if os.path.exists("perf.data") and os.path.getsize("perf.data") > 0:
        print(f"perf.data size: {os.path.getsize('perf.data')} bytes")
    else:
        print("Warning: perf.data not created or empty")
    
    print("Generating flamegraph...")
    if generate_flamegraph():
        print("Flamegraph generated successfully: graph.svg")
        print(f"graph.svg size: {os.path.getsize('graph.svg')} bytes")
    else:
        print("Failed to generate flamegraph")
    
    print("Stopping server...")
    stop_server(server_proc)
    
    print("Done!")

if __name__ == "__main__":
    main()

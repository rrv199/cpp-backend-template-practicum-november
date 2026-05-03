#include "json_loader.h"
#include <iostream>

namespace json_loader {
model::Game LoadGame(const std::filesystem::path& json_path) {
    std::cout << "Loading config from: " << json_path << std::endl;
    model::Game game;
    return game;
}
}

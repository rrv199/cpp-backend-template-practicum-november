#include "request_handler.h"
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <iostream>

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

using Response = http::response<http::string_body>;

void RequestHandler::operator()(http::request<http::string_body>&& req, std::function<void(http::response<http::string_body>&&)>&& send) {
    std::string target(req.target().data(), req.target().size());
    std::cout << "=== Request received: " << target << std::endl;

    Response response;

    if (target == Endpoint::MAPS) {
        response = HandleMapsRequest();
    }
    else if (target.size() > Endpoint::MAP_PREFIX.size() && 
             target.substr(0, Endpoint::MAP_PREFIX.size()) == Endpoint::MAP_PREFIX) {
        std::string map_id = target.substr(Endpoint::MAP_PREFIX.size());
        response = HandleMapRequest(map_id);
    }
    else if (target == Endpoint::RECORDS) {
        response = HandleRecordsRequest();
    }
    else if (target.size() >= Endpoint::API_PREFIX.size() && 
             target.substr(0, Endpoint::API_PREFIX.size()) == Endpoint::API_PREFIX) {
        response = MakeErrorResponse(http::status::bad_request, "badRequest", "Bad request");
    }
    else {
        response = MakeErrorResponse(http::status::not_found, "notFound", "Not found");
    }

    send(std::move(response));
}

Response RequestHandler::HandleMapsRequest() {
    json::array maps_array;
    
    for (const auto& map : game_.GetMaps()) {
        json::object map_obj;
        map_obj["id"] = *map.GetId();
        map_obj["name"] = map.GetName();
        maps_array.push_back(std::move(map_obj));
    }
    
    std::string body = json::serialize(maps_array);
    
    Response response(http::status::ok, 11);
    response.set(http::field::content_type, "application/json");
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(false);
    
    return response;
}

Response RequestHandler::HandleMapRequest(const std::string& map_id_str) {
    model::Map::Id map_id(map_id_str);
    const model::Map* map = game_.FindMap(map_id);
    
    if (!map) {
        return MakeErrorResponse(http::status::not_found, "mapNotFound", "Map not found");
    }
    
    json::object map_obj;
    map_obj["id"] = *map->GetId();
    map_obj["name"] = map->GetName();
    
    json::array roads_array;
    for (const auto& road : map->GetRoads()) {
        json::object road_obj;
        auto start = road.GetStart();
        auto end = road.GetEnd();
        road_obj["x0"] = start.x;
        road_obj["y0"] = start.y;
        
        if (road.IsHorizontal()) {
            road_obj["x1"] = end.x;
        } else {
            road_obj["y1"] = end.y;
        }
        roads_array.push_back(std::move(road_obj));
    }
    map_obj["roads"] = std::move(roads_array);
    
    json::array buildings_array;
    for (const auto& building : map->GetBuildings()) {
        auto bounds = building.GetBounds();
        json::object building_obj;
        building_obj["x"] = bounds.position.x;
        building_obj["y"] = bounds.position.y;
        building_obj["w"] = bounds.size.width;
        building_obj["h"] = bounds.size.height;
        buildings_array.push_back(std::move(building_obj));
    }
    map_obj["buildings"] = std::move(buildings_array);
    
    json::array offices_array;
    for (const auto& office : map->GetOffices()) {
        json::object office_obj;
        office_obj["id"] = *office.GetId();
        office_obj["x"] = office.GetPosition().x;
        office_obj["y"] = office.GetPosition().y;
        office_obj["offsetX"] = office.GetOffset().dx;
        office_obj["offsetY"] = office.GetOffset().dy;
        offices_array.push_back(std::move(office_obj));
    }
    map_obj["offices"] = std::move(offices_array);
    
    std::string body = json::serialize(map_obj);
    
    Response response(http::status::ok, 11);
    response.set(http::field::content_type, "application/json");
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(false);
    
    return response;
}

Response RequestHandler::HandleRecordsRequest() {
    std::cout << "=== HandleRecordsRequest called ===" << std::endl;
    
    try {
        auto records = db_manager_.GetRecords(0, 100);
        
        json::array json_array;
        for (const auto& record : records) {
            json::object obj;
            obj["name"] = record.name;
            obj["score"] = record.score;
            obj["playTime"] = record.play_time_seconds;
            json_array.push_back(obj);
        }
        
        std::string body = json::serialize(json_array);
        
        Response response(http::status::ok, 11);
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(false);
        
        return response;
    } catch (const std::exception& e) {
        std::cerr << "Error in HandleRecordsRequest: " << e.what() << std::endl;
        return MakeErrorResponse(http::status::internal_server_error, "internalError", e.what());
    }
}

Response RequestHandler::MakeErrorResponse(http::status status, const std::string& code, const std::string& message) {
    json::object error_obj;
    error_obj["code"] = code;
    error_obj["message"] = message;
    
    std::string body = json::serialize(error_obj);
    
    Response response(status, 11);
    response.set(http::field::content_type, "application/json");
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(false);
    
    return response;
}

} // namespace http_handler

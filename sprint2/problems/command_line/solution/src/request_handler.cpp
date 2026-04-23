
json::array RequestHandler::SerializeRoads(const model::Roads& roads) {
    json::array roads_array;
    for (const auto& road : roads) {
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
    return roads_array;
}

json::array RequestHandler::SerializeBuildings(const model::Buildings& buildings) {
    json::array buildings_array;
    for (const auto& building : buildings) {
        auto bounds = building.GetBounds();
        json::object building_obj;
        building_obj["x"] = bounds.position.x;
        building_obj["y"] = bounds.position.y;
        building_obj["w"] = bounds.size.width;
        building_obj["h"] = bounds.size.height;
        buildings_array.push_back(std::move(building_obj));
    }
    return buildings_array;
}

json::array RequestHandler::SerializeOffices(const model::Offices& offices) {
    json::array offices_array;
    for (const auto& office : offices) {
        json::object office_obj;
        office_obj["id"] = *office.GetId();
        office_obj["x"] = office.GetPosition().x;
        office_obj["y"] = office.GetPosition().y;
        office_obj["offsetX"] = office.GetOffset().dx;
        office_obj["offsetY"] = office.GetOffset().dy;
        offices_array.push_back(std::move(office_obj));
    }
    return offices_array;
}

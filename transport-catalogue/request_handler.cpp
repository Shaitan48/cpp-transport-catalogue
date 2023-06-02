/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

#include "request_handler.h"



const std::set<std::string> RequestHandler::GetBusesByStop(const std::string& stop_name) const  {
    std::set<std::string> unique_buses;
    const transportCatalog::Stop* stop = catalogue_.FindStop(stop_name);
    for(auto bus : stop->buses){
        unique_buses.insert(bus->number);
    }
    return unique_buses;
}

bool RequestHandler::ExistingBus(const std::string_view bus_number) const
{
    return catalogue_.FindRoute(bus_number);
}

bool RequestHandler::ExistingStop(const std::string_view stop_name) const
{
    return catalogue_.FindStop(stop_name);
}

const transportCatalog::RouteInfo RequestHandler::GetRoutStat(const std::string &route_number) const
{
    transportCatalog::RouteInfo route_info{};
    const transportCatalog::Bus* bus = catalogue_.FindRoute(route_number);

    if (bus == nullptr) {
        throw std::invalid_argument("bus not found");
    }

    route_info.stops_count = bus->stops.size();

    double route_length = 0.0;
    double geo_length = 0.0;

    for (auto iter = bus->stops.begin(); iter + 1 != bus->stops.end(); iter++) {
        transportCatalog::Stop* sourse = *iter;
        transportCatalog::Stop* destination = *(iter + 1);
        route_length += catalogue_.GetDistance(sourse, destination);
        geo_length += ComputeDistance(sourse->coordinates,
                                      destination->coordinates);

    }
    route_info.unique_stops_count = catalogue_.UniqueStopsCount(route_number);
    route_info.route_length = route_length;
    route_info.curvature = route_length / geo_length;

    return route_info;
}

//const json::Node RequestHandler::PrintStop(const json::Dict& request_map) const {
//    json::Dict result;
//    const std::string& stop_name = request_map.at("name").AsString();
//    result["request_id"] = request_map.at("id").AsInt();
//    if (!catalogue_.FindStop(stop_name)) {
//        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
//    }
//    else {
//        json::Array buses;
//        for (auto& bus : GetBusesByStop(stop_name)) {
//            buses.push_back(bus);
//        }
//        result["buses"] = buses;
//    }

//    return json::Node{result};
//}

//const json::Node RequestHandler::PrintRoute(const json::Dict& request_map) const {
//    json::Dict result;
//    const std::string& route_number = request_map.at("name").AsString();
//    result["request_id"] = request_map.at("id").AsInt();
//    if (!catalogue_.FindRoute(route_number)) {
//        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
//    }
//    else {
//        result["curvature"] = GetRoutStat(route_number).curvature;

//        result["route_length"] = GetRoutStat(route_number).route_length;
//        result["stop_count"] = static_cast<int>(GetRoutStat(route_number).stops_count);
//        result["unique_stop_count"] = static_cast<int>(GetRoutStat(route_number).unique_stops_count);
//    }
//    return json::Node{result};
//}

//void RequestHandler::ProcessRequests(const json::Node& request_map) const {
//    json::Array result;
//    const json::Array& arr = request_map.AsArray();
//    for (auto& request : arr) {
//        const auto& request_map = request.AsMap();
//        const auto& type = request_map.at("type").AsString();
//        if (type == "Stop") {
//            result.push_back(PrintStop(request_map).AsMap());
//        }
//        if (type == "Bus") {
//            result.push_back(PrintRoute(request_map).AsMap());
//        }
//    }
//    json::Print(json::Document{ result }, std::cout);
//}
svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.GetSortedAllBuses());
}

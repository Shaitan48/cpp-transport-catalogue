#include "request_handler.h"

#include <utility>
#include <sstream>
#include <unordered_set>

using namespace std;
using namespace transportCatalogue;
using namespace domain;

RequestHandler::RequestHandler(const transportCatalogue::Catalogue& catalogue,
                               const transportCatalogue::Router& router, const renderer::MapRenderer& renderer)
    : db_(catalogue)
    , router_(router)
    , renderer_(renderer) {}

void RequestHandler::JsonStatRequests(const json::Node& json_input, std::ostream& output) {
    const json::Array& arr = json_input.AsArray();
    json::Array output_array;
    output_array.reserve(arr.size());
    for (auto& request_node : arr) {
        const json::Dict& request_map = request_node.AsDict();
        const string& type = request_map.at("type"s).AsString();
        if (type == "Stop"s) {
            output_array.push_back(FindStopRequestProcessing(request_map));
            continue;
        }
        if (type == "Bus"s) {
            output_array.push_back(FindBusRequestProcessing(request_map));
            continue;
        }
        if (type == "Map"s) {
            output_array.push_back(BuildMapRequestProcessing(request_map));
            continue;
        }
        if (type == "Route"s) {
            output_array.push_back(BuildRouteRequestProcessing(request_map));
            continue;
        }
    }
    json::Print(json::Document(json::Node(move(output_array))), output);
}

<<<<<<< HEAD
=======
bool RequestHandler::ExistingBus(const std::string_view bus_number) const
{
    return catalogue_.FindRoute(bus_number);
}

bool RequestHandler::ExistingStop(const std::string_view stop_name) const
{
    return catalogue_.FindStop(stop_name);
}

const transportCatalog::RouteInfo RequestHandler::GetRoutStat(const std::string_view route_number) const
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
>>>>>>> parent of abd3691 (my favorite progaramm)
svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSvgDocument(db_.GetSortedAllBuses());
}

json::Node RequestHandler::FindStopRequestProcessing(const json::Dict& request_map) {
    int id = request_map.at("id"s).AsInt();
    const string& name = request_map.at("name"s).AsString();
    if (const Stop* stop = db_.FindStop(name)) {
        json::Array buses_array;
        const auto& buses_on_stop = db_.GetBusesOnStop(stop->name);
        buses_array.reserve(buses_on_stop.size());
        for (auto& [bus_name, bus] : buses_on_stop) {
            buses_array.push_back(bus->name);
        }
        return json::Node(json::Dict{
            {{"buses"s},{move(buses_array)}},
            {{"request_id"s},{id}}
        });
    }
    else {
        return json::Builder{}.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }
}

json::Node RequestHandler::FindBusRequestProcessing(const json::Dict& request_map) {
    int id = request_map.at("id"s).AsInt();
    const string& name = request_map.at("name"s).AsString();
    if (const Bus* bus = db_.FindBus(name)) {
        int stops_count = bus->stops.size();
        int distance = 0;
        double straight_distance = 0.0;
        for (int i = 1; i < stops_count; ++i) {
            distance += bus->stops[i - 1]->GetDistance(bus->stops[i]);
            straight_distance += geo::ComputeDistance(bus->stops[i - 1]->coordinates, bus->stops[i]->coordinates);
        }
        double curvature = distance / straight_distance;
        unordered_set<string> unique_stops_set;
        for (transportCatalogue::Stop* s : bus->stops) {
            unique_stops_set.emplace(s->name);
        }
        int unique_stops = unique_stops_set.size();
        return json::Node(json::Dict{
            {{"route_length"s},{distance}},
            {{"unique_stop_count"s},{unique_stops}},
            {{"stop_count"s},{stops_count}},
            {{"curvature"s},{curvature}},
            {{"request_id"s},{id}}
        });
    }
    else {
        return json::Builder{}.StartDict()
            .Key("error_message"s).Value("not found"s)
            .Key("request_id"s).Value(id)
            .EndDict().Build();
    }
}

json::Node RequestHandler::BuildMapRequestProcessing(const json::Dict& request_map) {
    int id = request_map.at("id"s).AsInt();
    svg::Document map = RenderMap();
    ostringstream strm;
    map.Render(strm);
    return json::Node(json::Dict{
        {{"map"s},{strm.str()}},
        {{"request_id"s},{id}}
    });
}

json::Node RequestHandler::BuildRouteRequestProcessing(const json::Dict& request_map) {
    int id = request_map.at("id"s).AsInt();
    const string& name_from = request_map.at("from"s).AsString();
    const string& name_to = request_map.at("to"s).AsString();
    if (const Stop* stop_from = db_.FindStop(name_from)) {
        if (const Stop* stop_to = db_.FindStop(name_to)) {
            if (auto ri = router_.GetRouteInfo(stop_from, stop_to)) {
                auto [wieght, edges] = ri.value();
                return json::Node(json::Dict{
                    {{"items"s},{router_.GetEdgesItems(edges)}},
                    {{"total_time"s},{wieght}},
                    {{"request_id"s},{id}}
                });
            }
        }
    }
    return json::Builder{}.StartDict()
        .Key("error_message"s).Value("not found"s)
        .Key("request_id"s).Value(id)
        .EndDict().Build();
}

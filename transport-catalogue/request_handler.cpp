/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

#include "request_handler.h"



const std::set<std::string> RequestHandler::GetBusesByStop(const std::string_view stop_name) const  {
    std::set<std::string> unique_buses;
    const transportCatalog::Stop* stop = catalogue_.FindStop(stop_name);
    for(auto bus : stop->buses){
        unique_buses.insert(bus->number);
    }
    return unique_buses;
}

const std::optional<graph::Router<double>::RouteInfo> RequestHandler::GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const
{
    return router_.FindRoute(stop_from, stop_to);
}

const graph::DirectedWeightedGraph<double> &RequestHandler::GetRouterGraph() const
{
    return router_.GetGraph();
}

bool RequestHandler::ExistingBus(const std::string_view bus_number) const
{
    return catalogue_.FindRoute(bus_number);
}

bool RequestHandler::ExistingStop(const std::string_view stop_name) const
{
    return catalogue_.FindStop(stop_name);
}

json::Node RequestHandler::FindStopRequestProcessing(const json::Dict &request_map)
{
    int id = request_map.at("id").AsInt();
    const std::string& name = request_map.at("name").AsString();
    if (const transportCatalog::Stop* stop = catalogue_.FindStop(name)) {
        json::Array buses_array;
        const auto& buses_on_stop = catalogue_.FindStop(stop->name)->buses;
        buses_array.reserve(buses_on_stop.size());
        for (auto& item : buses_on_stop) {
            buses_array.push_back(item->number);
        }
        return json::Node(json::Dict{
                                     {{"buses"},{std::move(buses_array)}},
            {{"request_id"},{id}}
        });
    }
    else {
        return json::Builder{}.StartDict()
            .Key("error_message").Value("not found")
            .Key("request_id").Value(id)
            .EndDict().Build();
    }
}

json::Node RequestHandler::FindBusRequestProcessing(const json::Dict &request_map)
{
    int id = request_map.at("id").AsInt();
    const std::string& name = request_map.at("name").AsString();

    if (!ExistingBus(name)) {
        return json::Builder{}.StartDict()
            .Key("error_message").Value("not found")
            .Key("request_id").Value(id)
            .EndDict().Build();
    }


    transportCatalog::RouteInfo info = GetRoutStat(name);

        return json::Node(json::Dict{
            {{"route_length"},{info.route_length}},
            {{"unique_stop_count"},{static_cast<int>(info.unique_stops_count)}},
            {{"stop_count"},{static_cast<int>(info.stops_count)}},
            {{"curvature"},{info.curvature}},
            {{"request_id"},{id}}
        });
//    }
}

json::Node RequestHandler::BuildMapRequestProcessing(const json::Dict &request_map)
{
        int id = request_map.at("id").AsInt();
        svg::Document map = RenderMap();
        std::ostringstream strm;
        map.Render(strm);
        return json::Node(json::Dict{
            {{"map"},{strm.str()}},
            {{"request_id"},{id}}
        });
}

json::Node RequestHandler::BuildRouteRequestProcessing(const json::Dict &request_map)
{
        int id = request_map.at("id").AsInt();
        const std::string& name_from = request_map.at("from").AsString();
        const std::string& name_to = request_map.at("to").AsString();
        if (const transportCatalog::Stop* stop_from = catalogue_.FindStop(name_from)) {
            if (const transportCatalog::Stop* stop_to = catalogue_.FindStop(name_to)) {
                if (auto ri = router_.GetRouteInfo(stop_from, stop_to)) {
                    auto [wieght, edges] = ri.value();
                    return json::Node(json::Dict{
                        {{"items"},{router_.GetEdgesItems(edges)}},
                        {{"total_time"},{wieght}},
                        {{"request_id"},{id}}
                    });
                }
            }
        }
        return json::Builder{}.StartDict()
            .Key("error_message").Value("not found")
            .Key("request_id").Value(id)
            .EndDict().Build();
}


const transportCatalog::RouteInfo RequestHandler::GetRoutStat(const std::string_view route_number) const
{
    const transportCatalog::Bus* bus = catalogue_.FindRoute(route_number);

    if (bus == nullptr) {
        throw std::invalid_argument("bus not found2");
    }

    return bus->routeInfo;
}


void RequestHandler::ProcessRequests(const json::Node &request , std::ostream& output) {

    catalogue_.CalcRoutesInfo();

    const json::Array& arr = request.AsArray();
    json::Array output_array;
    output_array.reserve(arr.size());
    for (auto& request_node : arr) {
        const json::Dict& request_map = request_node.AsMap();
        const std::string& type = request_map.at("type").AsString();
        if (type == "Stop") {
            output_array.push_back(FindStopRequestProcessing(request_map));
            continue;
        }
        if (type == "Bus") {
            output_array.push_back(FindBusRequestProcessing(request_map));
            continue;
        }
        if (type == "Map") {
            output_array.push_back(BuildMapRequestProcessing(request_map));
            continue;
        }
        if (type == "Route") {
            output_array.push_back(BuildRouteRequestProcessing(request_map));
            continue;
        }
    }
    json::Print(json::Document(json::Node(std::move(output_array))), output);
}


svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.GetSortedAllBuses());
}

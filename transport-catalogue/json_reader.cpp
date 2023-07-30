#include "json_reader.h"

using namespace std;

const json::Node& JsonReader::GetBaseRequest() const {
    if (input_.GetRoot().AsDict().count("base_requests"s))
        return input_.GetRoot().AsDict().at("base_requests"s);
    else return dumm_;
}

const json::Node& JsonReader::GetStatRequest() const {
    if (input_.GetRoot().AsDict().count("stat_requests"s))
        return input_.GetRoot().AsDict().at("stat_requests"s);
    else return dumm_;
}

const json::Node& JsonReader::GetRenderSettings() const {
    if (input_.GetRoot().AsDict().count("render_settings"s))
        return input_.GetRoot().AsDict().at("render_settings"s);
    else return dumm_;
}

<<<<<<< HEAD
const json::Node& JsonReader::GetRoutingSettings() const {
    if (input_.GetRoot().AsDict().count("routing_settings"s))
        return input_.GetRoot().AsDict().at("routing_settings"s);
    else return dumm_;
}

const json::Node& JsonReader::GetSerializationSettings() const {
    if (input_.GetRoot().AsDict().count("serialization_settings"s))
        return input_.GetRoot().AsDict().at("serialization_settings"s);
    else return dumm_;
}
=======
void JsonReader::FillCatalogue(transportCatalog::TransportCatalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
>>>>>>> parent of abd3691 (my favorite progaramm)

void JsonReader::FillCatalogue(transportCatalogue::Catalogue& catalogue) const {
    const json::Array& arr = GetBaseRequest().AsArray();
    StopsDistMap stop_to_stops_distance;
    BusesInfoMap buses_info;
    for (const auto& request_node : arr) {
        const json::Dict& request_map = request_node.AsDict();
        const string& type = request_map.at("type"s).AsString();
        if (type == "Stop"s) {
            ParseStopAddRequest(catalogue, request_map, stop_to_stops_distance);
        }
        if (type == "Bus"s) {
            ParseBusAddRequest(request_map, buses_info);
        }
    }
    SetStopsDistances(catalogue, stop_to_stops_distance);
    BusesAddProcess(catalogue, buses_info);
    SetFinals(catalogue, buses_info);
}

void JsonReader::ParseStopAddRequest(transportCatalogue::Catalogue& catalogue, const json::Dict& request_map,
                                     StopsDistMap& stop_to_stops_distance) const {
    const string& stop_name = request_map.at("name"s).AsString();
    catalogue.AddStop(stop_name, {
                                  request_map.at("latitude"s).AsDouble(),
                                  request_map.at("longitude"s).AsDouble() });
    const json::Dict& near_stops = request_map.at("road_distances"s).AsDict();
    for (const auto& [key_stop_name, dist_node] : near_stops) {
        stop_to_stops_distance[stop_name][key_stop_name] = dist_node.AsInt();
    }
}

void JsonReader::SetStopsDistances(transportCatalogue::Catalogue& catalogue,
                                   const StopsDistMap& stop_to_stops_distance) const {
    for (const auto& [stop, near_stops] : stop_to_stops_distance) {
        for (const auto& [stop_name, dist] : near_stops) {
            catalogue.SetDistance(catalogue.FindStop(stop), catalogue.FindStop(stop_name), dist);
        }
    }
}

void JsonReader::ParseBusAddRequest(const json::Dict& request_map, BusesInfoMap& buses_info) const {
    const string& bus_name = request_map.at("name"s).AsString();
    const json::Array& bus_stops = request_map.at("stops"s).AsArray();
    size_t stops_count = bus_stops.size();
    bool is_roundtrip = request_map.at("is_roundtrip"s).AsBool();
    buses_info[bus_name].is_circle = is_roundtrip;
    auto& stops = buses_info[bus_name].stops;
    if (stops_count > 0) stops.reserve(is_roundtrip ? stops_count : stops_count * 2);
    for (size_t i = 0; i < bus_stops.size(); ++i) {
        stops.push_back(bus_stops[i].AsString());
        if (i == bus_stops.size() - 1) {
            if (!is_roundtrip) {
                buses_info[bus_name].final_stop = bus_stops[i].AsString();
                for (int j = stops.size() - 2; j >= 0; --j) {
                    stops.push_back(stops[j]);
                }
            }
<<<<<<< HEAD
            else {
                buses_info[bus_name].final_stop = bus_stops[0].AsString();
=======
            catalogue.AddRoute(route_number, route_stops,is_round_);
        }
    }
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict &request_map) const
{
    renderer::RenderSettings render_settings;
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    render_settings.underlayer_color = ParceColor(request_map.at("underlayer_color"));

    //    if (request_map.at("underlayer_color").IsString()) render_settings.underlayer_color = request_map.at("underlayer_color").AsString();
    //    else if (request_map.at("underlayer_color").IsArray()) {
    //        const json::Array& underlayer_color = request_map.at("underlayer_color").AsArray();
    //        if (underlayer_color.size() == 3) {
    //            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
    //        }
    //        else if (underlayer_color.size() == 4) {
    //            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
    //        } else throw std::logic_error("wrong underlayer colortype");
    //    } else throw std::logic_error("wrong underlayer color");

    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        //        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        //        else if (color_element.IsArray()) {
        //            const json::Array& color_type = color_element.AsArray();
        //            if (color_type.size() == 3) {
        //                render_settings.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
        //            }
        //            else if (color_type.size() == 4) {
        //                render_settings.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
        //            } else throw std::logic_error("wrong color_palette type");
        //        } else throw std::logic_error("wrong color_palette");
        render_settings.color_palette.push_back(ParceColor(color_element));
    }

    return render_settings;
}

void JsonReader::ProcessRequests(const json::Node &stat_requests, RequestHandler &rh, std::ostream &out) const
{
//    json::Array result;
//    for (auto& request : stat_requests.AsArray()) {
//        const auto& request_map = request.AsMap();
//        const auto& type = request_map.at("type").AsString();
//        if (type == "Stop") result.push_back(PrintStop(request_map, rh).AsMap());
//        if (type == "Bus") result.push_back(PrintRoute(request_map, rh).AsMap());
//        if (type == "Map") result.push_back(PrintMap(request_map, rh).AsMap());
//    }

//    json::Print(json::Document{ result }, out);

    json::Array result;
    auto builder = json::Builder();
    builder.StartArray();
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        //if (type == "Stop") result.push_back(PrintStop(request_map, rh).AsMap());
        if (type == "Stop") builder.Value(PrintStop(request_map, rh).AsMap());
        //if (type == "Bus") result.push_back(PrintRoute(request_map, rh).AsMap());
        if (type == "Bus") builder.Value(PrintRoute(request_map, rh).AsMap());
        //if (type == "Map") result.push_back(PrintMap(request_map, rh).AsMap());
        if (type == "Map") builder.Value(PrintMap(request_map, rh).AsMap());
    }

    builder.EndArray();
    //json::Print(json::Document{ result }, out);
    json::Print(json::Document{ std::move(builder.Build()) }, out);

}

const json::Node JsonReader::PrintRoute(const json::Dict &request_map, RequestHandler &rh) const
{
//    json::Dict result;
//    const std::string& route_number = request_map.at("name").AsString();
//    result["request_id"] = request_map.at("id").AsInt();
//    if (!rh.ExistingBus(route_number)) {
//        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
//    }
//    else {
//        result["curvature"] = rh.GetRoutStat(route_number).curvature;
//        result["route_length"] = rh.GetRoutStat(route_number).route_length;
//        result["stop_count"] = static_cast<int>(rh.GetRoutStat(route_number).stops_count);
//        result["unique_stop_count"] = static_cast<int>(rh.GetRoutStat(route_number).unique_stops_count);
//    }
//    return json::Node{ result };
    auto builder = json::Builder();
    builder.StartDict();
    builder.Key("request_id").Value(request_map.at("id").AsInt());
    const std::string& route_number = request_map.at("name").AsString();

    if (!rh.ExistingBus(route_number)) {
        builder.Key("error_message").Value("not found");
    }
    else {
        builder.Key("curvature").Value(rh.GetRoutStat(route_number).curvature);
        builder.Key("route_length").Value(rh.GetRoutStat(route_number).route_length);
        builder.Key("stop_count").Value(static_cast<int>(rh.GetRoutStat(route_number).stops_count));
        builder.Key("unique_stop_count").Value(static_cast<int>(rh.GetRoutStat(route_number).unique_stops_count));
    }

    builder.EndDict();
    return std::move(builder.Build());

}

const json::Node JsonReader::PrintStop(const json::Dict &request_map, RequestHandler &rh) const
{
    //    json::Dict result;
    //        const std::string& stop_name = request_map.at("name").AsString();
    //        result["request_id"] = request_map.at("id").AsInt();
    //        if (!rh.ExistingStop(stop_name)) {
    //            result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    //        }
    //        else {
    //            json::Array buses;
    //            for (auto& bus : rh.GetBusesByStop(stop_name)) {
    //                buses.push_back(bus);
    //            }
    //            result["buses"] = buses;
    //        }

    //        return json::Node{ result };

    auto builder = json::Builder();
    builder.StartDict();
    builder.Key("request_id").Value(request_map.at("id").AsInt());

    const std::string& stop_name = request_map.at("name").AsString();

    if (!rh.ExistingStop(stop_name)) {
        builder.Key("error_message").Value("not found");
    }
    else {
        builder.Key("buses");
        builder.StartArray();
        for (auto& bus : rh.GetBusesByStop(stop_name)) {
            builder.Value(bus);
        }
        builder.EndArray();
    }

    builder.EndDict();
    return std::move(builder.Build());
}

const json::Node JsonReader::PrintMap(const json::Dict &request_map, RequestHandler &rh) const
{
    //    json::Dict result;
    //    result["request_id"] = request_map.at("id").AsInt();
    //    std::ostringstream strm;
    //    svg::Document map = rh.RenderMap();
    //    map.Render(strm);
    //    result["map"] = strm.str();

    //    return json::Node{ result };

    auto builder = json::Builder();
    builder.StartDict();
    builder.Key("request_id").Value(request_map.at("id").AsInt());
    std::ostringstream strm;
    svg::Document map = rh.RenderMap();
    map.Render(strm);
    builder.Key("map").Value(strm.str());
    builder.EndDict();
    return std::move(builder.Build());
}

svg::Color JsonReader::ParceColor(const json::Node &node) const{
    if (node.IsString())
        return node.AsString();
    else
        if (node.IsArray())
        {
            const json::Array& underlayer_color = node.AsArray();
            if (underlayer_color.size() == 3)
            {
                return svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
>>>>>>> parent of abd3691 (my favorite progaramm)
            }
        }
    }
}

void JsonReader::BusesAddProcess(transportCatalogue::Catalogue& catalogue, const BusesInfoMap& buses_info) const {
    for (const auto& [name, info] : buses_info) {
        vector<transportCatalogue::Stop*> stop_ptrs;
        const auto& stops = info.stops;
        stop_ptrs.reserve(stops.size());
        for (const auto& stop : stops) {
            stop_ptrs.push_back(catalogue.FindStop(stop));
        }
        catalogue.AddBus(static_cast<string>(name), stop_ptrs, info.is_circle);
    }
}

void JsonReader::SetFinals(transportCatalogue::Catalogue& catalogue, const BusesInfoMap& buses_info) const {
    for (auto& [bus_name, info] : buses_info) {
        if (domain::Bus* bus = catalogue.FindBus(bus_name)) {
            if (domain::Stop* stop = catalogue.FindStop(info.final_stop)) {
                bus->final_stop = stop;
            }
        }
    }
}

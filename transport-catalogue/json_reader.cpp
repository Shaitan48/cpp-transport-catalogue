#include "json_reader.h"

const json::Node& JsonReader::GetBaseRequests() const {
    if (!input_.GetRoot().AsMap().count("base_requests")) return dummy_;
    return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& JsonReader::GetStatRequests() const {
    if (!input_.GetRoot().AsMap().count("stat_requests")) return dummy_;
    return input_.GetRoot().AsMap().at("stat_requests");
}

renderer::RenderSettings JsonReader::GetRenderSettings()const
{
    json::Node render_settings = input_.GetRoot().AsMap().at("render_settings");
    renderer::RenderSettings render_settings_;
    //if (render_settings.IsNull()) return renderer::RenderSettings;
    const json::Dict& settings_map = render_settings.AsMap();
    render_settings_.width = settings_map.at("width").AsDouble();
    render_settings_.height = settings_map.at("height").AsDouble();
    render_settings_.padding = settings_map.at("padding").AsDouble();
    render_settings_.stop_radius = settings_map.at("stop_radius").AsDouble();
    render_settings_.line_width = settings_map.at("line_width").AsDouble();
    render_settings_.bus_label_font_size = settings_map.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = settings_map.at("bus_label_offset").AsArray();
    render_settings_.bus_label_offset = { bus_label_offset[0].AsDouble(),
                                         bus_label_offset[1].AsDouble() };
    render_settings_.stop_label_font_size = settings_map.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = settings_map.at("stop_label_offset").AsArray();
    render_settings_.stop_label_offset = { stop_label_offset[0].AsDouble(),
                                          stop_label_offset[1].AsDouble() };
    if (settings_map.at("underlayer_color").IsArray()) {
        const json::Array& arr = settings_map.at("underlayer_color").AsArray();
        if (arr.size() == 3) {
            svg::Rgb rgb_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
            render_settings_.underlayer_color = rgb_colors;
        }
        else if (arr.size() == 4) {
            svg::Rgba rgba_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
            render_settings_.underlayer_color = rgba_colors;
        }
        else throw std::logic_error("Strange array");
    }
    else if (settings_map.at("underlayer_color").IsString()) {
        render_settings_.underlayer_color = settings_map.at("underlayer_color").AsString();
    }
    else throw std::logic_error("Error color identity");
    render_settings_.underlayer_width = settings_map.at("underlayer_width").AsDouble();
    const json::Array& color_palette = settings_map.at("color_palette").AsArray();
    for (const json::Node& node : color_palette) {
        if (node.IsArray()) {
            const json::Array& arr = node.AsArray();
            if (arr.size() == 3) {
                svg::Rgb rgb_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
                render_settings_.color_palette.push_back(rgb_colors);
            }
            else if (arr.size() == 4) {
                svg::Rgba rgba_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
                render_settings_.color_palette.push_back(rgba_colors);
            }
            else throw std::logic_error("Strange array");
        }
        else if (node.IsString()) {
            render_settings_.color_palette.push_back(node.AsString());
        }
        else throw std::logic_error("Error palette color identity");
    }
    return render_settings_;

}

transportCatalog::routerSettings JsonReader::GetRoutingSettings() const
{
    transportCatalog::routerSettings result;
    if (!input_.GetRoot().AsMap().count("routing_settings")) return result;
    result.bus_velocity_ = input_.GetRoot().AsMap().at("routing_settings").AsMap().at("bus_velocity").AsDouble();
    result.bus_wait_time_ = input_.GetRoot().AsMap().at("routing_settings").AsMap().at("bus_wait_time").AsInt();
    return result;
}

const json::Node &JsonReader::GetSerializationSettings() const
{
    if (!input_.GetRoot().AsMap().count("serialization_settings")) return dummy_;
    return input_.GetRoot().AsMap().at("serialization_settings");
}

void JsonReader::FillCatalogue(transportCatalog::TransportCatalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();

    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop")
        {
            std::string_view stop_name = request_stops_map.at("name").AsString();
            geo::Coordinates coordinates = { request_stops_map.at("latitude").AsDouble(), request_stops_map.at("longitude").AsDouble() };
            catalogue.AddStop(stop_name, coordinates);
        }

    }

    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop")
        {
            std::string_view stop_name_source  = request_stops_map.at("name").AsString();
            auto stop_distances = request_stops_map.at("road_distances").AsMap();
            transportCatalog::Stop* souce = catalogue.FindStop(stop_name_source);
            for(auto& [stop_name_dest, dist] : stop_distances){
                const transportCatalog::Stop* destination = catalogue.FindStop(stop_name_dest);
                catalogue.SetDistance(souce,destination,dist.AsDouble());
            }
        }
    }

    for (auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsMap();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            bool is_round_ = true;
            std::string_view route_number = request_bus_map.at("name").AsString();
            std::vector<std::string> route_stops;
            for (auto& stop : request_bus_map.at("stops").AsArray()) {
                route_stops.push_back(stop.AsString());
            }
            if(!request_bus_map.at("is_roundtrip").AsBool()){
                route_stops.reserve(route_stops.size() * 2 - 1);
                auto it = route_stops.rbegin()+1;
                while(it!=route_stops.rend()){
                    route_stops.push_back(*it);
                    it++;
                }
                is_round_ = false;
            }
            catalogue.AddRoute(route_number, route_stops,is_round_);
        }
    }

    //catalogue.CalcRoutesInfo();


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

    return renderer::MapRenderer(render_settings);
}

transportCatalog::Router JsonReader::FillRoutingSettings(const json::Node &settings) const
{
    transportCatalog::Router routing_settings;
        return transportCatalog::Router{ settings.AsMap().at("bus_wait_time").AsInt(), settings.AsMap().at("bus_velocity").AsDouble() };
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
        if (type == "Route") builder.Value(PrintRouting(request_map, rh).AsMap());
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
        try {
            auto stat = rh.GetRoutStat(route_number);
            builder.Key("curvature").Value(stat.curvature);
            builder.Key("route_length").Value(stat.route_length);
            builder.Key("stop_count").Value(static_cast<int>(stat.stops_count));
            builder.Key("unique_stop_count").Value(static_cast<int>(stat.unique_stops_count));
        } catch (...) {
            builder.Key("error_message").Value("not found");
        }

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

const json::Node JsonReader::PrintRouting(const json::Dict &request_map, RequestHandler &rh) const
{
    json::Node result;
       const int id = request_map.at("id").AsInt();
       const std::string_view stop_from = request_map.at("from").AsString();
       const std::string_view stop_to = request_map.at("to").AsString();

       if (!(rh.ExistingStop(stop_from) and rh.ExistingStop(stop_to))) {
        auto builder = json::Builder();
        builder.StartDict()
            .Key("error_message").Value("not found")
                     .EndDict();
        return builder.Build();
       }

       const auto& routing = rh.GetOptimalRoute(stop_from, stop_to);

       if (!routing) {
           result = json::Builder{}
               .StartDict()
                   .Key("request_id").Value(id)
                   .Key("error_message").Value("not found")
               .EndDict()
           .Build();
       }
       else {
           json::Array items;
           double total_time = 0.0;
           items.reserve(routing.value().edges.size());
           for (auto& edge_id : routing.value().edges) {
               const graph::Edge<double> edge = rh.GetRouterGraph().GetEdge(edge_id);
               if (edge.quality == 0) {
                   items.emplace_back(json::Node(json::Builder{}
                       .StartDict()
                           .Key("stop_name").Value(edge.name)
                           .Key("time").Value(edge.weight)
                           .Key("type").Value("Wait")
                       .EndDict()
                   .Build()));

                   total_time += edge.weight;
               }
               else {
                   items.emplace_back(json::Node(json::Builder{}
                       .StartDict()
                           .Key("bus").Value(edge.name)
                           .Key("span_count").Value(static_cast<int>(edge.quality))
                           .Key("time").Value(edge.weight)
                           .Key("type").Value("Bus")
                       .EndDict()
                   .Build()));

                   total_time += edge.weight;
               }
           }

           result = json::Builder{}
               .StartDict()
                   .Key("request_id").Value(id)
                   .Key("total_time").Value(total_time)
                   .Key("items").Value(items)
               .EndDict()
           .Build();
       }

       return result;
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
            }
            else
                if (underlayer_color.size() == 4)
                {
                    return svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
                } else
                    throw std::logic_error("wrong underlayer/color_palette type");
        }
        else throw std::logic_error("wrong underlayer/color_palette color");
}

json::Node ConvertToNode(const svg::Point &p) {
        return json::Node(json::Array{ {p.x}, {p.y} });
}

json::Node ConvertToNode(const svg::Color &c) {
        if (std::holds_alternative<std::string>(c)) {
                return json::Node(std::get<std::string>(c));
        }
        else if (std::holds_alternative<svg::Rgb>(c)) {
                const svg::Rgb& rgb = std::get<svg::Rgb>(c);
                return json::Node(json::Array{ {rgb.red}, {rgb.green}, {rgb.blue} });
        }
        else if (std::holds_alternative<svg::Rgba>(c)) {
                const svg::Rgba& rgba = std::get<svg::Rgba>(c);
                return json::Node(json::Array{ {rgba.red}, {rgba.green}, {rgba.blue}, {rgba.opacity} });
        }
        else
                return json::Node("none");
}

json::Node ConvertToNode(const std::vector<svg::Color> &cv) {
        json::Array result;
        result.reserve(cv.size());
        for (const auto& c : cv) {
                result.emplace_back(ConvertToNode(c));
        }
        return json::Node(std::move(result));
}

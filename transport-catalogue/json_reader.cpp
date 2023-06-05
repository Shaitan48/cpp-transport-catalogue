#include "json_reader.h"

const json::Node& JsonReader::GetBaseRequests() const {
    if (!input_.GetRoot().AsMap().count("base_requests")) return dummy_;
    return input_.GetRoot().AsMap().at("base_requests");
}

const json::Node& JsonReader::GetStatRequests() const {
    if (!input_.GetRoot().AsMap().count("stat_requests")) return dummy_;
    return input_.GetRoot().AsMap().at("stat_requests");
}

const json::Node &JsonReader::GetRenderSettings() const
{
    if (!input_.GetRoot().AsMap().count("render_settings")) return dummy_;
    return input_.GetRoot().AsMap().at("render_settings");

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
            const transportCatalog::Stop* souce = catalogue.FindStop(stop_name_source);
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
    json::Array result;
       for (auto& request : stat_requests.AsArray()) {
           const auto& request_map = request.AsMap();
           const auto& type = request_map.at("type").AsString();
           if (type == "Stop") result.push_back(PrintStop(request_map, rh).AsMap());
           if (type == "Bus") result.push_back(PrintRoute(request_map, rh).AsMap());
           if (type == "Map") result.push_back(PrintMap(request_map, rh).AsMap());
       }

       json::Print(json::Document{ result }, out);
}

const json::Node JsonReader::PrintRoute(const json::Dict &request_map, RequestHandler &rh) const
{
    json::Dict result;
       const std::string& route_number = request_map.at("name").AsString();
       result["request_id"] = request_map.at("id").AsInt();
       if (!rh.ExistingBus(route_number)) {
           result["error_message"] = json::Node{ static_cast<std::string>("not found") };
       }
       else {
           result["curvature"] = rh.GetRoutStat(route_number).curvature;
           result["route_length"] = rh.GetRoutStat(route_number).route_length;
           result["stop_count"] = static_cast<int>(rh.GetRoutStat(route_number).stops_count);
           result["unique_stop_count"] = static_cast<int>(rh.GetRoutStat(route_number).unique_stops_count);
       }

       return json::Node{ result };
}

const json::Node JsonReader::PrintStop(const json::Dict &request_map, RequestHandler &rh) const
{
    json::Dict result;
        const std::string& stop_name = request_map.at("name").AsString();
        result["request_id"] = request_map.at("id").AsInt();
        if (!rh.ExistingStop(stop_name)) {
            result["error_message"] = json::Node{ static_cast<std::string>("not found") };
        }
        else {
            json::Array buses;
            for (auto& bus : rh.GetBusesByStop(stop_name)) {
                buses.push_back(bus);
            }
            result["buses"] = buses;
        }

        return json::Node{ result };
}

const json::Node JsonReader::PrintMap(const json::Dict &request_map, RequestHandler &rh) const
{
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = rh.RenderMap();
    map.Render(strm);
    result["map"] = strm.str();

    return json::Node{ result };
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

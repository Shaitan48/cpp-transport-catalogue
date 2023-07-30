/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

#include "map_renderer.h"

namespace renderer {

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

MapRenderer::MapRenderer(const json::Node &render_settings)
{
    if (render_settings.IsNull()) return;
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
}

std::vector<svg::Polyline> MapRenderer::GetRouteLines(const std::map<std::string_view, const transportCatalog::Bus*>& buses, const SphereProjector& sp) const {
    std::vector<svg::Polyline> result;
    size_t color_num = 0;
    for (const auto& [bus_number, bus] : buses) {
        if (bus->stops.empty()) continue;
        std::vector<const transportCatalog::Stop*> route_stops{ bus->stops.begin(), bus->stops.end() };
        svg::Polyline line;
        for (const auto& stop : route_stops) {
            line.AddPoint(sp(stop->coordinates));
        }
        line.SetStrokeColor(render_settings_.color_palette[color_num]);
        line.SetFillColor("none");
        line.SetStrokeWidth(render_settings_.line_width);
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        if (color_num < (render_settings_.color_palette.size() - 1)) ++color_num;
        else color_num = 0;

        result.push_back(line);
    }

    return result;
}

std::vector<svg::Text> MapRenderer::GetBusLabel(const std::map<std::string_view, const transportCatalog::Bus *> &buses, const SphereProjector &sp) const
{
    std::vector<svg::Text> result;
    size_t color_num = 0;
    for (const auto& [bus_number, bus] : buses) {
        if (bus->stops.empty()) continue;
        svg::Text text;
        svg::Text underlayer;
        text.SetPosition(sp(bus->stops[0]->coordinates));
        text.SetOffset(render_settings_.bus_label_offset);
        text.SetFontSize(render_settings_.bus_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetFontWeight("bold");
        text.SetData(bus->number);
        text.SetFillColor(render_settings_.color_palette[color_num]);
        if (color_num < (render_settings_.color_palette.size() - 1)) ++color_num;
        else color_num = 0;

        underlayer.SetPosition(sp(bus->stops[0]->coordinates));
        underlayer.SetOffset(render_settings_.bus_label_offset);
        underlayer.SetFontSize(render_settings_.bus_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetFontWeight("bold");
        underlayer.SetData(bus->number);
        underlayer.SetFillColor(render_settings_.underlayer_color);
        underlayer.SetStrokeColor(render_settings_.underlayer_color);
        underlayer.SetStrokeWidth(render_settings_.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        result.push_back(underlayer);
        result.push_back(text);

        if (bus->is_round == false && bus->stops[0] != bus->stops[bus->stops.size() /2]) {
            svg::Text text2 {text};
            svg::Text underlayer2 {underlayer};
            text2.SetPosition(sp(bus->stops[bus->stops.size()/2]->coordinates));
            underlayer2.SetPosition(sp(bus->stops[bus->stops.size()/2]->coordinates));

            result.push_back(underlayer2);
            result.push_back(text2);
        }
    }

    return result;
}

std::vector<svg::Circle> MapRenderer::GetStopsSymbols(const std::map<std::string_view, const transportCatalog::Stop *> &stops, const SphereProjector &sp) const
{
    std::vector<svg::Circle> result;
    for (const auto& [stop_name, stop] : stops) {
        svg::Circle symbol;
        symbol.SetCenter(sp(stop->coordinates));
        symbol.SetRadius(render_settings_.stop_radius);
        symbol.SetFillColor("white");

        result.push_back(symbol);
    }

    return result;
}

std::vector<svg::Text> MapRenderer::GetStopsLabels(const std::map<std::string_view, const transportCatalog::Stop *> &stops, const SphereProjector &sp) const
{
    std::vector<svg::Text> result;
    svg::Text text;
    svg::Text underlayer;
    for (const auto& [stop_name, stop] : stops) {
        text.SetPosition(sp(stop->coordinates));
        text.SetOffset(render_settings_.stop_label_offset);
        text.SetFontSize(render_settings_.stop_label_font_size);
        text.SetFontFamily("Verdana");
        text.SetData(stop->name);
        text.SetFillColor("black");

        underlayer.SetPosition(sp(stop->coordinates));
        underlayer.SetOffset(render_settings_.stop_label_offset);
        underlayer.SetFontSize(render_settings_.stop_label_font_size);
        underlayer.SetFontFamily("Verdana");
        underlayer.SetData(stop->name);
        underlayer.SetFillColor(render_settings_.underlayer_color);
        underlayer.SetStrokeColor(render_settings_.underlayer_color);
        underlayer.SetStrokeWidth(render_settings_.underlayer_width);
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        result.push_back(underlayer);
        result.push_back(text);
    }

    return result;
}

svg::Document MapRenderer::GetSVG(const std::map<std::string_view, const transportCatalog::Bus*>& buses) const {
    svg::Document result;
    std::vector<geo::Coordinates> route_stops_coord;
    std::map<std::string_view, const transportCatalog::Stop*> all_stops;
    for (const auto& [bus_number, bus] : buses) {
        if (bus->stops.empty()) continue;
        for (const auto& stop : bus->stops) {
            route_stops_coord.push_back(stop->coordinates);
            all_stops[stop->name] = stop;
        }
    }
    SphereProjector sp(route_stops_coord.begin(), route_stops_coord.end(), render_settings_.width, render_settings_.height, render_settings_.padding);
    for (const auto& line : GetRouteLines(buses, sp)) {
        result.Add(line);
    }

    for (const auto& line : GetBusLabel(buses, sp)) {
        result.Add(line);
    }

    for (const auto& line : GetStopsSymbols(all_stops, sp)) {
        result.Add(line);
    }

    for (const auto& line : GetStopsLabels(all_stops, sp)) {
        result.Add(line);
    }

    return result;
}

json::Node MapRenderer::GetRenderSettings() const
{
    return json::Node(json::Dict{
                                 {{"width"},{render_settings_.width}},
                                 {{"height"},{render_settings_.height}},
                                 {{"padding"},{render_settings_.padding}},
                                 {{"stop_radius"},{render_settings_.stop_radius}},
                                 {{"line_width"},{render_settings_.line_width}},
                                 {{"bus_label_font_size"},{render_settings_.bus_label_font_size}},
                                 {{"bus_label_offset"},ConvertToNode(render_settings_.bus_label_offset)},
                                 {{"stop_label_font_size"},{render_settings_.stop_label_font_size}},
                                 {{"stop_label_offset"},ConvertToNode(render_settings_.stop_label_offset)},
                                 {{"underlayer_color"},ConvertToNode(render_settings_.underlayer_color)},
                                 {{"underlayer_width"},{render_settings_.underlayer_width}},
                                 {{"color_palette"},ConvertToNode(render_settings_.color_palette)},
                                 });
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

} // namespace renderer

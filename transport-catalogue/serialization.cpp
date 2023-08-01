#include "serialization.h"

using namespace std;

void Serialize(const transportCatalog::TransportCatalogue &catalog,
               const renderer::MapRenderer& renderer, const transportCatalog::Router &router,
               std::ostream& output) {
    serialize::TransportCatalogue database;
    for (const auto& [name, s] : catalog.GetSortedAllStops()) {
        *database.add_stop() = Serialize(s);
    }
    for (const auto& [name, b] : catalog.GetSortedAllBuses()) {
        *database.add_bus() = Serialize(b);
    }
    *database.mutable_render_settings() = GetRenderSettingSerialize(renderer.GetRenderSettings());
    *database.mutable_router() = Serialize(router);
    database.SerializeToOstream(&output);
}

serialize::Stop Serialize(const transportCatalog::Stop *stop) {
    serialize::Stop result;
    result.set_name(stop->name);
    result.add_coordinate(stop->coordinates.lat);
    result.add_coordinate(stop->coordinates.lng);
    for (const auto& [n, d] : stop->stop_distances) {
        result.add_stop_(static_cast<string>(n));
        result.add_distance(d);
    }
    return result;
}

serialize::Bus Serialize(const transportCatalog::Bus *bus) {
    serialize::Bus result;
    result.set_number(bus->number);
    for (const auto& s : bus->stops) {
        result.add_stop(s->name);
    }
    result.set_is_round(bus->is_round);
    //    if (bus->final_stop)
    //        result.set_final_stop(bus->final_stop->name);
    return result;
}

serialize::Point GetPointSerialize(const svg::Point& p) {
    serialize::Point result;
    result.set_x(p.x);
    result.set_y(p.y);
    return result;
}

serialize::Color GetColorSerialize(const svg::Color& color) {
    serialize::Color result;
    switch(color.index()){
    case(3):{
        serialize::RGBA rgba;
        rgba.set_red(std::get<svg::Rgba>(color).red);
        rgba.set_green(std::get<svg::Rgba>(color).green);
        rgba.set_blue(std::get<svg::Rgba>(color).blue);
        rgba.set_opacity(std::get<svg::Rgba>(color).opacity);
        *result.mutable_rgba() = rgba;
        break;
    }
    case(2):{
        serialize::RGB rgb;
        rgb.set_red(std::get<svg::Rgb>(color).red);
        rgb.set_green(std::get<svg::Rgb>(color).green);
        rgb.set_blue(std::get<svg::Rgb>(color).blue);
        *result.mutable_rgb() = rgb;
        break;
    }
    case(1):{
        result.set_name(std::get<string>(color));
        break;
    }
    case(0):{
        break;
    }
    }
    return result;
}

serialize::RenderSettings GetRenderSettingSerialize(renderer::RenderSettings renderer_settings) {
    //const json::Dict& rs_map = render_settings.AsMap();
    serialize::RenderSettings result;
    result.set_width(renderer_settings.width);
    result.set_height(renderer_settings.height);
    result.set_padding(renderer_settings.padding);
    result.set_stop_radius(renderer_settings.stop_radius);
    result.set_line_width(renderer_settings.line_width);
    result.set_bus_label_font_size(renderer_settings.bus_label_font_size);
    *result.mutable_bus_label_offset() = GetPointSerialize(renderer_settings.bus_label_offset);
    result.set_stop_label_font_size(renderer_settings.stop_label_font_size);
    *result.mutable_stop_label_offset() = GetPointSerialize(renderer_settings.stop_label_offset);
    *result.mutable_underlayer_color() = GetColorSerialize(renderer_settings.underlayer_color);
    result.set_underlayer_width(renderer_settings.underlayer_width);
    for (const auto& c : renderer_settings.color_palette) {
        *result.add_color_palette() = GetColorSerialize(c);
    }
    return result;
}

serialize::RouterSettings GetRouterSettingSerialize(transportCatalog::routerSettings router_settings) {
    //const json::Dict& rs_map = router_settings.AsMap();
    serialize::RouterSettings result;
    result.set_bus_wait_time(router_settings.bus_wait_time_);
    result.set_bus_velocity(router_settings.bus_velocity_);
    return result;
}

serialize::Graph GetGraphSerialize(const graph::DirectedWeightedGraph<double>& g) {
    serialize::Graph result;
    size_t vertex_count = g.GetVertexCount();
    size_t edge_count = g.GetEdgeCount();
    for (size_t i = 0; i < edge_count; ++i) {
        const graph::Edge<double>& edge = g.GetEdge(i);
        serialize::Edge s_edge;
        s_edge.set_name(edge.name);
        s_edge.set_quality(edge.quality);
        s_edge.set_from(edge.from);
        s_edge.set_to(edge.to);
        s_edge.set_weight(edge.weight);
        *result.add_edge() = s_edge;
    }
    for (size_t i = 0; i < vertex_count; ++i) {
        serialize::Vertex vertex;
        for (const auto& edge_id : g.GetIncidentEdges(i)) {
            vertex.add_edge_id(edge_id);
        }
        *result.add_vertex() = vertex;
    }
    return result;
}

serialize::Router Serialize(const transportCatalog::Router &router) {
    serialize::Router result;
    *result.mutable_router_settings() = GetRouterSettingSerialize(router.GetSettings());
    *result.mutable_graph() = GetGraphSerialize(router.GetGraph());
    for (const auto& [n, id] : router.GetStopIds()) {
        serialize::StopId si;
        si.set_name(n);
        si.set_id(id);
        *result.add_stop_id() = si;
    }
    return result;
}

void SetStopsDistances(transportCatalog::TransportCatalogue& catalog, const serialize::TransportCatalogue& database) {
    for (size_t i = 0; i < database.stop_size(); ++i) {
        const serialize::Stop& stop_i = database.stop(i);
        transportCatalog::Stop* from = catalog.FindStop(stop_i.name());
        for (size_t j = 0; j < stop_i.stop__size(); ++j) {
            catalog.SetDistance(from, catalog.FindStop(stop_i.stop_(j)), stop_i.distance(j));
        }
    }
}

void AddStopFromDB(transportCatalog::TransportCatalogue& catalog, const serialize::TransportCatalogue& database) {
    for (size_t i = 0; i < database.stop_size(); ++i) {
        const serialize::Stop& stop_i = database.stop(i);
        geo::Coordinates pos = {stop_i.coordinate(0), stop_i.coordinate(1)};
        catalog.AddStop(stop_i.name(), pos);
    }
    SetStopsDistances(catalog, database);
}

void AddBusFromDB(transportCatalog::TransportCatalogue& catalog, const serialize::TransportCatalogue& database) {
    for (size_t i = 0; i < database.bus_size(); ++i) {
        const serialize::Bus& bus_i = database.bus(i);
        std::vector<transportCatalog::Stop*> stops(bus_i.stop_size());
        for (size_t j = 0; j < stops.size(); ++j) {
            stops[j] = catalog.FindStop(bus_i.stop(j));
        }
        catalog.AddRoute(bus_i.number(), stops, bus_i.is_round());
    }
}

svg::Point ConvertToPoint(const serialize::Point& p) {
    return svg::Point(p.x(), p.y());
}

svg::Color ConvertToColor(const serialize::Color& c) {
    if (!c.name().empty()) {
        return svg::Color(c.name());
    }
    else if (c.has_rgb()) {
        const serialize::RGB& rgb = c.rgb();
        return svg::Rgb(rgb.red(), rgb.green(), rgb.blue());
    }
    else if (c.has_rgba()) {
        const serialize::RGBA& rgba = c.rgba();
        return svg::Rgba(rgba.red(), rgba.green(), rgba.blue(), rgba.opacity());
    }
    else
        return svg::NoneColor;
}

std::vector<svg::Color> ConvertToPalete(const google::protobuf::RepeatedPtrField<serialize::Color>& cv) {
    std::vector<svg::Color> result;
    result.reserve(cv.size());
    for (const auto& c : cv) {
        result.emplace_back(ConvertToColor(c));
    }
    return result;
}

renderer::RenderSettings GetRenderSettingsFromDB(const serialize::TransportCatalogue& database) {
    const serialize::RenderSettings& rs = database.render_settings();
    renderer::RenderSettings renderer_settings;
    renderer_settings.width = rs.width();
    renderer_settings.height = rs.height();
    renderer_settings.padding = rs.padding();
    renderer_settings.stop_radius = rs.stop_radius();
    renderer_settings.line_width = rs.line_width();
    renderer_settings.bus_label_font_size = rs.bus_label_font_size();
    renderer_settings.bus_label_offset = svg::Point(rs.bus_label_offset().x(),rs.bus_label_offset().y());
    renderer_settings.stop_label_font_size = rs.stop_label_font_size();
    renderer_settings.stop_label_offset = svg::Point(rs.stop_label_offset().x(),rs.stop_label_offset().y());
    renderer_settings.underlayer_color = ConvertToColor(rs.underlayer_color());
    renderer_settings.underlayer_width = rs.underlayer_width();
    renderer_settings.color_palette = ConvertToPalete(rs.color_palette());
    return renderer_settings;
    /*
    return json::Node(json::Dict{
                                 {{"width"s},{ rs.width() }},
                                 {{"height"s},{ rs.height() }},
                                 {{"padding"s},{ rs.padding() }},
                                 {{"stop_radius"s},{ rs.stop_radius() }},
                                 {{"line_width"s},{ rs.line_width() }},
                                 {{"bus_label_font_size"s},{ rs.bus_label_font_size() }},
                                 {{"bus_label_offset"s},ConvertToNode(rs.bus_label_offset())},
                                 {{"stop_label_font_size"s},{rs.stop_label_font_size()}},
                                 {{"stop_label_offset"s},ConvertToNode(rs.stop_label_offset())},
                                 {{"underlayer_color"s},ConvertToNode(rs.underlayer_color())},
                                 {{"underlayer_width"s},{rs.underlayer_width()}},
                                 {{"color_palette"s},ConvertToNode(rs.color_palette())},
                                 });
*/
}

transportCatalog::routerSettings GetRouterSettingsFromDB(const serialize::Router& router) {
    const serialize::RouterSettings& rs = router.router_settings();
    return transportCatalog::routerSettings{rs.bus_wait_time(),rs.bus_velocity()};
}

graph::DirectedWeightedGraph<double> GetGraphFromDB(const serialize::Router& router) {
    const serialize::Graph& g = router.graph();
    std::vector<graph::Edge<double>> edges(g.edge_size());
    std::vector<std::vector<graph::EdgeId>> incidence_lists(g.vertex_size());
    for (size_t i = 0; i < edges.size(); ++i) {
        const serialize::Edge& e = g.edge(i);
        edges[i] = { e.name(), static_cast<size_t>(e.quality()),
                    static_cast<size_t>(e.from()), static_cast<size_t>(e.to()), e.weight() };
    }
    for (size_t i = 0; i < incidence_lists.size(); ++i) {
        const serialize::Vertex& v = g.vertex(i);
        incidence_lists[i].reserve(v.edge_id_size());
        for (const auto& id : v.edge_id()) {
            incidence_lists[i].push_back(id);
        }
    }
    return graph::DirectedWeightedGraph<double>(edges, incidence_lists);
}

std::map<std::string, graph::VertexId> GetStopIdsFromDB(const serialize::Router& router) {
    std::map<std::string, graph::VertexId> result;
    for (const auto& s : router.stop_id()) {
        result[s.name()] = s.id();
    }
    return result;
}

std::tuple<transportCatalog::TransportCatalogue, renderer::MapRenderer, transportCatalog::Router, graph::DirectedWeightedGraph<double>, std::map<string, graph::VertexId> > Deserialize(std::istream& input) {
    serialize::TransportCatalogue database;
    database.ParseFromIstream(&input);
    transportCatalog::TransportCatalogue tcat;
    renderer::MapRenderer renderer(GetRenderSettingsFromDB(database));
    transportCatalog::Router router(GetRouterSettingsFromDB(database.router()));
    AddStopFromDB(tcat, database);
    AddBusFromDB(tcat, database);
    return { std::move(tcat), std::move(renderer), std::move(router),
            GetGraphFromDB(database.router()),
            GetStopIdsFromDB(database.router()) };
}

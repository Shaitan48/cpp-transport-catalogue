#include "transport_router.h"

namespace transportCatalog {



const graph::DirectedWeightedGraph<double>& transportCatalog::Router::BuildGraph(const TransportCatalogue& catalogue) {
    const auto& all_stops = catalogue.GetSortedAllStops();
    const auto& all_buses = catalogue.GetSortedAllBuses();
    graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
    std::map<std::string, graph::VertexId> stop_ids;
    graph::VertexId vertex_id = 0;

    for (const auto& [stop_name, stop_info] : all_stops) {
        stop_ids[stop_info->name] = vertex_id;
        stops_graph.AddEdge({
            stop_info->name,
            0,
            vertex_id,
            ++vertex_id,
            static_cast<double>(bus_wait_time_)
        });
        ++vertex_id;
    }
    stop_ids_ = std::move(stop_ids);

    for_each(
        all_buses.begin(),
        all_buses.end(),
        [&stops_graph, this, &catalogue](const auto& item)
        {
            const auto& bus_ptr = item.second;
            const std::vector<Stop*>& stops = bus_ptr->stops;
            size_t stops_count = stops.size();
            for (size_t i = 0; i < stops_count; ++i) {
                for (size_t j = i + 1; j < stops_count; ++j) {
                    
                    const Stop* stop_from = stops[i];
                    const Stop* stop_to = stops[j];
                    int dist_sum = 0;
                    for (size_t k = i + 1; k <= j; ++k) {
                        dist_sum += stops[k - 1]->GetDistance(stops[k]);
                        //catalogue.counterGetDistance++;
                    }
                    stops_graph.AddEdge({ bus_ptr->number,
                                         j - i,
                                         stop_ids_.at(stop_from->name) + 1,
                                         stop_ids_.at(stop_to->name),
                                         static_cast<double>(dist_sum) / (bus_velocity_ * (100.0 / 6.0)) });
                    if (!bus_ptr->is_round &&  j == stops_count / 2 )
                        break;
                }
            }
        });


    graph_ = std::move(stops_graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);

    return graph_;
}

const std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
    return router_->BuildRoute(stop_ids_.at(std::string(stop_from)),stop_ids_.at(std::string(stop_to)));
}

void Router::SetGraph(graph::DirectedWeightedGraph<double> &&graph, std::map<std::string, graph::VertexId> &&stop_ids)
{
    graph_ = std::move(graph);
    stop_ids_ = std::move(stop_ids);
    router_ = std::make_unique<graph::Router<double>>(graph::Router<double>(graph_));
}

const graph::DirectedWeightedGraph<double>& Router::GetGraph() const {
    return graph_;
}

const std::map<std::string, graph::VertexId> &Router::GetStopIds() const
{
    return stop_ids_;
}

size_t Router::GetGraphVertexCount()
{
    return graph_.GetVertexCount();
}

json::Array Router::GetEdgesItems(const std::vector<graph::EdgeId> &edges) const
{
    json::Array items_array;
    items_array.reserve(edges.size());
    for (auto& edge_id : edges) {
        const graph::Edge<double>& edge = graph_.GetEdge(edge_id);
        if (edge.quality == 0) {
            items_array.emplace_back(json::Node(json::Dict{
                {{"stop_name"},{static_cast<std::string>(edge.name)}},
                {{"time"},{edge.weight}},
                {{"type"},{"Wait"}}
            }));
        }
        else {
            items_array.emplace_back(json::Node(json::Dict{
                {{"bus"},{static_cast<std::string>(edge.name)}},
                {{"span_count"},{static_cast<int>(edge.quality)}},
                {{"time"},{edge.weight}},
                {{"type"},{"Bus"}}
            }));
        }
    }
    return items_array;
}

routerSettings Router::GetSettings() const
{
    return routerSettings{bus_wait_time_,bus_velocity_};
}

void Router::SetSettings(routerSettings settings_node)
{
    bus_wait_time_ = settings_node.bus_wait_time_;
    bus_velocity_ = settings_node.bus_velocity_;
}

std::optional<graph::Router<double>::RouteInfo> Router::GetRouteInfo(const Stop *from, const Stop *to) const
{
    return router_->BuildRoute(stop_ids_.at(from->name), stop_ids_.at(to->name));
}

}

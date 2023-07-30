#pragma once

#include "router.h"
#include "transport_catalogue.h"
#include "json.h"
#include "graph.h"
#include <memory>

namespace transportCatalog {

class Router {
public:
    Router() = default;

    Router(const int bus_wait_time, const double bus_velocity)
        : bus_wait_time_(bus_wait_time)
        , bus_velocity_(bus_velocity) {}

    Router(const Router& settings, const transportCatalog::TransportCatalogue& catalogue) {
        bus_wait_time_ = settings.bus_wait_time_;
        bus_velocity_ = settings.bus_velocity_;
        BuildGraph(catalogue);
    }

    Router(const json::Node& settings_node)
        : bus_wait_time_(settings_node.AsMap().at("bus_wait_time").AsInt())
        , bus_velocity_(settings_node.AsMap().at("bus_velocity").AsDouble()) {

    }

    Router(const json::Node& settings_node, const transportCatalog::TransportCatalogue& catalogue)
        : bus_wait_time_(settings_node.AsMap().at("bus_wait_time").AsInt())
        , bus_velocity_(settings_node.AsMap().at("bus_velocity").AsDouble()) {
        BuildGraph(catalogue);
    }

    Router(const json::Node& settings_node,
           graph::DirectedWeightedGraph<double> graph,
           std::map<std::string, graph::VertexId> stop_ids)
        : bus_wait_time_(settings_node.AsMap().at("bus_wait_time").AsInt())
        , bus_velocity_(settings_node.AsMap().at("bus_velocity").AsDouble())
        ,graph_(graph)
        ,stop_ids_(stop_ids)
    {
        router_ = std::make_unique<graph::Router<double>>(graph::Router<double>(graph_));
    }

    const graph::DirectedWeightedGraph<double>& BuildGraph(const transportCatalog::TransportCatalogue& catalogue);

    const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

    void SetGraph(graph::DirectedWeightedGraph<double>&& graph,
                  std::map<std::string, graph::VertexId>&& stop_ids);
    const graph::DirectedWeightedGraph<double>& GetGraph() const;

    const std::map<std::string, graph::VertexId>& GetStopIds() const;
    size_t GetGraphVertexCount();
    json::Array GetEdgesItems(const std::vector<graph::EdgeId>& edges) const;

    json::Node GetSettings() const;
    void SetSettings(const json::Node& settings_node);

    std::optional<graph::Router<double>::RouteInfo> GetRouteInfo(const Stop* from, const Stop* to) const;

private:
    int bus_wait_time_ = 0;
    double bus_velocity_ = 0.0;

    graph::DirectedWeightedGraph<double> graph_;
    std::map<std::string, graph::VertexId> stop_ids_;
    std::unique_ptr<graph::Router<double>> router_;
};

}

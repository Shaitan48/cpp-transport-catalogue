#pragma once

#include "domain.h"
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

#include "json_builder.h"

#include <iostream>
#include <sstream>
#include <string>
#include <variant>

class JsonReader {
public:
//    explicit JsonReader(std::istream& input)
//        : input_(json::Load(input))
//    {}
    JsonReader(json::Document input)
        : input_(input)
    {}

    const json::Node& GetBaseRequests() const;
    const json::Node& GetStatRequests() const;
//    const json::Node& GetRenderSettings() const;
//    const json::Node& GetRoutingSettings() const;
//    const json::Node& GetSerializationSettings() const;
    renderer::RenderSettings GetRenderSettings() const;
    transportCatalog::routerSettings GetRoutingSettings() const;
    const json::Node& GetSerializationSettings() const;

    json::Node ConvertToNode(const svg::Point& p);
    json::Node ConvertToNode(const svg::Color& c);
    json::Node ConvertToNode(const std::vector<svg::Color>& cv);

    void FillCatalogue(transportCatalog::TransportCatalogue& catalogue);
    renderer::MapRenderer FillRenderSettings(const json::Dict& request_map) const;
    transportCatalog::Router FillRoutingSettings(const json::Node& settings) const;

    void ProcessRequests(const json::Node& stat_requests, RequestHandler& rh, std::ostream &out = std::cout) const;
    const json::Node PrintRoute(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintStop(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintMap(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintRouting(const json::Dict& request_map, RequestHandler& rh) const;

private:
    json::Document input_;
    static inline const json::Node dummy_ = nullptr;

    transportCatalog::Bus FillRoute(const json::Dict& request_map) const;
    transportCatalog::Stop FillStop(const json::Dict& request_map) const;

    svg::Color ParceColor(const json::Node &node) const;
};

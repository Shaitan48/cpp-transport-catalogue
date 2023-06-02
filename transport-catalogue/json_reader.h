#pragma once

#include "domain.h"
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <iostream>
#include <sstream>

class JsonReader {
public:
    JsonReader(std::istream& input)
        : input_(json::Load(input))
    {}

    const json::Node& GetBaseRequests() const;
    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;

    void FillCatalogue(transportCatalog::TransportCatalogue& catalogue);
    renderer::MapRenderer FillRenderSettings(const json::Dict& request_map) const;

    void ProcessRequests(const json::Node& stat_requests, RequestHandler& rh, std::ostream &out = std::cout) const;
    const json::Node PrintRoute(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintStop(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintMap(const json::Dict& request_map, RequestHandler& rh) const;

private:
    json::Document input_;
    json::Node dummy_ = nullptr;

    transportCatalog::Bus FillRoute(const json::Dict& request_map) const;
    transportCatalog::Stop FillStop(const json::Dict& request_map) const;
};

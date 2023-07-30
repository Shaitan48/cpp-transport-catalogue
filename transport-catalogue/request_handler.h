#pragma once

#include "transport_catalogue.h"
<<<<<<< HEAD
#include "transport_router.h"
#include "domain.h"
#include "json.h"
#include "map_renderer.h"
#include "json_builder.h"

#include <utility>
#include <string>
#include <string_view>
=======
#include "map_renderer.h"
>>>>>>> parent of abd3691 (my favorite progaramm)

class RequestHandler {
public:
    RequestHandler(const transportCatalogue::Catalogue& catalogue,
                   const transportCatalogue::Router& router, const renderer::MapRenderer& renderer);

<<<<<<< HEAD
    void JsonStatRequests(const json::Node& json_doc, std::ostream& output);
=======
    explicit RequestHandler(const renderer::MapRenderer& renderer, const transportCatalog::TransportCatalogue& catalogue)
            : catalogue_(catalogue)
            , renderer_(renderer)
        {
        }

    const transportCatalog::RouteInfo GetRoutStat(const std::string_view route_number) const;
    const std::set<std::string> GetBusesByStop(const std::string_view stop_name) const;

    bool ExistingBus(const std::string_view bus_number) const;
    bool ExistingStop(const std::string_view stop_name) const;

    //const json::Node PrintStop(const json::Dict& request_map) const;
    //const json::Node PrintRoute(const json::Dict& request_map) const;

    //void ProcessRequests(const json::Node &request_map) const;
>>>>>>> parent of abd3691 (my favorite progaramm)

    svg::Document RenderMap() const;

private:
    const transportCatalogue::Catalogue& db_;
    const transportCatalogue::Router& router_;
    const renderer::MapRenderer& renderer_;
<<<<<<< HEAD

    json::Node FindStopRequestProcessing(const json::Dict& request_map);
    json::Node FindBusRequestProcessing(const json::Dict& request_map);
    json::Node BuildMapRequestProcessing(const json::Dict& request_map);
    json::Node BuildRouteRequestProcessing(const json::Dict& request_map);
=======
>>>>>>> parent of abd3691 (my favorite progaramm)
};

#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <transport_catalogue.pb.h>

void Serialize(const transportCatalog::TransportCatalogue& tcat,
               const renderer::MapRenderer& renderer,
               const transportCatalog::Router& router,
               std::ostream& output);

serialize::Stop Serialize(const transportCatalog::Stop* stop);
serialize::Bus Serialize(const transportCatalog::Bus* bus);

serialize::RenderSettings GetRenderSettingSerialize(const json::Node& render_settings);
serialize::RouterSettings GetRouterSettingSerialize(const json::Node& router_settings);

serialize::Router Serialize(const transportCatalog::Router& router);

std::tuple<transportCatalog::TransportCatalogue, renderer::MapRenderer, transportCatalog::Router,
           graph::DirectedWeightedGraph<double>, std::map<std::string, graph::VertexId>> Deserialize(std::istream& input);

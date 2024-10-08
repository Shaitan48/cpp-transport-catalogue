#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <variant>

#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <transport_catalogue.pb.h>

void Serialize(const transportCatalog::TransportCatalogue& catalog,
               const renderer::MapRenderer& renderer,
               const transportCatalog::Router& router,
               std::ostream& output);

serialize::Stop Serialize(const transportCatalog::Stop* stop);
serialize::Bus Serialize(const transportCatalog::Bus* bus);

serialize::RenderSettings GetRenderSettingSerialize(renderer::RenderSettings renderer_settings);

//serialize::RouterSettings GetRouterSettingSerialize(const json::Node& router_settings);
serialize::RouterSettings GetRouterSettingSerialize(transportCatalog::routerSettings router_settings);

serialize::Router Serialize(const transportCatalog::Router& router);

std::tuple<transportCatalog::TransportCatalogue, renderer::MapRenderer, transportCatalog::Router,
           graph::DirectedWeightedGraph<double>, std::map<std::string, graph::VertexId>> Deserialize(std::istream& input);

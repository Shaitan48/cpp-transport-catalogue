/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
/*
class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
*/

#pragma once

#include "transport_catalogue.h"
#include "transport_router.h"
#include "domain.h"
#include "json.h"
#include "map_renderer.h"
#include "json_builder.h"

#include <utility>
#include <iostream>
#include <string>
#include <string_view>
#include <iostream>
#include <sstream>

class RequestHandler {
public:
//    explicit RequestHandler(JsonReader& requests, transportCatalog::TransportCatalogue& catalogue)
//        : requests_(requests)
//        , catalogue_(catalogue)
//    {
//        ProcessRequests();
//    }

    explicit RequestHandler(const renderer::MapRenderer& renderer
                            , transportCatalog::TransportCatalogue& catalogue
                            , const transportCatalog::Router& router)
            : catalogue_(catalogue)
            , renderer_(renderer)
            , router_(router)
        {

        }

    const transportCatalog::RouteInfo GetRoutStat(const std::string_view route_number) const;
    const std::set<std::string> GetBusesByStop(const std::string_view stop_name) const;
    const std::optional<graph::Router<double>::RouteInfo> GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const;
    const graph::DirectedWeightedGraph<double>& GetRouterGraph() const;

    bool ExistingBus(const std::string_view bus_number) const;
    bool ExistingStop(const std::string_view stop_name) const;

    //const json::Node PrintStop(const json::Dict& request_map) const;
    //const json::Node PrintRoute(const json::Dict& request_map) const;
    json::Node FindStopRequestProcessing(const json::Dict& request_map);
    json::Node FindBusRequestProcessing(const json::Dict& request_map);
    json::Node BuildMapRequestProcessing(const json::Dict& request_map);
    json::Node BuildRouteRequestProcessing(const json::Dict& request_map);

    void ProcessRequests(const json::Node &request , std::ostream& output);

    //void CalsBusStats(const std::string_view route_number);

    svg::Document RenderMap() const;

private:
    //const JsonReader& requests_;
    transportCatalog::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
    const transportCatalog::Router& router_;
};

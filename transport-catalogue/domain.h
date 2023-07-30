#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace transportCatalog {

struct Stop;

struct RouteInfo {
    size_t stops_count;
    size_t unique_stops_count;
    double route_length;
    double geo_length;
    double  curvature ;
};

struct Bus {
    std::string number;
    std::vector<Stop*> stops;
    bool is_round;
    RouteInfo routeInfo;

    Bus(const std::string& number, std::vector<Stop*> stops, bool is_round)
        : number(number)
        , stops(stops)
        , is_round(is_round){

    }
};

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    std::set<Bus*> buses;
    std::unordered_map<std::string_view, int> stop_distances;

    Stop(const std::string& name, const geo::Coordinates& coordinates)
        : name(name)
        , coordinates(coordinates) {}

    int GetDistance(Stop* to){
        if (stop_distances.count(to->name)) return stop_distances.at(to->name);
        else if (to->stop_distances.count(this->name)) return to->stop_distances.at(this->name);
        else return 0;
    }
};



} // namespace transportCatalog

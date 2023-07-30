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
        auto surseElem = stop_distances.find(to->name);
        if (surseElem != stop_distances.end())
            return surseElem->second;
        else {
            auto destinationElem = to->stop_distances.find(name);
            if (destinationElem != to->stop_distances.end())
                return destinationElem->second;
        }
        return 0;
    }

    std::set<std::string> GetBuses(){
        std::set<std::string> result;
        for(auto bus : buses){
            result.emplace(bus->number);
        }
        return result;
    };
};



} // namespace transportCatalog

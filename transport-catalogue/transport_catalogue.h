#pragma once

#include "domain.h"
#include "geo.h"

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <optional>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <map>

namespace transportCatalog {

struct Stop;

struct Bus;

struct RouteInfo
;


class TransportCatalogue {
public:
    void AddRoute(std::string_view route_number, const std::vector<std::string>& route_stops, bool is_round);
    void AddRoute(std::string_view route_number, const std::vector<Stop*>& route_stops, bool is_round);
    //void AddRoute(Bus* bus);
    void AddStop(std::string_view stop_name, geo::Coordinates& coordinates);
    //void AddStop(Stop* stop);
    void SetDistance(Stop *sourse, const Stop *destination, double dist);
    int GetDistance(const Stop *sourse, const Stop *destination) const;
    const Bus* FindRoute(std::string_view route_number) const;
    const Stop* FindStop(std::string_view stop_name) const;
    Stop* FindStop(std::string_view stop_name);
    size_t UniqueStopsCount(std::string_view route_number) const ;
    //const std::optional<RouteInfo> RouteInformation(const std::string& route_number) const;
    //std::optional<std::set<Bus *> > Stopformation(std::string_view stop_name) const;
    const std::map<std::string_view, const Bus*> GetSortedAllBuses() const;
    const std::map<std::string_view, const Stop*> GetSortedAllStops() const;

    void CalcRoutesInfo();

private:
    std::deque<Bus> all_buses_;
    std::deque<Stop> all_stops_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;

    Stop* FindStopUnconst(std::string_view stop_name) const;

    class StopPairHasher {
    public:
        size_t operator()(std::pair<const Stop* ,const Stop*> p) const {
            // измените эту функцию, чтобы она учитывала все данные номера
            // рекомендуется использовать метод ToString() и существующий
            // класс hash<string>
            return static_cast<size_t>(
                        hash_(p.first->name)
                        +hash_(p.second->name)*37
                        +p.first->coordinates.lat*37*37
                        +p.first->coordinates.lng*37*37*37
                        +p.second->coordinates.lat*37*37*37*37
                        +p.second->coordinates.lng*37*37*37*37*37
                        );
        }

        std::hash<std::string> hash_;
    };

    //std::unordered_map<std::pair<const Stop*,const Stop*>, double,StopPairHasher> distance_map;
};

}//namespace transportCatalog

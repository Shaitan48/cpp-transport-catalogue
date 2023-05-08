#pragma once

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

namespace transportCatalog {

struct Bus {
    std::string number;
    std::optional<std::vector<std::string>> stops;
};

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    std::set<std::string> buses;
    //std::unordered_map<const Stop*, double> distance_map;
    std::unordered_map<std::string, double> distance_map;
};

struct RouteInfo {
    size_t stops_count;
    size_t unique_stops_count;
    int route_length;
    double  curvature ;
};

//struct StopInfo {
//    std::optional<std::set<std::string>> buses;
//};

class TransportCatalogue {
public:
    void AddRoute(const std::string& route_number, const std::vector<std::string>& route_stops);
    void AddStop(const std::string& stop_name, geo::Coordinates& coordinates);
    void AddDistance(Stop *ssourse, Stop *destination, double dist);
    int GetDistance(Stop *sourse, Stop *destination) const;
    const Bus* FindRoute(const std::string& route_number) const;
    void extracted(const std::string &stop_name) const;
    Stop* FindStop(const std::string &stop_name) const;
    const RouteInfo RouteInformation(const std::string& route_number) const;
    size_t UniqueStopsCount(const std::string& route_number) const ;
    const std::optional<std::set<std::string> > Stopformation(const std::string& stop_name) const;

private:
    std::deque<Bus> all_buses_;
    std::deque<Stop> all_stops_;
    std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;

    //std::unordered_map<std::pair<const Stop*,const Stop*>, double> distance_map;
};

}//namespace transportCatalog

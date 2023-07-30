#pragma once

#include "geo.h"
#include "domain.h"

#include <deque>
#include <vector>
#include <string>
#include <unordered_map>
#include <string_view>
#include <map>

namespace transportCatalogue {

using namespace domain;

class Catalogue {
public:
<<<<<<< HEAD
    void AddStop(const std::string& name, const geo::Coordinates& coordinates);
    void AddBus(const std::string& num, const std::vector<Stop*>& stops, bool is_circle);

    Stop* FindStop(const std::string_view stop);
    const Stop* FindStop(const std::string_view stop) const;

    Bus* FindBus(const std::string_view bus_num);
    const Bus* FindBus(const std::string_view bus_num) const;

    std::map<std::string_view, Bus*> GetBusesOnStop(const std::string_view stop_name);
    const std::map<std::string_view, Bus*> GetBusesOnStop(const std::string_view stop_name) const;

    void SetDistance(Stop* from, Stop* to, int dist);
    int GetDistance(const Stop* from, const Stop* to) const;

    const std::map <std::string_view, Bus*>& GetSortedAllBuses() const;
    const std::map <std::string_view, Stop*>& GetSortedAllStops() const;
=======
    void AddRoute(std::string_view route_number, const std::vector<std::string>& route_stops, bool is_round);
    //void AddRoute(Bus* bus);
    void AddStop(std::string_view stop_name, geo::Coordinates& coordinates);
    //void AddStop(Stop* stop);
    void SetDistance(const Stop *ssourse, const Stop *destination, double dist);
    int GetDistance(const Stop *sourse, const Stop *destination) const;
    const Bus* FindRoute(std::string_view route_number) const;
    const Stop* FindStop(std::string_view stop_name) const;
    size_t UniqueStopsCount(std::string_view route_number) const ;
    const std::optional<RouteInfo> RouteInformation(const std::string& route_number) const;
    std::optional<std::set<Bus *> > Stopformation(std::string_view stop_name) const;
    const std::map<std::string_view, const Bus*> GetSortedAllBuses() const;
>>>>>>> parent of abd3691 (my favorite progaramm)

private:
    std::deque<Stop> storage_stops_;
    std::deque<Bus> storage_buses_;
    std::unordered_map < std::string_view, std::map<std::string_view, Bus*>> stop_to_buses_;
    std::map < std::string_view, Stop* > stops_;
    std::map < std::string_view, Bus* > buses_;
};

} // namespace transport

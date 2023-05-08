#include "transport_catalogue.h"

namespace transportCatalog {

void TransportCatalogue::AddRoute(const std::string& route_number, const std::vector<std::string>& route_stops) {
    all_buses_.push_back({ route_number, route_stops});
    busname_to_bus_[all_buses_.back().number] = &all_buses_.back();

    for (const auto& route_stop : route_stops) {
        for (auto& stop_ : all_stops_) {
            if (stop_.name == route_stop) stop_.buses.insert(route_number);
        }
    }
}

void TransportCatalogue::AddStop(const std::string& stop_name, geo::Coordinates &coordinates) {
    all_stops_.push_back({ stop_name, coordinates,{},{}});
    stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();
}

void TransportCatalogue::AddDistance(Stop* sourse,  Stop* destination, double dist)
{

    sourse->distance_map[destination->name] = dist;

}

int TransportCatalogue::GetDistance(Stop *sourse, Stop *destination) const
{
    if (sourse->distance_map.count(destination->name)) return sourse->distance_map.at(destination->name);
    else if (destination->distance_map.count(sourse->name)) return destination->distance_map.at(sourse->name);
    else return 0;
}

const Bus* TransportCatalogue::FindRoute(const std::string& route_number) const {
    auto bus = std::find_if(busname_to_bus_.begin(),busname_to_bus_.end(),[route_number](std::pair<std::string_view, const Bus*> p){return route_number==p.first;});
    if(bus!=busname_to_bus_.end())
        return bus->second;
    return nullptr;
}

Stop* TransportCatalogue::FindStop(const std::string& stop_name) const {
    auto stop = std::find_if(stopname_to_stop_.begin(),stopname_to_stop_.end(),[stop_name](std::pair<std::string_view, Stop*> p){return stop_name==p.first;});
    if(stop!=stopname_to_stop_.end())
        return stop->second;
    return nullptr;
}

const RouteInfo TransportCatalogue::RouteInformation(const std::string& route_number) const {
    RouteInfo route_info{};
    const Bus* bus = FindRoute(route_number);

    if (bus == nullptr) {
        throw std::invalid_argument("bus not found");
    }

    route_info.stops_count = bus->stops->size();

    double route_length = 0.0;
    double geo_length = 0.0;
    for (auto iter = bus->stops.value().begin(); iter + 1 != bus->stops.value().end(); iter++) {
        Stop* sourse = stopname_to_stop_.find(*iter)->second;
        Stop* destination = stopname_to_stop_.find((*(iter + 1)))->second;
        route_length += GetDistance(sourse, destination);
        geo_length += ComputeDistance(stopname_to_stop_.find(*iter)->second->coordinates,
                                      stopname_to_stop_.find(*(iter + 1))->second->coordinates);

    }
    route_info.unique_stops_count = UniqueStopsCount(route_number);
    route_info.route_length = route_length;
    route_info.curvature = route_length / geo_length;

    return route_info;
}

size_t TransportCatalogue::UniqueStopsCount(const std::string& route_number) const {
    std::unordered_set<std::string> unique_stops;
    for (const auto& stop : busname_to_bus_.at(route_number)->stops.value()) {
        unique_stops.insert(stop);
    }
    return unique_stops.size();
}

const std::optional<std::set<std::string>> TransportCatalogue::Stopformation(const std::string &stop_name) const
{
    //    std::optional<std::set<std::string>> stop_info{};

    //    const Stop* stop = FindStop(stop_name);

    //    if (stop == nullptr) {
    //        throw std::invalid_argument("stop not found");
    //    }


    //    for (auto& bus : all_buses_) {
    //        auto stopT = std::find(bus.stops->begin(),bus.stops->end(),stop_name);

    //        if(stopT != bus.stops->end()){
    //            stop_info.buses->emplace(bus.number);
    //        }
    //    }

    return stopname_to_stop_.at(stop_name)->buses;
}

}//namespace transportCatalog

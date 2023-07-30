#include "transport_catalogue.h"

namespace transportCatalog {

void TransportCatalogue::AddRoute(std::string_view route_number, const std::vector<std::string>& route_stops, bool is_round) {
    std::vector<Stop*> stops;

    for(auto& item : route_stops){
        stops.push_back(stopname_to_stop_.at(item));
    }

    all_buses_.push_back({ std::string(route_number), stops, is_round});
    busname_to_bus_[all_buses_.back().number] = &all_buses_.back();

    for(auto item : stops){
           item->buses.insert(&all_buses_.back());
    }
}

void TransportCatalogue::AddRoute(std::string_view route_number, const std::vector<Stop *> &route_stops, bool is_round)
{
    std::vector<Stop*> stops;

    for(auto& item : route_stops){
           stops.push_back(stopname_to_stop_.at(item->name));
    }

    all_buses_.push_back({ std::string(route_number), stops, is_round});
    busname_to_bus_[all_buses_.back().number] = &all_buses_.back();

    for(auto item : stops){
           item->buses.insert(&all_buses_.back());
    }
}

void TransportCatalogue::AddStop(std::string_view stop_name, geo::Coordinates &coordinates) {
    all_stops_.push_back(Stop(stop_name.data(), coordinates));
    stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();
}


void TransportCatalogue::SetDistance(Stop *sourse,  const Stop *destination, double dist)
{
    //distance_map.emplace(std::make_pair(sourse,destination),dist);
    sourse->stop_distances[destination->name] = dist;
}

int TransportCatalogue::GetDistance(const Stop *sourse, const Stop *destination) const
{

//    if (distance_map.count(std::make_pair(sourse, destination)) > 0)
//        return distance_map.at(std::make_pair(sourse, destination));
//    else     if (distance_map.count(std::make_pair(destination, sourse)) > 0)
//        return distance_map.at(std::make_pair(destination, sourse));
//    else
//        return 0;
    this->counterGetDistance++;

    auto surseElem = sourse->stop_distances.find(destination->name);
    if (surseElem != sourse->stop_distances.end())
           return surseElem->second;
    else {
           auto destinationElem = destination->stop_distances.find(sourse->name);
           if (destinationElem != destination->stop_distances.end())
               return destinationElem->second;
    }
           return 0;
}

const Bus* TransportCatalogue::FindRoute(std::string_view route_number) const {
    auto bus = busname_to_bus_.find(route_number);
    if(bus!=busname_to_bus_.end())
        return bus->second;
    return nullptr;
}


const Stop *TransportCatalogue::FindStop(std::string_view stop_name) const {
    auto stop = stopname_to_stop_.find(stop_name);
    if(stop!=stopname_to_stop_.end())
        return stop->second;
    return nullptr;
}

Stop *TransportCatalogue::FindStop(std::string_view stop_name)
{
    auto stop = stopname_to_stop_.find(stop_name);
    if(stop!=stopname_to_stop_.end())
        return stop->second;
    return nullptr;
}

//const std::optional<RouteInfo> TransportCatalogue::RouteInformation(const std::string& route_number) const {
//    RouteInfo route_info;

//    const Bus* bus = FindRoute(route_number);

//    if (bus == nullptr) {
//        throw std::invalid_argument("bus not found1");
//    }

//    route_info.stops_count = bus->stops.size();

//    double route_length = 0.0;
//    double geo_length = 0.0;
//    for (auto iter = bus->stops.begin(); iter + 1 != bus->stops.end(); iter++) {
//        Stop* sourse = *iter;
//        Stop* destination = *(iter + 1);
//        route_length += GetDistance(sourse, destination);
//        geo_length += ComputeDistance(sourse->coordinates,
//                                      destination->coordinates);

//    }
//    route_info.unique_stops_count = UniqueStopsCount(route_number);
//    route_info.route_length = route_length;
//    route_info.curvature = route_length / geo_length;

//    return route_info;
//}

size_t TransportCatalogue::UniqueStopsCount(std::string_view route_number) const {
    std::unordered_set<std::string> unique_stops;
    for (const auto& stop : busname_to_bus_.at(route_number)->stops) {
        unique_stops.insert(stop->name);
    }
    return unique_stops.size();
}

const std::map<std::string_view, const Bus *> TransportCatalogue::GetSortedAllBuses() const
{
    std::map<std::string_view, const Bus*> result;
        for (const auto& bus : busname_to_bus_) {
            result.emplace(bus);
        }
        return result;
}

const std::map<std::string_view, const Stop*> TransportCatalogue::GetSortedAllStops() const
{
    std::map<std::string_view, const Stop*> result;
        for (const auto& stop : stopname_to_stop_) {
            result.emplace(stop);
        }
        return result;
}

//const std::unordered_map<std::string_view, const Bus *> TransportCatalogue::GetAllBuses() const
//{
//        std::unordered_map<std::string_view, const Bus*> result;
//        for (const auto& stop : busname_to_bus_) {
//            result.emplace(stop);
//        }
//        return result;
//}

//const std::unordered_map<std::string_view, const Stop *> TransportCatalogue::GetAllStops() const
//{
//        std::unordered_map<std::string_view, const Stop*> result;
//        for (const auto& stop : stopname_to_stop_) {
//            result.emplace(stop);
//        }
//        return result;
//}

//std::map<std::string_view, Bus *> TransportCatalogue::GetAllBuses() const
//{
//        std::map<std::string_view, Bus*> result;
//        for (const auto& bus : busname_to_bus_) {
//            result.emplace(bus);
//        }
//        return result;
//}

//std::map<std::string_view, Stop *> TransportCatalogue::GetAllStops() const
//{
//        std::map<std::string_view, Stop*> result;
//        for (const auto& stop : stopname_to_stop_) {
//            result.emplace(stop);
//        }
//        return result;
//}


void TransportCatalogue::CalcRoutesInfo()
{
        for (auto &[number, bus]: busname_to_bus_) {
            auto stops = bus->stops;
                if (bus == nullptr) {
                    throw std::invalid_argument("bus not found1");
                }

                bus->routeInfo.stops_count = bus->stops.size();

                double route_length = 0.0;
                double geo_length = 0.0;
                for (auto iter = bus->stops.begin(); iter + 1 != bus->stops.end(); iter++) {
                    Stop* sourse = *iter;
                    Stop* destination = *(iter + 1);
                    route_length += GetDistance(sourse, destination);
                    geo_length += ComputeDistance(sourse->coordinates,
                                                  destination->coordinates);

                }
                bus->routeInfo.unique_stops_count = UniqueStopsCount(number);
                bus->routeInfo.route_length = route_length;
                bus->routeInfo.curvature = route_length / geo_length;
        }
}

Stop *TransportCatalogue::FindStopUnconst(std::string_view stop_name) const
{
    auto stop = std::find_if(stopname_to_stop_.begin(),stopname_to_stop_.end(),[stop_name](std::pair<std::string_view, Stop*> p){return stop_name==p.first;});
    if(stop!=stopname_to_stop_.end())
        return stop->second;
    return nullptr;
}

}//namespace transportCatalog

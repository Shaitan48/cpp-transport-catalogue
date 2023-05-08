#include "stat_reader.h"

#include <iomanip>

namespace transportCatalog {

void ProcessRequests(std::ostream& out, TransportCatalogue& catalogue) {
    size_t requests_count;
    std::cin >> requests_count;
    for (size_t i = 0; i < requests_count; ++i) {
        std::string action, line;
        std::cin >> action;
        std::getline(std::cin, line);
        if(action == "Bus"){
            std::string route_number = line.substr(1, line.npos);
            if (catalogue.FindRoute(route_number)) {
                out << "Bus " << route_number << ": " << catalogue.RouteInformation(route_number).stops_count << " stops on route, "
                    << catalogue.RouteInformation(route_number).unique_stops_count << " unique stops, " << std::setprecision(6)
                    << catalogue.RouteInformation(route_number).route_length << " route length, "
                    << catalogue.RouteInformation(route_number).curvature  << " curvature\n";
            }
            else {
                out << "Bus " << route_number << ": not found\n";
            }
        }
        if (action == "Stop") {
            std::string stop_name = line.substr(1, line.npos);
            if (catalogue.FindStop(stop_name)) {
                out << "Stop " << stop_name << ": ";
                auto buses = catalogue.Stopformation(stop_name);
                if (buses->size()>0) {
                    out << "buses ";
                    for (const auto& bus : *buses) {
                        out << bus << " ";
                    }
                    out << "\n";
                }
                else {
                    out << "no buses\n";
                }
            }
            else {
                out << "Stop " << stop_name << ": not found\n";
            }
        }
    }
}

}//namespace transportCatalog {

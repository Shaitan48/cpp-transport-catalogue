#include "input_reader.h"

#include <string>
#include <vector>
#include <cmath>

namespace transportCatalog {

void FillTransportCatalogue(std::istream& in, TransportCatalogue& catalogue) {
    size_t requests_count;
    in >> requests_count;

    std::vector<std::string> bussBuffer;
    std::unordered_map<std::string,std::string> distBuffer;

    for (size_t i = 0; i < requests_count; ++i) {
        std::string keyword, line;
        in >> keyword;
        std::getline(in, line);
        if (keyword == "Stop") {
            std::string stop_name = line.substr(1, line.find_first_of(':') - line.find_first_of(' ') - 1);
            double lat = std::stod(line.substr(line.find_first_of(':') + 2, 9));
            double lng = std::stod(line.substr(line.find_first_of(',') + 2, 9));
            geo::Coordinates stop_coordinates = { lat, lng };
            catalogue.AddStop(stop_name, stop_coordinates);

            //std::cout << line << "   "<< line.substr(line.find_first_of(':')).size();
            int firsr_zap_pos = line.find_first_of(',')+1;
            int second_zap_pos = line.substr(firsr_zap_pos+1).find_first_of(',') + firsr_zap_pos+1;
            if(firsr_zap_pos != second_zap_pos){
                distBuffer[stop_name] = line.substr(second_zap_pos);
            }

        }
        else if (keyword == "Bus") {
            bussBuffer.push_back(line);
        }
    }

    for(auto& query : distBuffer){
        for(auto& stop : SplitBySep(query.second,',')){
            std::string_view destination = stop.substr(stop.find_first_of('m') + 5);
            double dist = std::stod(std::string(stop.substr(0, stop.find_first_of('m'))));

            catalogue.SetDistance(catalogue.FindStop(query.first),catalogue.FindStop(std::string(destination)),dist);
        }


    }

    for(auto line : bussBuffer){
        std::string route_number = line.substr(1, line.find_first_of(':') - 1);
        line.erase(0, line.find_first_of(':') + 2);
        catalogue.AddRoute(route_number, FillRoute(line));
    }
}


std::vector<std::string> FillRoute(std::string& line) {
    std::vector<std::string> route_stops;
    std::string stop_name;
    char pos = line.find('>') != line.npos ? '>' : '-';
    while (line.find(pos) != line.npos) {
        stop_name = line.substr(0, line.find_first_of(pos) - 1);
        route_stops.push_back(stop_name);
        line.erase(0, line.find_first_of(pos) + 2);
    }
    stop_name = line.substr(0, line.npos - 1);
    route_stops.push_back(stop_name);
    if (pos == '-') {
        route_stops.reserve(route_stops.size() * 2 - 1);
        auto it = route_stops.end()-2;
        while(it!=route_stops.begin()){
            route_stops.push_back(*it);
            it--;
        }
        route_stops.push_back(route_stops.front());
    }

    return route_stops;
}

std::vector<std::string_view> SplitBySep(std::string_view line, char sep){
    std::vector<std::string_view> res;
    int64_t begin = 1;
    const int64_t end = line.npos;
    while (true) {
        int64_t space = line.find(sep, begin);

        if(space != end){
            res.push_back(line.substr(begin+1, space - begin-1));
        }else{
            res.push_back(line.substr(begin+1));
        }

        if (space == end) {
            break;
        }
        else {
            begin = space + 1;
        }
    }

    return res;
}

}//namespace transportCatalog

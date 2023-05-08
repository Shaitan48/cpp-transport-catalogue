#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>

namespace transportCatalog {

void FillTransportCatalogue(std::istream& in, TransportCatalogue& catalogue);

std::vector<std::string> FillRoute(std::string& line);

std::vector<std::string_view> SplitBySep(std::string_view line, char sep);


}//namespace transportCatalog

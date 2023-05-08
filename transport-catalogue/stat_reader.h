#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transportCatalog {

void ProcessRequests(std::ostream &out, std::istream &in, const TransportCatalogue& catalogue);

}//namespace transportCatalog

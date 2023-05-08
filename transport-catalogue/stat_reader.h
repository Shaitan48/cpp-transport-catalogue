#pragma once

#include "transport_catalogue.h"

#include <iostream>

namespace transportCatalog {

void ProcessRequests(std::ostream& out, TransportCatalogue& catalogue);

}//namespace transportCatalog

#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "json.h"
#include "geo.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "serialization.h"

#include <transport_catalogue.pb.h>

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        JsonReader input_json(json::Load(std::cin));
        transportCatalog::TransportCatalogue catalogue;
        input_json.FillCatalogue(catalogue);
        renderer::MapRenderer renderer(input_json.GetRenderSettings());
        transportCatalog::Router router(input_json.GetRoutingSettings(), catalogue);
        std::ofstream fout(input_json.GetSerializationSettings().AsMap().at("file"s).AsString(), std::ios::binary);
        if (fout.is_open()) {
            Serialize(catalogue, renderer, router, fout);
        }
    }
    else if (mode == "process_requests"sv) {
        JsonReader input_json(json::Load(std::cin));
        std::ifstream db_file(input_json.GetSerializationSettings().AsMap().at("file"s).AsString(), std::ios::binary);
        if (db_file) {
            auto [tcat, renderer, router, graph, stop_ids] = Deserialize(db_file);
            router.SetGraph(std::move(graph), std::move(stop_ids));
            RequestHandler handler(renderer, tcat, router);
            handler.ProcessRequests(input_json.GetStatRequests(), std::cout);
        }
    } else {
        PrintUsage();
        return 1;
    }
}

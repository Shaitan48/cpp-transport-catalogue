/*
 * Примерная структура программы:
 *
 * Считать JSON из stdin
 * Построить на его основе JSON базу данных транспортного справочника
 * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
 * с ответами.
 * Вывести в stdout ответы в виде JSON
 */

#include "json_reader.h"
#include "request_handler.h"

#include <iostream>
#include <fstream>

int main() {
    //setlocale(LC_ALL, "Russian");
    //std::ifstream file("input.txt");

    //std::ofstream file1("output.txt");

    transportCatalog::TransportCatalogue catalogue;

    JsonReader requests(std::cin);
    //JsonReader requests(file);
    requests.FillCatalogue(catalogue);

    const auto& stat_requests = requests.GetStatRequests();
    const auto& render_settings = requests.GetRenderSettings().AsMap();
    const auto& renderer = requests.FillRenderSettings(render_settings);

    RequestHandler rh(renderer, catalogue);
    //rh.ProcessRequests(stat_requests);
    //rh.RenderMap().Render(std::cout);

    //requests.ProcessRequests(stat_requests, rh, file1);
    requests.ProcessRequests(stat_requests, rh);

}


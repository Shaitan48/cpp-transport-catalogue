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
    const auto& routing_settings = requests.FillRoutingSettings(requests.GetRoutingSettings());
    const transportCatalog::Router router = { routing_settings, catalogue };

    RequestHandler rh(renderer, catalogue, router);
    //rh.ProcessRequests(stat_requests);
    //rh.RenderMap().Render(std::cout);

    //requests.ProcessRequests(stat_requests, rh, file1);
    requests.ProcessRequests(stat_requests, rh);
    //file1.close();

}
/*
#include "json_builder.h"
#include <iostream>


using namespace std;

int main() {
    json::Print(
        json::Document{
            //json::Builder{}.Value("just a string"s).Build()
            //json::Builder{}.Value("s"s).Key("1"s).Build()
            //json::Builder{}.Value("s"s).StartDict().Build()
            json::Builder{}.StartDict()
                    .Key("1"s).Value("value1"s)
                    //.Key("2"s).Value("value2"s)
                    //.Key("3"s).Value("value3"s)
                .EndDict().Build()
        },
        cout
    );
    cout << endl;

    json::Print(
        json::Document{
                    // Форматирование не имеет формального значения:
                    // это просто цепочка вызовов методов
            json::Builder{}
            .StartDict()
                .Key("key1"s).Value(123)
                .Key("key2"s).Value("value2"s)
                .Key("key3"s).StartArray()
                    .Value(456)
                    .StartDict().EndDict()
                    .StartDict()
                        .Key(""s).Value(nullptr)
                    .EndDict()
                    .Value(""s)
                .EndArray()
            .EndDict()
            .Build()
        },
        cout
    );
}



*/

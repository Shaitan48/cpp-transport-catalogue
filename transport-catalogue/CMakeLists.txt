cmake_minimum_required(VERSION 3.10)

project(TransportCatalogue CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(TRANSPORT_CATALOGUE_SRCS
                      TRANSPORT_CATALOGUE_HDRS
                      graph.proto
                      map_renderer.proto
                      svg.proto
                      transport_catalogue.proto
                      transport_router.proto)

set(TRANSPORT_CATALOGUE_FILES
    domain.cpp                  domain.h
    geo.cpp                     geo.h
                                graph.h
    graph.proto
    json.cpp                    json.h
    json_builder.cpp            json_builder.h
    json_reader.cpp             json_reader.h
    main.cpp
    map_renderer.cpp            map_renderer.h
    map_renderer.proto
                                ranges.h
    request_handler.cpp         request_handler.h
                                router.h
    serialization.cpp           serialization.h
    svg.cpp                     svg.h
    svg.proto
    transport_catalogue.cpp     transport_catalogue.h
    transport_catalogue.proto
    transport_router.cpp        transport_router.h
    transport_router.proto
    )

add_executable(transport_catalogue
               ${TRANSPORT_CATALOGUE_SRCS}
               ${TRANSPORT_CATALOGUE_HDRS}
               ${TRANSPORT_CATALOGUE_FILES})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY_RELEASE}>" Threads::Threads)

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        domain.cpp \
        geo.cpp \
        input_reader.cpp \
        json.cpp \
        json_builder.cpp \
        json_reader.cpp \
        main.cpp \
        map_renderer.cpp \
        request_handler.cpp \
        stat_reader.cpp \
        svg.cpp \
        transport_catalogue.cpp \
        transport_router.cpp

HEADERS += \
    domain.h \
    geo.h \
    graph.h \
    input_reader.h \
    json.h \
    json_builder.h \
    json_reader.h \
    map_renderer.h \
    ranges.h \
    request_handler.h \
    router.h \
    stat_reader.h \
    svg.h \
    transport_catalogue.h \
    transport_router.h

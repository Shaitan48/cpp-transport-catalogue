#pragma once

#include <cmath>

namespace geo {

const int EARTH_RADIUS = 6371000;

const double dr = M_PI  / 180.0;

struct Coordinates {

    double lat;
    double lng;

    bool operator==(const Coordinates& other) const
    {
        return lat == other.lat && lng == other.lng;
    }

    bool operator!=(const Coordinates& other) const
    {
        return !(*this == other);
    }
};

double ComputeDistance(Coordinates sourse, Coordinates destination);

}//namespace geo

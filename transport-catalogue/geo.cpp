#include "geo.h"

namespace geo {

double ComputeDistance(Coordinates sourse, Coordinates destination) {
    using namespace std;
    //if (sourse != destination) {


        return acos(sin(sourse.lat * dr) * sin(destination.lat * dr)
                    +
                    cos(sourse.lat * dr) * cos(destination.lat * dr)
                        *
                    cos(abs(sourse.lng - destination.lng) * dr)
                )
                    *
                EARTH_RADIUS;
//    }
//    return 0;
}

}//namespace geo

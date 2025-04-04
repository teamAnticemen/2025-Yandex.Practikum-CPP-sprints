#pragma once
#include <iostream>
#include <cmath>

namespace geo {

struct Coordinates {
    double lat;
    double lng;
};

inline double ComputeDistance(const Coordinates& from, const Coordinates& to) {
    using namespace std;
    if (from.lat == to.lat && from.lng == to.lng) {
        return 0;
    }
    static const double dr = 3.1415926535 / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * 6371000; // Радиус Земли в метрах
}

} // namespace geo

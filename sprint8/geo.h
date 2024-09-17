#pragma once

#include <cmath>

namespace geo {

    struct Coordinates {
        double lat;
        double lng;
    };

    double ComputeDistance(Coordinates from, Coordinates to);

    inline bool IsZero(double value) {
        return std::abs(value) < 1e-6;
    }

}  // namespace geo

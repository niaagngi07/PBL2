#include "Track.h"
#include <limits>

using namespace std;

Track::Track(Station* src, Station* dest, double dist, double speed)
    : source(src), destination(dest), distance(dist), speedLimit(speed), isOccupied(false) {
    if (speedLimit > 0) {
        weight = distance / speedLimit;
    } else {
        weight = numeric_limits<double>::infinity();
    }
}

double Track::getWeight() const {
    if (isOccupied) {
        return numeric_limits<double>::infinity();
    }
    return weight;
}

void Track::setOccupied(bool status) {
    isOccupied = status;
}

#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>

using namespace std;

class Station; // Forward declaration

class Track {
public:
    Station* source;
    Station* destination;
    double distance;
    double speedLimit;
    double weight; // Time = distance / speedLimit
    bool isOccupied;

    Track(Station* src, Station* dest, double dist, double speed);
    double getWeight() const;
    void setOccupied(bool status);
};

#endif // TRACK_H

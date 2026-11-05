#ifndef STATION_H
#define STATION_H

#include <string>
#include <vector>

using namespace std;

class Track; // Forward declaration

class Station {
public:
    string id;
    string name;
    bool isBlocked;
    vector<Track*> adjTracks;

    Station(string id, string name);
    void addTrack(Track* track);
};

#endif // STATION_H

#include "Station.h"

using namespace std;

Station::Station(string id, string name)
    : id(move(id)), name(move(name)), isBlocked(false) {}

void Station::addTrack(Track* track) {
    adjTracks.push_back(track);
}

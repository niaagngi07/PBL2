#include "Train.h"
#include "Station.h"

using namespace std;

Train::Train(string id, TrainType type)
    : id(move(id)), type(type), currentStation(nullptr), destinationStation(nullptr) {
    if (type == TrainType::MRT) {
        priority = 1;
    } else if (type == TrainType::LRT) {
        priority = 2;
    } else {
        priority = 3;
    }
}

void Train::setRoute(const queue<Station*>& newRoute, Station* dest) {
    route = newRoute;
    destinationStation = dest;
}

void Train::moveNext() {
    if (!route.empty()) {
        currentStation = route.front();
        route.pop();
    }
}

Station* Train::getNextStation() const {
    if (!route.empty()) {
        return route.front();
    }
    return nullptr;
}

bool Train::hasReachedDestination() const {
    return route.empty();
}

bool Train::containsStationInRoute(const string& stId) const {
    queue<Station*> copyQ = route;
    while (!copyQ.empty()) {
        if (copyQ.front() && copyQ.front()->id == stId) return true;
        copyQ.pop();
    }
    return false;
}

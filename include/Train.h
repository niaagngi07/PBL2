#ifndef TRAIN_H
#define TRAIN_H

#include <string>
#include <vector>
#include <queue>

using namespace std;

class Station;

enum class TrainType {
    MRT,
    LRT,
    LRT_TOURIST
};

class Train {
public:
    string id;
    TrainType type;
    int priority; // MRT: 1, LRT: 2, LRT_DL: 3
    queue<Station*> route;
    Station* currentStation;
    Station* destinationStation;

    Train(string id, TrainType type);
    
    void setRoute(const queue<Station*>& newRoute, Station* dest = nullptr);
    void moveNext();
    Station* getNextStation() const;
    bool hasReachedDestination() const;
    bool containsStationInRoute(const string& stId) const;
};

#endif // TRAIN_H

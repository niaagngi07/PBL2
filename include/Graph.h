#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include "Train.h"

using namespace std;

class Station;
class Track;

class Graph {
private:
    unordered_map<string, Station*> stations;
    vector<Track*> tracks;

public:
    Graph();
    ~Graph();

    void addStation(const string& id, const string& name);
    void addTrack(const string& srcId, const string& destId, double distance, double speedLimit);
    
    // Thuật toán Dijkstra tiêu chuẩn
    vector<Station*> dynamicDijkstra(const string& startId, const string& endId);

    // Thuật toán Dijkstra phân cấp ưu tiên theo loại tàu (MRT > LRT > LRT DL)
    vector<Station*> priorityDijkstra(const string& startId, const string& endId, TrainType trainType);

    // Quản lý trạng thái tắc nghẽn của đoạn đường
    void setTrackOccupancy(const string& srcId, const string& destId, bool status);

    // Getters cho lớp đồ họa
    Station*              getStation(const string& id);
    const vector<Track*>& getTracks() const;
};

#endif // GRAPH_H

#include "Graph.h"
#include "Station.h"
#include "Track.h"
#include <limits>
#include <algorithm>

using namespace std;

Graph::Graph() {}

Graph::~Graph() {
    for (auto& pair : stations) {
        delete pair.second;
    }
    for (auto* track : tracks) {
        delete track;
    }
}

void Graph::addStation(const string& id, const string& name) {
    if (stations.find(id) == stations.end()) {
        stations[id] = new Station(id, name);
    }
}

void Graph::addTrack(const string& srcId, const string& destId, double distance, double speedLimit) {
    if (stations.find(srcId) != stations.end() && stations.find(destId) != stations.end()) {
        Station* src = stations[srcId];
        Station* dest = stations[destId];
        
        // Tuyến ray có hướng từ src -> dest
        Track* track1 = new Track(src, dest, distance, speedLimit);
        src->addTrack(track1);
        tracks.push_back(track1);
        
        // Tuyến ray ngược chiều từ dest -> src (Đường đôi Bi-track)
        Track* track2 = new Track(dest, src, distance, speedLimit);
        dest->addTrack(track2);
        tracks.push_back(track2);
    }
}

void Graph::setTrackOccupancy(const string& srcId, const string& destId, bool status) {
    if (stations.find(srcId) != stations.end()) {
        Station* src = stations[srcId];
        for (Track* track : src->adjTracks) {
            if (track->destination->id == destId) {
                track->setOccupied(status);
                break;
            }
        }
    }
}

Station* Graph::getStation(const string& id) {
    auto it = stations.find(id);
    if (it != stations.end()) return it->second;
    return nullptr;
}

const vector<Track*>& Graph::getTracks() const {
    return tracks;
}

vector<Station*> Graph::priorityDijkstra(const string& startId, const string& endId, TrainType trainType) {
    if (stations.find(startId) == stations.end() || stations.find(endId) == stations.end()) {
        return {};
    }

    Station* startStation = stations[startId];
    Station* targetStation = stations[endId];

    // Nếu ga đầu hoặc ga cuối đang bị tắc nghẽn/sự cố thì không thể lập tuyến
    if (startStation->isBlocked || targetStation->isBlocked) {
        return {};
    }

    unordered_map<Station*, double> minWeight;
    unordered_map<Station*, Station*> previous;
    for (auto& pair : stations) {
        minWeight[pair.second] = numeric_limits<double>::infinity();
        previous[pair.second] = nullptr;
    }

    minWeight[startStation] = 0.0;

    auto cmp = [&minWeight](Station* left, Station* right) {
        return minWeight[left] > minWeight[right];
    };
    priority_queue<Station*, vector<Station*>, decltype(cmp)> pq(cmp);
    pq.push(startStation);

    // Xác định trần tốc độ theo phân cấp loại tàu
    double trainMaxSpeed = 80.0;
    if (trainType == TrainType::LRT) trainMaxSpeed = 60.0;
    else if (trainType == TrainType::LRT_TOURIST) trainMaxSpeed = 40.0;

    while (!pq.empty()) {
        Station* current = pq.top();
        pq.pop();

        if (current->id == endId) break;

        for (Track* track : current->adjTracks) {
            Station* neighbor = track->destination;

            // Đoạn ray đang bị tắc nghẽn hoặc ga kề bị sự cố -> Bỏ qua
            if (track->isOccupied || neighbor->isBlocked) continue;

            // 1. Thời gian lưu thông thực tế: t = s / min(V_tau, V_track)
            double effectiveSpeed = min(trainMaxSpeed, track->speedLimit);
            double baseWeight = track->distance / effectiveSpeed;

            // 2. Trọng số điều phối theo phân cấp ưu tiên (Priority Tuning):
            if (trainType == TrainType::MRT) {
                if (track->speedLimit <= 45.0) {
                    baseWeight *= 2.5; // Phạt nặng khi đi vào tuyến du lịch
                } else if (track->speedLimit >= 75.0) {
                    baseWeight *= 0.85; // Khuyến khích đường ray cao tốc
                }
            } else if (trainType == TrainType::LRT_TOURIST) {
                if (track->speedLimit <= 45.0) {
                    baseWeight *= 0.8; // Khuyến khích tuyến du lịch
                } else if (track->speedLimit >= 75.0) {
                    baseWeight *= 1.5; // Nhường đường cho tàu nhanh
                }
            }

            if (minWeight[current] + baseWeight < minWeight[neighbor]) {
                minWeight[neighbor] = minWeight[current] + baseWeight;
                previous[neighbor] = current;
                pq.push(neighbor);
            }
        }
    }

    vector<Station*> path;
    Station* current = stations[endId];
    if (previous[current] != nullptr || current == startStation) {
        while (current != nullptr) {
            path.push_back(current);
            current = previous[current];
        }
        reverse(path.begin(), path.end());
    }

    return path;
}

vector<Station*> Graph::dynamicDijkstra(const string& startId, const string& endId) {
    return priorityDijkstra(startId, endId, TrainType::MRT);
}

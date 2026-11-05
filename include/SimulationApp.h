#ifndef SIMULATION_APP_H
#define SIMULATION_APP_H

#include <vector>
#include <unordered_map>
#include <string>
#include "raylib.h"
#include "Graph.h"
#include "Train.h"
#include "StationView.h"
#include "TrackView.h"
#include "TrainView.h"
#include "MapDatabase.h"

using namespace std;

class UIPanel;

struct QueuedDispatch {
    string           trainId;
    TrainType        type;
    int              priority;
    Station*         startStation;
    Station*         endStation;
    vector<Station*> path;
};

struct ArrivedTrainInfo {
    string    trainId;
    TrainType type;
    string    originId;
    string    originName;
    string    destId;
    string    destName;
    float     arrivedTime;
};

class SimulationApp {
    friend class UIPanel;

private:
    int       screenW, screenH;
    bool      isFullscreen, mapTextureLoaded, showSatelliteMap;
    Texture2D mapTexture;
    float     mapScale, mapOffsetX, mapOffsetY;

    Graph                               graph;
    unordered_map<string, StationView*> stationViews;
    vector<TrackView*>                  trackViews;
    vector<TrainView*>                  trainViews;
    vector<Train*>                      trains;
    unordered_map<string, Vector2>      stationPositions;

    // Cấu hình chọn tàu ở bảng bên trái
    TrainType activeTrainType;
    int       selectedTrainNum;
    int       originIndex;
    int       destIndex;

    // Cuộn danh sách trên giao diện
    int       scrollOrigin;
    int       scrollDest;
    int       scrollBlocked;
    int       scrollSaved;
    int       scrollArrived;

    // Hạm đội đã lưu & Nhật ký tàu đến nơi
    vector<QueuedDispatch>   pendingFleet;
    vector<ArrivedTrainInfo> arrivedFleet;
    vector<Station*>         currentPath;
    string                   statusMsg;
    int                      trainCounter;

    void buildMap();
    void buildViews();
    void recalculateViewport();
    void toggleFullscreenMode();
    void processEvents();
    void update(float dt);
    void render();

    bool spawnTrainOnPath(const vector<Station*>& path, TrainType t, const string& customId);

public:
    SimulationApp(int width = 1920, int height = 1080);
    ~SimulationApp();
    void run();

    void selectTrainType(TrainType t);
    void selectTrainNum(int num);
    void setOriginIndex(int idx);
    void setDestIndex(int idx);
    void toggleBlockedStation(const string& stId);
    void saveCurrentTrainToFleet();
    void removeSavedTrain(size_t idx);
    void clearSavedFleet();
    void launchAllQueuedTrains();
    void clearArrivedFleet();
    void runDijkstra();

    // Tự động tái định tuyến khi phát hiện ga tắc nghẽn
    bool reRouteTrain(TrainView* tv);

    bool isSegmentOccupied(Station* from, Station* to, TrainView* excludeTrain) const;
    TrainView* getSegmentOccupier(Station* from, Station* to, TrainView* excludeTrain) const;

    int getPanelWidth() const;
    Rectangle getLeftPanelRec() const;
    Rectangle getRightPanelRec() const;
    StationView* getHoveredStation() const;
};

#endif // SIMULATION_APP_H

#include "SimulationApp.h"
#include "RenderUtils.h"
#include "MapDatabase.h"
#include "UIPanel.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <queue>

using namespace std;

SimulationApp::SimulationApp(int width, int height)
    : screenW(width), screenH(height), isFullscreen(false),
      mapTexture({0}), mapTextureLoaded(false), showSatelliteMap(true),
      mapScale(1.0f), mapOffsetX(0.0f), mapOffsetY(0.0f),
      activeTrainType(TrainType::MRT), selectedTrainNum(1),
      originIndex(0), destIndex(5),
      scrollOrigin(0), scrollDest(0), scrollBlocked(0), scrollSaved(0), scrollArrived(0),
      trainCounter(0)
{
    srand((unsigned)time(nullptr));
    buildMap();
    buildViews();
    runDijkstra();
    statusMsg = "HE THONG SAN SANG: Chon loai tau, ga dau & ga cuoi o bang ben trai, bam [LUU CHUYEN TAU]";
}

SimulationApp::~SimulationApp() {
    for (auto& [id, sv] : stationViews) delete sv;
    for (auto* tv : trackViews)  delete tv;
    for (auto* tv : trainViews)  delete tv;
    for (auto* t  : trains)      delete t;
    if (mapTextureLoaded) UnloadTexture(mapTexture);
    UnloadRenderFonts();
}

void SimulationApp::buildMap() {
    for (const auto& s : MapDatabase::getStations()) graph.addStation(s.id, s.name);
    for (const auto& t : MapDatabase::getTracks())   graph.addTrack(t.srcId, t.destId, t.distance, t.speedLimit);
}

void SimulationApp::buildViews() {
    for (const auto& s : MapDatabase::getStations()) {
        Station* st = graph.getStation(s.id);
        if (!st) continue;
        stationViews[s.id] = new StationView(st, s.imgX, s.imgY);
        stationPositions[s.id] = {s.imgX, s.imgY};
    }
    for (Track* tr : graph.getTracks()) {
        auto itS = stationViews.find(tr->source->id), itD = stationViews.find(tr->destination->id);
        if (itS != stationViews.end() && itD != stationViews.end())
            trackViews.push_back(new TrackView(tr, itS->second, itD->second));
    }
}

void SimulationApp::recalculateViewport() {
    screenW = GetScreenWidth();
    screenH = GetScreenHeight();
    gUiScale = max(1.0f, (float)screenH / 1080.0f);

    float availH = (float)screenH - ScaleF(96.0f);
    mapScale = availH / 1249.0f;
    mapOffsetX = (screenW - 1038.0f * mapScale) * 0.5f;
    mapOffsetY = ScaleF(50.0f);

    for (const auto& s : MapDatabase::getStations()) {
        Vector2 pos = {mapOffsetX + s.imgX * mapScale, mapOffsetY + s.imgY * mapScale};
        stationPositions[s.id] = pos;
        if (stationViews.count(s.id)) {
            stationViews[s.id]->pos = pos;
            stationViews[s.id]->radius = ScaleF(13.0f);
        }
    }
}

int SimulationApp::getPanelWidth() const {
    float maxSideW = (screenW - 1038.0f * mapScale) * 0.5f - ScaleF(16.0f);
    return clamp(ScaleI(460.0f), ScaleI(380.0f), max(ScaleI(380.0f), (int)maxSideW));
}

Rectangle SimulationApp::getLeftPanelRec() const {
    int pw = getPanelWidth();
    int px = ScaleI(14.0f), py = ScaleI(52.0f), ph = screenH - py - ScaleI(48.0f);
    return {(float)px, (float)py, (float)pw, (float)ph};
}

Rectangle SimulationApp::getRightPanelRec() const {
    int pw = getPanelWidth();
    int px = screenW - pw - ScaleI(14.0f), py = ScaleI(52.0f), ph = screenH - py - ScaleI(48.0f);
    return {(float)px, (float)py, (float)pw, (float)ph};
}

void SimulationApp::toggleFullscreenMode() {
    isFullscreen = !isFullscreen;
    ToggleFullscreen();
    recalculateViewport();
}

void SimulationApp::selectTrainType(TrainType t) {
    activeTrainType = t;
    selectedTrainNum = 1;
    runDijkstra();
    string tName = (t == TrainType::MRT) ? "MRT (Cao toc 80 km/h, UT1)" : ((t == TrainType::LRT) ? "LRT (Do thi 60 km/h, UT2)" : "LRT Du Lich (40 km/h, UT3)");
    statusMsg = "Da chon loai tau: " + tName + ".";
}

void SimulationApp::selectTrainNum(int num) {
    selectedTrainNum = num;
    string prefix = (activeTrainType == TrainType::MRT) ? "MRT-" : ((activeTrainType == TrainType::LRT) ? "LRT-" : "DL-");
    statusMsg = "Da chon so hieu tau: " + prefix + to_string(num) + ".";
}

void SimulationApp::setOriginIndex(int idx) {
    const auto& stations = MapDatabase::getStations();
    if (idx < 0 || idx >= (int)stations.size()) return;
    originIndex = idx;
    for (auto& [id, sv] : stationViews) sv->isSelected = false;
    if (stationViews.count(stations[originIndex].id)) stationViews[stations[originIndex].id]->isSelected = true;
    if (destIndex >= 0 && destIndex < (int)stations.size() && stationViews.count(stations[destIndex].id))
        stationViews[stations[destIndex].id]->isSelected = true;
    runDijkstra();
    statusMsg = "Ga xuat phat: " + stations[originIndex].id + " (" + stations[originIndex].name + ").";
}

void SimulationApp::setDestIndex(int idx) {
    const auto& stations = MapDatabase::getStations();
    if (idx < 0 || idx >= (int)stations.size()) return;
    destIndex = idx;
    for (auto& [id, sv] : stationViews) sv->isSelected = false;
    if (originIndex >= 0 && originIndex < (int)stations.size() && stationViews.count(stations[originIndex].id))
        stationViews[stations[originIndex].id]->isSelected = true;
    if (stationViews.count(stations[destIndex].id)) stationViews[stations[destIndex].id]->isSelected = true;
    runDijkstra();
    statusMsg = "Ga dich den: " + stations[destIndex].id + " (" + stations[destIndex].name + ").";
}

bool SimulationApp::reRouteTrain(TrainView* tv) {
    if (!tv || tv->isFinished || !tv->model || !tv->model->destinationStation) return false;
    Station* dest = tv->model->destinationStation;

    // Điểm bắt đầu tái định tuyến:
    // Nếu tàu đang chạy thì ga tái định tuyến là toStation (nếu toStation không bị tắc)
    // Nếu toStation bị tắc, hoặc tàu đang dừng thì ga tái định tuyến là fromStation
    Station* startReroute = tv->fromStation;
    if (tv->moving && tv->toStation && !tv->toStation->isBlocked) {
        startReroute = tv->toStation;
    }

    if (!startReroute || startReroute->isBlocked) return false;

    vector<Station*> altPath = graph.priorityDijkstra(startReroute->id, dest->id, tv->model->type);
    if (altPath.size() < 2) {
        // Không có đường vòng nào khác tới đích
        return false;
    }

    // Cập nhật lại hàng đợi lộ trình của tàu
    queue<Station*> newQ;
    for (size_t i = 1; i < altPath.size(); ++i) newQ.push(altPath[i]);
    tv->model->setRoute(newQ, dest);

    if (!tv->moving) {
        // Nếu tàu đang đỗ, thiết lập chặng mới sang ga đầu tiên của tuyến vòng
        tv->fromStation = startReroute;
        tv->toStation = altPath[1];
        tv->setSegment(tv->fromStation, tv->toStation, stationPositions);
        tv->isWaitingBlock = false;
        tv->moving = true;
        tv->dwellTimer = 0.25f;
    }
    return true;
}

void SimulationApp::toggleBlockedStation(const string& stId) {
    Station* st = graph.getStation(stId);
    if (!st) return;
    st->isBlocked = !st->isBlocked;

    // 1. Tự động tái định tuyến cho toàn bộ tàu đang chạy
    int rerouted = 0;
    for (TrainView* tv : trainViews) {
        if (tv->isFinished) continue;
        if (tv->model->containsStationInRoute(stId) || (tv->toStation && tv->toStation->id == stId)) {
            if (reRouteTrain(tv)) rerouted++;
        }
    }

    // 2. Cập nhật lại tuyến cho toàn bộ tàu đã lưu trong hàng đợi
    for (auto& qd : pendingFleet) {
        vector<Station*> newPath = graph.priorityDijkstra(qd.startStation->id, qd.endStation->id, qd.type);
        if (!newPath.empty()) {
            qd.path = newPath;
        }
    }

    // 3. Cập nhật lại tuyến đang chọn ở bảng bên trái
    runDijkstra();

    string stStr = st->isBlocked ? " [TAC NGHEN / SU CO]" : " [HOAT DONG BINH THUONG]";
    statusMsg = "Ga " + stId + stStr + (rerouted > 0 ? " -> Da tai dinh tuyen cho " + to_string(rerouted) + " doan tau tranh ga nay!" : "");
}

void SimulationApp::runDijkstra() {
    const auto& stations = MapDatabase::getStations();
    if (originIndex < 0 || originIndex >= (int)stations.size() || destIndex < 0 || destIndex >= (int)stations.size()) {
        currentPath.clear();
        return;
    }
    if (originIndex == destIndex) {
        currentPath.clear();
        return;
    }
    string startId = stations[originIndex].id;
    string endId = stations[destIndex].id;
    currentPath = graph.priorityDijkstra(startId, endId, activeTrainType);
}

void SimulationApp::saveCurrentTrainToFleet() {
    const auto& stations = MapDatabase::getStations();
    if (currentPath.size() < 2 || originIndex < 0 || originIndex >= (int)stations.size() || destIndex < 0 || destIndex >= (int)stations.size()) {
        statusMsg = "Khong the luu: Khong co tuyen duong hop le hoac ga bi tac nghen!";
        return;
    }

    string prefix = (activeTrainType == TrainType::MRT) ? "MRT-" : ((activeTrainType == TrainType::LRT) ? "LRT-" : "DL-");
    string tId = prefix + to_string(selectedTrainNum);

    // Kiểm tra xem ID này đã có trong hàng đợi chưa, nếu có thì tự động tăng đuôi
    int suffix = 1;
    string finalId = tId;
    bool exists = true;
    while (exists) {
        exists = false;
        for (const auto& qd : pendingFleet) {
            if (qd.trainId == finalId) { exists = true; break; }
        }
        for (const auto* tv : trainViews) {
            if (tv->model->id == finalId && !tv->isFinished) { exists = true; break; }
        }
        if (exists) {
            finalId = tId + "#" + to_string(++suffix);
        }
    }

    QueuedDispatch qd;
    qd.trainId = finalId;
    qd.type = activeTrainType;
    qd.priority = (activeTrainType == TrainType::MRT) ? 1 : ((activeTrainType == TrainType::LRT) ? 2 : 3);
    qd.startStation = graph.getStation(stations[originIndex].id);
    qd.endStation = graph.getStation(stations[destIndex].id);
    qd.path = currentPath;

    pendingFleet.push_back(qd);
    statusMsg = ">> Da luu tau " + qd.trainId + " vao danh sach cho (" + to_string(pendingFleet.size()) + " tau). Nhan [SPACE] de xuat phat!";
}

void SimulationApp::removeSavedTrain(size_t idx) {
    if (idx < pendingFleet.size()) {
        string tid = pendingFleet[idx].trainId;
        pendingFleet.erase(pendingFleet.begin() + idx);
        statusMsg = "Da xoa tau " + tid + " khoi danh sach cho.";
    }
}

void SimulationApp::clearSavedFleet() {
    pendingFleet.clear();
    statusMsg = "Da xoa toan bo danh sach tau cho xuat phat.";
}

void SimulationApp::clearArrivedFleet() {
    arrivedFleet.clear();
    statusMsg = "Da xoa nhat ky thong bao tau ve dich.";
}

bool SimulationApp::isSegmentOccupied(Station* from, Station* to, TrainView* excludeTrain) const {
    if (!from || !to) return false;
    for (TrainView* tv : trainViews) {
        if (tv == excludeTrain || tv->isFinished || tv->isWaitingBlock) continue;
        if (tv->fromStation == from && tv->toStation == to) return true;
    }
    return false;
}

TrainView* SimulationApp::getSegmentOccupier(Station* from, Station* to, TrainView* excludeTrain) const {
    if (!from || !to) return nullptr;
    for (TrainView* tv : trainViews) {
        if (tv == excludeTrain || tv->isFinished || tv->isWaitingBlock) continue;
        if (tv->fromStation == from && tv->toStation == to) return tv;
    }
    return nullptr;
}

bool SimulationApp::spawnTrainOnPath(const vector<Station*>& path, TrainType t, const string& customId) {
    if (path.size() < 2) return false;

    string id = customId.empty() ? (((t == TrainType::MRT) ? "MRT-" : ((t == TrainType::LRT) ? "LRT-" : "DL-")) + to_string(++trainCounter)) : customId;

    Train* train = new Train(id, t);
    queue<Station*> q;
    for (size_t i = 1; i < path.size(); ++i) q.push(path[i]);
    train->setRoute(q, path.back()); // Ghi nhận ga đích cuối cùng
    train->currentStation = path[0];
    trains.push_back(train);

    Station *stA = path[0], *stB = path[1];
    TrainView* tv = new TrainView(train, stA, stB, stationPositions);

    // Kiểm tra an toàn trước khi xuất phát đoạn ray đầu tiên
    TrainView* occupier = getSegmentOccupier(stA, stB, tv);
    if (occupier || stB->isBlocked) {
        tv->isWaitingBlock = true;
        tv->waitingForTrainId = occupier ? occupier->model->id : ("GA NGHEN: " + stB->id);
        tv->moving = false;

        int order = 0;
        for (TrainView* o : trainViews) {
            if (o->isWaitingBlock && o->fromStation == stA && o->toStation == stB) order++;
        }
        tv->waitingOrder = order;
    }

    trainViews.push_back(tv);
    return true;
}

void SimulationApp::launchAllQueuedTrains() {
    if (pendingFleet.empty()) {
        if (currentPath.size() >= 2) {
            saveCurrentTrainToFleet();
        } else {
            statusMsg = "Danh sach trong! Vui long chon ga va bam [LUU CHUYEN TAU].";
            return;
        }
    }

    // Sắp xếp hạm đội theo thứ tự ưu tiên: MRT (UT1) > LRT (UT2) > LRT DL (UT3)
    sort(pendingFleet.begin(), pendingFleet.end(), [](const QueuedDispatch& a, const QueuedDispatch& b) {
        return a.priority < b.priority;
    });

    int launched = 0, waiting = 0;
    for (const auto& item : pendingFleet) {
        if (spawnTrainOnPath(item.path, item.type, item.trainId)) {
            if (trainViews.back()->isWaitingBlock) waiting++;
            else launched++;
        }
    }
    pendingFleet.clear();

    statusMsg = (waiting > 0) ? (">> DIEU PHOI: " + to_string(launched) + " tau chay, " + to_string(waiting) + " tau cho thoat ray theo cap uu tien!")
                              : (">> XUAT PHAT DONG LOAT: " + to_string(launched) + " tau da khoi hanh tren mang luoi!");
}

void SimulationApp::run() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    int monitor = GetCurrentMonitor();
    screenW = GetMonitorWidth(monitor);
    screenH = GetMonitorHeight(monitor);

    InitWindow(screenW, screenH, "He Thong Dieu Phoi Tau Hoa Dan Dung - Da Nang");
    SetTargetFPS(60);
    toggleFullscreenMode();
    InitRenderFonts();

    const char* paths[] = {
        "DaNang_Map_Clean.png",
        "Screenshot 2026-09-24 170712.png",
        "../DaNang_Map_Clean.png",
        "e:/PBL-DUT/Test/DaNang_Map_Clean.png"
    };
    for (const char* p : paths) {
        if (FileExists(p)) {
            mapTexture = LoadTexture(p);
            if (mapTexture.id != 0) {
                SetTextureFilter(mapTexture, TEXTURE_FILTER_BILINEAR);
                mapTextureLoaded = true;
                break;
            }
        }
    }
    recalculateViewport();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (IsWindowResized() || screenW != GetScreenWidth() || screenH != GetScreenHeight()) recalculateViewport();

        processEvents();
        update(dt);

        BeginDrawing();
        render();
        EndDrawing();
    }
    CloseWindow();
}

void SimulationApp::processEvents() {
    Vector2 mouse = GetMousePosition();
    for (auto& [id, sv] : stationViews) sv->update(mouse);

    // Xử lý click và cuộn chuột trên các panel UI trước
    if (UIPanel::handleMouseInteraction(this)) {
        return;
    }

    // Các phím tắt nhanh
    if (IsKeyPressed(KEY_ONE))   selectTrainType(TrainType::MRT);
    if (IsKeyPressed(KEY_TWO))   selectTrainType(TrainType::LRT);
    if (IsKeyPressed(KEY_THREE)) selectTrainType(TrainType::LRT_TOURIST);

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_L)) saveCurrentTrainToFleet();
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_X)) launchAllQueuedTrains();
    if (IsKeyPressed(KEY_C)) clearSavedFleet();
    if (IsKeyPressed(KEY_R)) {
        for (auto* tv : trainViews) delete tv; trainViews.clear();
        for (auto* t  : trains)     delete t;  trains.clear();
        pendingFleet.clear(); currentPath.clear();
        for (auto& [id, sv] : stationViews) {
            sv->isSelected = false;
            sv->model->isBlocked = false;
        }
        runDijkstra();
        statusMsg = "He thong da duoc Reset toan bo trang thai ve ban dau.";
    }
    if (IsKeyPressed(KEY_M)) showSatelliteMap = !showSatelliteMap;
    if (IsKeyPressed(KEY_F11) || IsKeyPressed(KEY_F)) toggleFullscreenMode();

    // Click trực tiếp lên trạm trên bản đồ
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        StationView* clicked = getHoveredStation();
        if (clicked) {
            const auto& stations = MapDatabase::getStations();
            for (size_t i = 0; i < stations.size(); ++i) {
                if (stations[i].id == clicked->model->id) {
                    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                        setDestIndex((int)i);
                    } else {
                        setOriginIndex((int)i);
                    }
                    break;
                }
            }
        }
    }
}

StationView* SimulationApp::getHoveredStation() const {
    Vector2 m = GetMousePosition();
    for (auto& [id, sv] : stationViews) if (sv->contains(m)) return sv;
    return nullptr;
}

void SimulationApp::update(float dt) {
    // 1. Cập nhật vị trí và trạng thái của các đoàn tàu
    for (auto it = trainViews.begin(); it != trainViews.end(); ) {
        TrainView* tv = *it;
        tv->update(dt);

        // Ghi nhận bảng thông báo khi tàu về đến ga cuối an toàn
        if (tv->isFinished && !tv->hasAnnouncedArrival) {
            tv->hasAnnouncedArrival = true;
            ArrivedTrainInfo info;
            info.trainId = tv->model->id;
            info.type = tv->model->type;
            info.destId = tv->toStation ? tv->toStation->id : (tv->fromStation ? tv->fromStation->id : "DICH");
            info.destName = tv->toStation ? tv->toStation->name : (tv->fromStation ? tv->fromStation->name : "Ga Den");
            info.arrivedTime = (float)GetTime();
            arrivedFleet.insert(arrivedFleet.begin(), info); // Đưa lên đầu danh sách
            statusMsg = ">> TAU " + info.trainId + " DA VE GA " + info.destId + " AN TOAN!";
        }

        if (tv->shouldRemove()) {
            auto tIt = find(trains.begin(), trains.end(), tv->model);
            if (tIt != trains.end()) { delete *tIt; trains.erase(tIt); }
            delete tv;
            it = trainViews.erase(it);
        } else {
            ++it;
        }
    }

    // 2. LOGIC ĐIỀU PHỐI GIỮA HÀNH TRÌNH (Mid-journey Block Signaling & Tự động tránh ga nghẽn):
    for (TrainView* tv : trainViews) {
        if (tv->isFinished || tv->moving || tv->dwellTimer > 0.0f || tv->isWaitingBlock) continue;

        if (tv->fromStation && tv->toStation) {
            // KIỂM TRA 1: GA TIẾP THEO CÓ BỊ TẮC NGHẼN / SỰ CỐ KHÔNG?
            if (tv->toStation->isBlocked) {
                // Thử tái định tuyến tìm đường vòng khác
                bool rerouted = reRouteTrain(tv);
                if (!rerouted) {
                    // Không có đường vòng -> DỪNG LẠI TẠI GA, TUYỆT ĐỐI KHÔNG CHẠY VÀO GA NGHẼN!
                    tv->isWaitingBlock = true;
                    tv->waitingForTrainId = "GA NGHEN: " + tv->toStation->id;
                    tv->moving = false;
                    statusMsg = ">> DUNG TAU: Tau " + tv->model->id + " dung cho tai " + tv->fromStation->id + " do ga " + tv->toStation->id + " bi tac nghen!";
                    continue;
                }
            }

            // KIỂM TRA 2: ĐOẠN RAY CÓ BỊ TÀU KHÁC CHIẾM KHÔNG?
            TrainView* occupier = getSegmentOccupier(tv->fromStation, tv->toStation, tv);
            if (occupier) {
                // Ray phía trước đang bị chiếm -> Dừng chờ tại ga
                tv->isWaitingBlock = true;
                tv->waitingForTrainId = occupier->model->id;
                tv->moving = false;

                int ord = 0;
                for (TrainView* o : trainViews) {
                    if (o != tv && o->isWaitingBlock && o->fromStation == tv->fromStation && o->toStation == tv->toStation)
                        ord++;
                }
                tv->waitingOrder = ord;
                statusMsg = ">> TIN HIEU DO: Tau " + tv->model->id + " dung cho tai ga " + tv->fromStation->id + " (ray " + tv->fromStation->id + " -> " + tv->toStation->id + " bi chiem boi " + occupier->model->id + ")!";
            } else {
                // Đoạn ray trống và ga không bị nghẽn -> Cho phép vào chặng kế tiếp
                tv->setSegment(tv->fromStation, tv->toStation, stationPositions);
                tv->moving = true;
                tv->isWaitingBlock = false;
                tv->waitingForTrainId = "";
            }
        }
    }

    // 3. GIẢI PHÓNG ĐOẠN RAY VÀ CẤP PHÉP XUẤT PHÁT CHO TÀU CÓ MỨC ƯU TIÊN CAO NHẤT:
    // Duyệt qua tất cả các tàu đang chờ. Nếu đoạn ray của chúng đã thông thoáng VÀ ga đích không bị tắc:
    for (TrainView* w : trainViews) {
        if (!w->isWaitingBlock || w->isFinished || !w->fromStation || !w->toStation) continue;
        Station *sA = w->fromStation, *sB = w->toStation;

        // Nếu ga đích đang bị tắc, thử tái định tuyến
        if (sB->isBlocked) {
            reRouteTrain(w);
            continue;
        }

        if (!isSegmentOccupied(sA, sB, nullptr)) {
            // Tìm tất cả các tàu đang chờ cùng đoạn ray sA -> sB
            TrainView* best = w;
            for (TrainView* o : trainViews) {
                if (o->isWaitingBlock && o->fromStation == sA && o->toStation == sB) {
                    if (o->model->priority < best->model->priority ||
                       (o->model->priority == best->model->priority && o->waitingOrder < best->waitingOrder)) {
                        best = o;
                    }
                }
            }

            // Cấp lệnh xuất phát cho tàu ưu tiên cao nhất
            best->isWaitingBlock = false;
            best->waitingForTrainId = "";
            best->waitingOrder = 0;
            best->setSegment(sA, sB, stationPositions);
            best->moving = true;
            best->dwellTimer = 0.2f;

            statusMsg = ">> TIN HIEU XANH: Giai phong ray " + sA->id + " -> " + sB->id + " cho tau " + best->model->id + " (UT" + to_string(best->model->priority) + ")!";

            // Cập nhật lại các tàu còn lại tiếp tục chờ tàu 'best' này
            for (TrainView* o : trainViews) {
                if (o != best && o->isWaitingBlock && o->fromStation == sA && o->toStation == sB) {
                    o->waitingForTrainId = best->model->id;
                    if (o->waitingOrder > 0) o->waitingOrder--;
                }
            }
            break;
        }
    }
}

void SimulationApp::render() {
    ClearBackground(UI_BG_DARK);

    // 1. Bản đồ vệ tinh Đà Nẵng sắc nét không vết mực đè
    if (mapTextureLoaded && showSatelliteMap) {
        Rectangle src = {0.0f, 0.0f, (float)mapTexture.width, (float)mapTexture.height};
        Rectangle dst = {mapOffsetX, mapOffsetY, mapTexture.width * mapScale, mapTexture.height * mapScale};
        DrawTexturePro(mapTexture, src, dst, {0, 0}, 0.0f, Color{230, 240, 255, 245});
        DrawRectangleLinesEx(dst, ScaleF(2.0f), Color{60, 110, 190, 180});
    }

    // 2. Vẽ toàn bộ mạng lưới đường ray 2 chiều (Double tracks + tà-vẹt + mũi tên)
    for (auto* tv : trackViews) tv->draw();

    // 3. Vẽ vệt sáng lộ trình Dijkstra hiện tại (bám chuẩn làn ray bên phải)
    if (currentPath.size() >= 2) {
        float pulse = (sinf((float)GetTime() * 4.0f) + 1.0f) * 0.5f;
        for (size_t i = 0; i < currentPath.size() - 1; ++i) {
            Station* sA = currentPath[i];
            Station* sB = currentPath[i + 1];
            if (!stationPositions.count(sA->id) || !stationPositions.count(sB->id)) continue;

            Vector2 p1 = stationPositions.at(sA->id), p2 = stationPositions.at(sB->id);
            float dx = p2.x - p1.x, dy = p2.y - p1.y;
            float len = sqrtf(dx * dx + dy * dy);
            if (len > 0.001f) {
                float nx = -dy / len, ny = dx / len;
                float off = ScaleF(4.2f);
                Vector2 p1Off = {p1.x + nx * off, p1.y + ny * off};
                Vector2 p2Off = {p2.x + nx * off, p2.y + ny * off};
                DrawLineEx(p1Off, p2Off, ScaleF(8.0f), Color{0, 220, 255, (unsigned char)(60 + (int)(pulse * 90))});
                DrawLineEx(p1Off, p2Off, ScaleF(3.0f), Color{255, 255, 255, (unsigned char)(180 + (int)(pulse * 75))});
            }
        }
    }

    // 4. Vẽ các nhà ga và nhãn
    for (auto& [id, sv] : stationViews) sv->draw();
    for (auto& [id, sv] : stationViews) sv->drawLabel();

    // 5. Vẽ các đoàn tàu và nhãn tàu
    for (auto* tv : trainViews) { tv->draw(); tv->drawLabel(); }

    // 6. Header bar
    int headerH = ScaleI(44.0f);
    DrawRectangle(0, 0, screenW, headerH, UI_PANEL_COLOR);
    DrawLine(0, headerH, screenW, headerH, UI_PANEL_BORDER);
    const char* title = "HE THONG DIEU PHOI TAU HOA DAN DUNG - TP. DA NANG";
    int tSize = ScaleI(17.0f);
    DrawTextBold(title, (int)((screenW - MeasureTextBold(title, tSize).x) * 0.5f), ScaleI(12.0f), tSize, UI_TEXT_PRIMARY);

    // 7. Bảng điều khiển Trái (Thiết lập & Lập lịch) và Phải (Giám sát & Sự cố)
    UIPanel::drawLeftPanel(this);
    UIPanel::drawRightPanel(this);
    UIPanel::drawStatusBar(this);
}

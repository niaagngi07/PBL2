#ifndef TRAIN_VIEW_H
#define TRAIN_VIEW_H

#include "raylib.h"
#include "Train.h"
#include "StationView.h"
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

class TrainView {
public:
    Train*   model;
    Vector2  currentPos;   // Vị trí thực tế trên màn hình (bám chuẩn làn ray đôi bên phải)
    Vector2  targetPos;    // Tọa độ đích đến của chặng hiện tại
    float    speed;        // Vận tốc di chuyển (pixel/giây)
    bool     moving;       // Đang chạy hay đang đỗ tại ga
    float    angle;        // Góc xoay hướng di chuyển
    float    dwellTimer;   // Thời gian tạm dừng tại ga đón/trả khách (giây)
    float    finishTimer;  // Đếm ngược sau khi về đích
    bool     isFinished;   // Đã hoàn thành toàn bộ lộ trình
    bool     hasAnnouncedArrival; // Đã ghi nhận vào bảng thông báo về đích chưa

    // Điều phối đoạn ray & chờ tín hiệu an toàn
    Station* fromStation;          // Ga hiện tại / ga bắt đầu của chặng
    Station* toStation;            // Ga mục tiêu tiếp theo của chặng
    bool     isWaitingBlock;       // Đang chờ tại ga do đoạn ray phía trước bị chiếm dụng
    string   waitingForTrainId;    // ID của tàu đang chạy trên đoạn ray mà tàu này phải đợi
    int      waitingOrder;         // Thứ tự xếp hàng chờ tại ga (0, 1, 2, ...)

    // Vệt sáng di chuyển phía sau tàu
    vector<Vector2> trail;

    TrainView(Train* train, Station* startSt, Station* firstTargetSt,
              const unordered_map<string, Vector2>& stationPositions);

    // Tính toán tọa độ hiển thị có offset lùi nhẹ nếu đang xếp hàng chờ
    Vector2 getDisplayPos() const;

    // Màu sắc theo cấp ưu tiên: MRT (đỏ), LRT (vàng cam), LRT_DL (xanh lá)
    Color resolveColor() const;

    // Thiết lập chặng di chuyển mới bám chuẩn làn ray đôi
    void setSegment(Station* src, Station* dest, const unordered_map<string, Vector2>& stationPositions);

    // Cập nhật vị trí mượt mà
    void update(float dt);

    void draw() const;
    void drawLabel() const;

    bool shouldRemove() const { return isFinished && finishTimer > 10.0f; }
};

#endif // TRAIN_VIEW_H

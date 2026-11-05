#ifndef STATION_VIEW_H
#define STATION_VIEW_H

#include <string>
#include <vector>
#include "raylib.h"
#include "Station.h"
#include "Track.h"

using namespace std;

// Màu sắc phân loại tuyến dựa trên speedLimit của track kết nối
// MRT (>=80): Đỏ | LRT (>=60): Vàng cam | LRT_DL (>=40): Xanh lá

class StationView {
public:
    Station* model;
    Vector2  pos;         // Tọa độ màn hình (pixel)
    float    radius;
    bool     isHovered;
    bool     isSelected;  // Được chọn làm điểm xuất phát hoặc đích
    Color    baseColor;

    StationView(Station* station, float x, float y);

    // Xác định màu dựa vào loại tuyến kết nối (tốc độ cao nhất trong adj)
    void resolveColor();

    void update(Vector2 mousePos);
    void draw() const;
    void drawLabel() const;

    bool contains(Vector2 point) const;
};

#endif // STATION_VIEW_H

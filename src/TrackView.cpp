#include "TrackView.h"
#include "RenderUtils.h"
#include <cmath>

using namespace std;

TrackView::TrackView(Track* track, StationView* src, StationView* dest)
    : model(track), srcView(src), destView(dest) {}

Color TrackView::resolveColor() const {
    if (model->isOccupied || srcView->model->isBlocked || destView->model->isBlocked) {
        return Color{130, 95, 95, 210}; // Bị chiếm hoặc tắc nghẽn — Xám đỏ
    }

    double spd = model->speedLimit;
    if (spd >= 75.0) {
        return Color{225, 60,  60,  210}; // MRT    — Đỏ
    } else if (spd >= 55.0) {
        return Color{245, 180, 30,  210}; // LRT    — Vàng cam
    } else if (spd >= 35.0) {
        return Color{60,  210, 95,  210}; // LRT DL — Xanh lá
    }
    return Color{145, 145, 155, 180};     // Kết nối phụ — Xám nhạt
}

void TrackView::draw() const {
    Color c = resolveColor();

    Vector2 s = srcView->pos;
    Vector2 d = destView->pos;

    float dx = d.x - s.x;
    float dy = d.y - s.y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < ScaleF(12.0f)) return;

    // Pháp tuyến đơn vị sang bên phải theo hướng di chuyển s -> d
    float nx = -dy / len;
    float ny =  dx / len;
    float offsetDist = ScaleF(4.2f); // Lệch đường ray đôi sang làn phải

    // Tọa độ hai đầu của làn ray chiều này
    Vector2 sOff = {s.x + nx * offsetDist, s.y + ny * offsetDist};
    Vector2 dOff = {d.x + nx * offsetDist, d.y + ny * offsetDist};

    // 1. Vẽ tà-vẹt (sleepers) nối hai làn ray song song (chỉ vẽ 1 lần cho cặp ga để tránh vẽ đè)
    if (srcView->model->id < destView->model->id) {
        float stepDist = ScaleF(16.0f);
        int numSteps = (int)(len / stepDist);
        for (int i = 1; i < numSteps; ++i) {
            float u = (float)i / (float)numSteps;
            Vector2 midP = {s.x + dx * u, s.y + dy * u};
            Vector2 tieA = {midP.x + nx * (offsetDist + ScaleF(2.0f)), midP.y + ny * (offsetDist + ScaleF(2.0f))};
            Vector2 tieB = {midP.x - nx * (offsetDist + ScaleF(2.0f)), midP.y - ny * (offsetDist + ScaleF(2.0f))};
            DrawLineEx(tieA, tieB, ScaleF(1.8f), Color{170, 185, 205, 110});
        }
    }

    // 2. Thân ray chính của làn
    DrawLineEx(sOff, dOff, ScaleF(3.2f), c);

    // 3. Lõi kim loại phản quang sáng giữa thanh ray
    DrawLineEx(sOff, dOff, ScaleF(1.2f), Color{255, 255, 255, 140});

    // 4. Mũi tên chỉ hướng lưu thông của riêng làn này
    Vector2 midOff = {(sOff.x + dOff.x) * 0.5f, (sOff.y + dOff.y) * 0.5f};
    float angle = atan2f(dy, dx);
    float arrowSize = ScaleF(6.5f);
    Vector2 arrowP1 = {
        midOff.x - arrowSize * cosf(angle - 0.55f),
        midOff.y - arrowSize * sinf(angle - 0.55f)
    };
    Vector2 arrowP2 = {
        midOff.x - arrowSize * cosf(angle + 0.55f),
        midOff.y - arrowSize * sinf(angle + 0.55f)
    };
    DrawLineEx(midOff, arrowP1, ScaleF(1.8f), Color{255, 255, 255, 200});
    DrawLineEx(midOff, arrowP2, ScaleF(1.8f), Color{255, 255, 255, 200});
}

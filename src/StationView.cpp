#include "StationView.h"
#include "RenderUtils.h"
#include <cmath>
#include <algorithm>

using namespace std;

StationView::StationView(Station* station, float x, float y)
    : model(station), pos{x, y}, radius(ScaleF(13.0f)),
      isHovered(false), isSelected(false), baseColor(GRAY)
{
    resolveColor();
}

void StationView::resolveColor() {
    double maxSpeed = 0.0;
    for (Track* t : model->adjTracks)
        if (t->speedLimit > maxSpeed) maxSpeed = t->speedLimit;

    if (maxSpeed >= 75.0)
        baseColor = Color{220,  55,  55, 255}; // MRT — Đỏ
    else if (maxSpeed >= 55.0)
        baseColor = Color{240, 175,  30, 255}; // LRT — Vàng cam
    else if (maxSpeed >= 35.0)
        baseColor = Color{ 55, 190,  85, 255}; // LRT DL — Xanh lá
    else
        baseColor = Color{145, 145, 158, 255}; // Kết nối phụ — Xám
}

void StationView::update(Vector2 mousePos) {
    isHovered = contains(mousePos);
}

void StationView::draw() const {
    float r = isHovered ? (radius + ScaleF(4.0f)) : radius;

    // Hiệu ứng cảnh báo nếu ga đang bị sự cố/tắc nghẽn
    if (model->isBlocked) {
        float pulse = (sinf((float)GetTime() * 6.0f) + 1.0f) * 0.5f;
        DrawCircleV(pos, r + ScaleF(9.0f), Color{255, 40, 40, (unsigned char)(90 + pulse * 140)});
        DrawCircleLines((int)pos.x, (int)pos.y, r + ScaleF(12.0f), Color{255, 60, 60, 230});
    }

    // Hào sáng khi hover
    if (isHovered)
        DrawCircleV(pos, r + ScaleF(7.0f), Color{255, 255, 255, 35});

    // Viền ngoài
    Color outline = Color{255, 255, 255, 190};
    if (model->isBlocked) outline = Color{255, 60, 60, 255};
    else if (isSelected) outline = Color{255, 215, 0, 255}; // Vàng gold
    DrawCircleV(pos, r + ScaleF(3.0f), outline);

    // Thân chính
    DrawCircleV(pos, r, model->isBlocked ? Color{160, 40, 40, 255} : baseColor);

    // Điểm trắng giữa (trông như biển hiệu ga hoặc dấu X nếu bị block)
    if (model->isBlocked) {
        DrawCircleV(pos, r * 0.45f, Color{255, 220, 220, 240});
        DrawLineEx({pos.x - r * 0.35f, pos.y - r * 0.35f}, {pos.x + r * 0.35f, pos.y + r * 0.35f}, ScaleF(2.0f), Color{200, 20, 20, 255});
        DrawLineEx({pos.x - r * 0.35f, pos.y + r * 0.35f}, {pos.x + r * 0.35f, pos.y - r * 0.35f}, ScaleF(2.0f), Color{200, 20, 20, 255});
    } else {
        DrawCircleV(pos, r * 0.38f, Color{255, 255, 255, 220});
    }
}

void StationView::drawLabel() const {
    if (!isHovered && !isSelected && !model->isBlocked) return;

    string txt = model->name + (model->isBlocked ? " [TAC NGHEN / SU CO]" : "");
    int fontSize = ScaleI(13.0f);
    Vector2 sz = MeasureTextBold(txt.c_str(), fontSize);

    float padX = ScaleF(10.0f);
    float padY = ScaleF(5.0f);
    float bx = pos.x - sz.x * 0.5f - padX;
    float by = (pos.y < ScaleF(90.0f)) ? (pos.y + radius + ScaleF(12.0f))
                                       : (pos.y - radius - sz.y - padY * 2.0f - ScaleF(8.0f));

    bx = max(ScaleF(14.0f), min(bx, (float)GetScreenWidth() - sz.x - padX * 2.0f - ScaleF(14.0f)));

    Rectangle rec = {bx, by, sz.x + padX * 2.0f, sz.y + padY * 2.0f};
    Color bgCol = model->isBlocked ? Color{65, 15, 15, 245} : Color{15, 24, 42, 235};
    Color bdrCol = model->isBlocked ? Color{255, 60, 60, 240} : (isSelected ? Color{255, 215, 70, 230} : Color{80, 140, 220, 200});
    Color txtCol = model->isBlocked ? Color{255, 140, 140, 255} : (isSelected ? Color{255, 235, 120, 255} : UI_TEXT_PRIMARY);

    DrawRectangleRec(rec, bgCol);
    DrawRectangleLinesEx(rec, ScaleF(1.5f), bdrCol);
    DrawTextBold(txt.c_str(), (int)(bx + padX), (int)(by + padY), fontSize, txtCol);
}

bool StationView::contains(Vector2 point) const {
    float dx = point.x - pos.x;
    float dy = point.y - pos.y;
    float hitR = radius + ScaleF(6.0f);
    return (dx * dx + dy * dy) <= (hitR * hitR);
}

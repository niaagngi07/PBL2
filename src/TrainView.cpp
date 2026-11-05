#include "TrainView.h"
#include "RenderUtils.h"
#include <cmath>
#include <algorithm>

using namespace std;

TrainView::TrainView(Train* train, Station* startSt, Station* firstTargetSt,
                     const unordered_map<string, Vector2>& stationPositions)
    : model(train), moving(true), angle(0.0f), dwellTimer(0.0f), finishTimer(0.0f),
      isFinished(false), hasAnnouncedArrival(false),
      fromStation(startSt), toStation(firstTargetSt),
      isWaitingBlock(false), waitingForTrainId(""), waitingOrder(0)
{
    float baseSpd = (model->type == TrainType::MRT) ? 180.0f : ((model->type == TrainType::LRT) ? 135.0f : 95.0f);
    speed = ScaleF(baseSpd);

    setSegment(startSt, firstTargetSt, stationPositions);
}

void TrainView::setSegment(Station* src, Station* dest, const unordered_map<string, Vector2>& stationPositions) {
    fromStation = src;
    toStation = dest;

    if (!src || !dest || !stationPositions.count(src->id) || !stationPositions.count(dest->id)) return;

    Vector2 s = stationPositions.at(src->id);
    Vector2 d = stationPositions.at(dest->id);
    float dx = d.x - s.x;
    float dy = d.y - s.y;
    float len = sqrtf(dx * dx + dy * dy);

    if (len > 0.001f) {
        float nx = -dy / len;
        float ny =  dx / len;
        float offsetDist = ScaleF(4.2f); // Lệch vào làn ray phải của đường đôi
        currentPos = {s.x + nx * offsetDist, s.y + ny * offsetDist};
        targetPos  = {d.x + nx * offsetDist, d.y + ny * offsetDist};
        angle = atan2f(dy, dx) * RAD2DEG;
    } else {
        currentPos = s;
        targetPos  = d;
    }
}

Vector2 TrainView::getDisplayPos() const {
    if (isWaitingBlock && waitingOrder > 0) {
        float backOffset = ScaleF(18.0f * (float)waitingOrder);
        float rad = angle * DEG2RAD;
        return {currentPos.x - backOffset * cosf(rad), currentPos.y - backOffset * sinf(rad)};
    }
    return currentPos;
}

Color TrainView::resolveColor() const {
    if (model->type == TrainType::MRT) return Color{255, 75, 75, 255};
    if (model->type == TrainType::LRT) return Color{255, 195, 40, 255};
    return Color{70, 225, 110, 255};
}

void TrainView::update(float dt) {
    if (isWaitingBlock) {
        moving = false;
        return;
    }
    if (isFinished) {
        finishTimer += dt;
        return;
    }
    if (dwellTimer > 0.0f) {
        dwellTimer -= dt;
        return;
    }
    if (!moving) return;

    float dx = targetPos.x - currentPos.x;
    float dy = targetPos.y - currentPos.y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist > 0.1f) angle = atan2f(dy, dx) * RAD2DEG;

    float step = speed * dt;
    if (step >= dist || dist <= ScaleF(2.5f)) {
        currentPos = targetPos;
        moving = false;
        dwellTimer = 0.35f; // Tạm dừng tại ga đón/trả khách

        // Hoàn tất chặng hiện tại: lấy điểm đến của chặng làm ga hiện tại
        model->moveNext();

        if (model->hasReachedDestination()) {
            isFinished = true;
        } else {
            // Chuẩn bị cho chặng tiếp theo
            fromStation = toStation;
            toStation = model->getNextStation();
        }
    } else {
        currentPos.x += (dx / dist) * step;
        currentPos.y += (dy / dist) * step;
    }

    trail.push_back(currentPos);
    if (trail.size() > 55) trail.erase(trail.begin());
}

void TrainView::draw() const {
    Color c = resolveColor();
    Vector2 dPos = getDisplayPos();

    float alpha = (isFinished && finishTimer > 4.0f) ? max(0.0f, 1.0f - (finishTimer - 4.0f) / 4.0f) : 1.0f;
    c.a = (unsigned char)(c.a * alpha);

    if (!isWaitingBlock && moving) {
        for (size_t i = 1; i < trail.size(); ++i) {
            float p = (float)i / (float)trail.size();
            DrawLineEx(trail[i - 1], trail[i], ScaleF(3.5f) * p, {c.r, c.g, c.b, (unsigned char)(p * 90 * alpha)});
        }
    }

    float tw = ScaleF(28.0f), th = ScaleF(13.0f);
    Rectangle body = {dPos.x, dPos.y, tw, th};
    Vector2 origin = {tw * 0.5f, th * 0.5f};

    Color glowCol = isWaitingBlock ? Color{255, 180, 40, (unsigned char)(80 * alpha)} : Color{c.r, c.g, c.b, (unsigned char)(60 * alpha)};
    DrawRectanglePro({dPos.x, dPos.y, tw + ScaleF(6.0f), th + ScaleF(6.0f)}, {(tw + ScaleF(6.0f)) * 0.5f, (th + ScaleF(6.0f)) * 0.5f}, angle, glowCol);
    DrawRectanglePro(body, origin, angle, c);
    Color bdrCol = isWaitingBlock ? Color{255, 220, 100, 240} : Color{(unsigned char)(c.r / 3), (unsigned char)(c.g / 3), (unsigned char)(c.b / 3), (unsigned char)(180 * alpha)};
    DrawRectanglePro(body, origin, angle, bdrCol);

    float rad = angle * DEG2RAD, hx = dPos.x + (tw * 0.5f) * cosf(rad), hy = dPos.y + (tw * 0.5f) * sinf(rad);
    DrawCircleV({hx, hy}, ScaleF(3.5f), isWaitingBlock ? Color{255, 200, 80, (unsigned char)(200 * alpha)} : Color{255, 255, 230, (unsigned char)(230 * alpha)});

    if (!isWaitingBlock && moving) {
        float blen = ScaleF(26.0f);
        Vector2 b1 = {hx + blen * cosf(rad - 0.25f), hy + blen * sinf(rad - 0.25f)};
        Vector2 b2 = {hx + blen * cosf(rad + 0.25f), hy + blen * sinf(rad + 0.25f)};
        DrawTriangle({hx, hy}, b1, b2, Color{255, 255, 200, (unsigned char)(35 * alpha)});
    }
}

void TrainView::drawLabel() const {
    if (isFinished && finishTimer > 5.0f) return;

    Vector2 dPos = getDisplayPos();
    string text = model->id + (isFinished ? " [DA DEN DICH]" : (isWaitingBlock ? " [CHO RAY: " + waitingForTrainId + "]" : (dwellTimer > 0.0f ? " [DO GA]" : "")));

    int fSize = ScaleI(12.0f);
    Vector2 sz = MeasureTextF(text.c_str(), fSize);
    float padX = ScaleF(7.0f), padY = ScaleF(4.0f), bx = dPos.x - sz.x * 0.5f - padX, by = dPos.y - ScaleF(26.0f);

    Color bg = isFinished ? Color{20, 60, 30, 225} : (isWaitingBlock ? Color{55, 35, 10, 235} : Color{15, 22, 40, 225});
    Color bdr = isFinished ? Color{80, 220, 120, 210} : (isWaitingBlock ? Color{255, 190, 40, 240} : Color{90, 140, 220, 190});
    Color txtCol = isFinished ? Color{120, 255, 160, 255} : (isWaitingBlock ? Color{255, 225, 120, 255} : UI_TEXT_PRIMARY);

    Rectangle rec = {bx, by, sz.x + padX * 2.0f, sz.y + padY * 2.0f};
    DrawRectangleRec(rec, bg);
    DrawRectangleLinesEx(rec, ScaleF(1.2f), bdr);
    DrawTextBold(text.c_str(), (int)(bx + padX), (int)(by + padY), fSize, txtCol);
}

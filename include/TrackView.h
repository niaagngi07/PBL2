#ifndef TRACK_VIEW_H
#define TRACK_VIEW_H

#include <string>
#include <vector>
#include "raylib.h"
#include "Track.h"
#include "StationView.h"

using namespace std;

class TrackView {
public:
    Track*       model;
    StationView* srcView;
    StationView* destView;

    TrackView(Track* track, StationView* src, StationView* dest);

    // Chọn màu theo loại tuyến: MRT=đỏ, LRT=vàng, LRT_DL=xanh lá, occupied=xám
    Color resolveColor() const;

    void draw() const;
};

#endif // TRACK_VIEW_H

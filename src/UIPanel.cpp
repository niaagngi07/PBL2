#include "UIPanel.h"
#include "SimulationApp.h"
#include "RenderUtils.h"
#include "MapDatabase.h"
#include <algorithm>

using namespace std;

void UIPanel::drawModernPanel(int x, int y, int w, int h, const char* title) {
    DrawRectangle(x, y, w, h, UI_PANEL_COLOR);
    DrawRectangleLines(x, y, w, h, UI_PANEL_BORDER);
    int headH = ScaleI(38.0f);
    DrawRectangle(x, y, w, headH, UI_PANEL_HEADER);
    DrawLine(x, y + headH, x + w, y + headH, UI_PANEL_BORDER);
    DrawTextBold(title, x + ScaleI(16.0f), y + ScaleI(9.0f), ScaleI(16.0f), UI_TEXT_ACCENT);
}

void UIPanel::drawLeftPanel(SimulationApp* app) {
    int pw = app->getPanelWidth();
    int px = ScaleI(14.0f);
    int py = ScaleI(52.0f);
    int ph = app->screenH - py - ScaleI(54.0f);

    drawModernPanel(px, py, pw, ph, "THIET LAP & LAP LICH TAU");

    int tx = px + ScaleI(14.0f);
    int tw = pw - ScaleI(28.0f);
    int curY = py + ScaleI(46.0f);

    const auto& stations = MapDatabase::getStations();
    Vector2 mouse = GetMousePosition();

    // ── ĐỀ MỤC 1: CHỌN LOẠI TÀU (3 MÀU) & SỐ HIỆU ───────────────────────────
    DrawTextBold("1. CHON LOAI TAU & SO HIEU:", tx, curY, ScaleI(15.0f), UI_TEXT_GOLD);
    curY += ScaleI(24.0f);

    struct TypeBtn { TrainType type; const char* name; Color col; const char* spd; };
    TypeBtn typeBtns[] = {
        {TrainType::MRT,         "MRT",    Color{255,  75,  75, 255}, "80 km/h"},
        {TrainType::LRT,         "LRT",    Color{255, 195,  40, 255}, "60 km/h"},
        {TrainType::LRT_TOURIST, "LRT DL", Color{ 70, 225, 110, 255}, "40 km/h"}
    };

    float btnGap = ScaleF(8.0f);
    float btnW = ((float)tw - btnGap * 2.0f) / 3.0f;
    float btnH = ScaleF(38.0f);

    for (int i = 0; i < 3; ++i) {
        bool act = (app->activeTrainType == typeBtns[i].type);
        Rectangle r = {(float)tx + i * (btnW + btnGap), (float)curY, btnW, btnH};
        bool hov = CheckCollisionPointRec(mouse, r);

        Color bg = act ? Color{35, 75, 130, 245} : (hov ? Color{28, 40, 68, 220} : Color{18, 26, 45, 190});
        DrawRectangleRec(r, bg);
        DrawRectangleLinesEx(r, act ? 2.0f : 1.0f, act ? UI_TEXT_GOLD : (hov ? UI_TEXT_ACCENT : Color{60, 90, 140, 180}));

        DrawCircle((int)(r.x + ScaleF(14.0f)), (int)(r.y + btnH * 0.5f), ScaleF(6.0f), typeBtns[i].col);
        DrawTextBold(typeBtns[i].name, (int)(r.x + ScaleF(26.0f)), (int)(r.y + ScaleF(9.0f)), ScaleI(14.0f), act ? UI_TEXT_GOLD : UI_TEXT_PRIMARY);
    }
    curY += (int)btnH + ScaleI(10.0f);

    // Hàng chọn số hiệu tàu
    DrawTextF("Chon so hieu tau (Click de doi so):", tx, curY, ScaleI(12.0f), UI_TEXT_SECONDARY);
    curY += ScaleI(18.0f);

    int maxNum = (app->activeTrainType == TrainType::MRT) ? 4 : ((app->activeTrainType == TrainType::LRT) ? 11 : 4);
    string prefix = (app->activeTrainType == TrainType::MRT) ? "MRT-" : ((app->activeTrainType == TrainType::LRT) ? "LRT-" : "DL-");

    int pillsPerRow = (app->activeTrainType == TrainType::LRT) ? 6 : 4;
    float pillGap = ScaleF(5.0f);
    float pillH = ScaleF(28.0f);

    for (int n = 1; n <= maxNum; ++n) {
        int row = (n - 1) / pillsPerRow;
        int col = (n - 1) % pillsPerRow;
        int itemsInThisRow = (row == 0 && app->activeTrainType == TrainType::LRT) ? 6 : (app->activeTrainType == TrainType::LRT ? 5 : 4);
        float pW = ((float)tw - pillGap * (float)(itemsInThisRow - 1)) / (float)itemsInThisRow;
        float pX = (float)tx + col * (pW + pillGap);
        float pY = (float)curY + row * (pillH + ScaleF(5.0f));

        bool act = (app->selectedTrainNum == n);
        Rectangle pr = {pX, pY, pW, pillH};
        bool hov = CheckCollisionPointRec(mouse, pr);

        Color pBg = act ? Color{45, 105, 175, 245} : (hov ? Color{30, 48, 80, 210} : Color{18, 25, 45, 180});
        DrawRectangleRec(pr, pBg);
        DrawRectangleLinesEx(pr, act ? 1.8f : 1.0f, act ? UI_TEXT_GOLD : (hov ? UI_TEXT_ACCENT : Color{60, 85, 125, 160}));

        string lbl = (app->activeTrainType == TrainType::MRT ? "M" : (app->activeTrainType == TrainType::LRT ? "L" : "D")) + to_string(n);
        Vector2 mSz = MeasureTextBold(lbl.c_str(), ScaleI(12.0f));
        DrawTextBold(lbl.c_str(), (int)(pX + (pW - mSz.x) * 0.5f), (int)(pY + ScaleF(6.0f)), ScaleI(12.0f), act ? UI_TEXT_GOLD : UI_TEXT_PRIMARY);
    }
    curY += (app->activeTrainType == TrainType::LRT ? ScaleI(66.0f) : ScaleI(34.0f)) + ScaleI(8.0f);

    DrawLine(px + ScaleI(8.0f), curY, px + pw - ScaleI(8.0f), curY, UI_PANEL_BORDER);
    curY += ScaleI(10.0f);

    // ── ĐỀ MỤC 2: CHỌN GA ĐẦU (XUẤT PHÁT) ────────────────────────────────────
    DrawTextBold("2. CHON GA DAU (XUAT PHAT):", tx, curY, ScaleI(15.0f), UI_TEXT_GOLD);
    curY += ScaleI(20.0f);

    // Banner ga đầu được chọn
    string origName = (app->originIndex >= 0 && app->originIndex < (int)stations.size())
                      ? (stations[app->originIndex].id + " - " + stations[app->originIndex].name) : "Chua chon ga dau";
    Rectangle bannerOrig = {(float)tx, (float)curY, (float)tw, ScaleF(30.0f)};
    DrawRectangleRec(bannerOrig, Color{15, 40, 75, 230});
    DrawRectangleLinesEx(bannerOrig, 1.2f, Color{0, 200, 255, 220});
    DrawTextBold(origName.c_str(), tx + ScaleI(10.0f), curY + ScaleI(6.0f), ScaleI(13.0f), Color{140, 235, 255, 255});
    curY += ScaleI(36.0f);

    // Khung cuộn danh sách ga đầu
    int listH = ScaleI(115.0f);
    Rectangle origBox = {(float)tx, (float)curY, (float)tw, (float)listH};
    DrawRectangleRec(origBox, Color{12, 18, 32, 220});
    DrawRectangleLinesEx(origBox, 1.0f, UI_PANEL_BORDER);

    int rowH = ScaleI(28.0f);
    int visibleRows = listH / rowH;
    int scrollOrig = clamp(app->scrollOrigin, 0, max(0, (int)stations.size() - visibleRows));

    for (int i = 0; i < visibleRows && (scrollOrig + i) < (int)stations.size(); ++i) {
        int idx = scrollOrig + i;
        bool isSel = (app->originIndex == idx);
        Rectangle rRow = {(float)tx + 2.0f, (float)(curY + i * rowH + 1), (float)tw - ScaleF(28.0f), (float)rowH - 2.0f};
        bool hov = CheckCollisionPointRec(mouse, rRow);

        Station* st = app->graph.getStation(stations[idx].id);
        bool isBlk = st ? st->isBlocked : false;

        if (isSel) {
            DrawRectangleRec(rRow, Color{25, 75, 135, 240});
            DrawRectangleLinesEx(rRow, 1.2f, UI_TEXT_GOLD);
        } else if (hov) {
            DrawRectangleRec(rRow, Color{25, 40, 65, 200});
        }

        string itemText = stations[idx].id + ": " + stations[idx].name;
        Color txtCol = isSel ? UI_TEXT_GOLD : (isBlk ? Color{255, 95, 95, 255} : (hov ? UI_TEXT_ACCENT : UI_TEXT_PRIMARY));
        DrawTextBold(itemText.c_str(), tx + ScaleI(8.0f), curY + i * rowH + ScaleI(6.0f), ScaleI(12.0f), txtCol);
    }

    // Nút cuộn Lên/Xuống cho ga đầu
    Rectangle btnOrigUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(listH / 2 - ScaleI(3.0f))};
    Rectangle btnOrigDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + listH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(listH / 2 - ScaleI(3.0f))};
    DrawRectangleRec(btnOrigUp, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnOrigUp, 1.0f, UI_PANEL_BORDER);
    DrawTextBold("^", (int)btnOrigUp.x + ScaleI(6.0f), (int)btnOrigUp.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);
    DrawRectangleRec(btnOrigDown, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnOrigDown, 1.0f, UI_PANEL_BORDER);
    DrawTextBold("v", (int)btnOrigDown.x + ScaleI(6.0f), (int)btnOrigDown.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);

    curY += listH + ScaleI(8.0f);
    DrawLine(px + ScaleI(8.0f), curY, px + pw - ScaleI(8.0f), curY, UI_PANEL_BORDER);
    curY += ScaleI(10.0f);

    // ── ĐỀ MỤC 3: CHỌN GA CUỐI (ĐÍCH ĐẾN) ────────────────────────────────────
    DrawTextBold("3. CHON GA CUOI (DICH DEN):", tx, curY, ScaleI(15.0f), UI_TEXT_GOLD);
    curY += ScaleI(20.0f);

    // Banner ga cuối được chọn
    string destName = (app->destIndex >= 0 && app->destIndex < (int)stations.size())
                      ? (stations[app->destIndex].id + " - " + stations[app->destIndex].name) : "Chua chon ga dich";
    Rectangle bannerDest = {(float)tx, (float)curY, (float)tw, ScaleF(30.0f)};
    DrawRectangleRec(bannerDest, Color{50, 40, 15, 230});
    DrawRectangleLinesEx(bannerDest, 1.2f, Color{255, 190, 40, 220});
    DrawTextBold(destName.c_str(), tx + ScaleI(10.0f), curY + ScaleI(6.0f), ScaleI(13.0f), Color{255, 225, 140, 255});
    curY += ScaleI(36.0f);

    // Khung cuộn danh sách ga cuối
    Rectangle destBox = {(float)tx, (float)curY, (float)tw, (float)listH};
    DrawRectangleRec(destBox, Color{12, 18, 32, 220});
    DrawRectangleLinesEx(destBox, 1.0f, UI_PANEL_BORDER);

    int scrollDest = clamp(app->scrollDest, 0, max(0, (int)stations.size() - visibleRows));

    for (int i = 0; i < visibleRows && (scrollDest + i) < (int)stations.size(); ++i) {
        int idx = scrollDest + i;
        bool isSel = (app->destIndex == idx);
        Rectangle rRow = {(float)tx + 2.0f, (float)(curY + i * rowH + 1), (float)tw - ScaleF(28.0f), (float)rowH - 2.0f};
        bool hov = CheckCollisionPointRec(mouse, rRow);

        Station* st = app->graph.getStation(stations[idx].id);
        bool isBlk = st ? st->isBlocked : false;

        if (isSel) {
            DrawRectangleRec(rRow, Color{45, 90, 60, 240});
            DrawRectangleLinesEx(rRow, 1.2f, UI_TEXT_GOLD);
        } else if (hov) {
            DrawRectangleRec(rRow, Color{25, 40, 65, 200});
        }

        string itemText = stations[idx].id + ": " + stations[idx].name;
        Color txtCol = isSel ? UI_TEXT_GOLD : (isBlk ? Color{255, 95, 95, 255} : (hov ? UI_TEXT_ACCENT : UI_TEXT_PRIMARY));
        DrawTextBold(itemText.c_str(), tx + ScaleI(8.0f), curY + i * rowH + ScaleI(6.0f), ScaleI(12.0f), txtCol);
    }

    // Nút cuộn Lên/Xuống cho ga cuối
    Rectangle btnDestUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(listH / 2 - ScaleI(3.0f))};
    Rectangle btnDestDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + listH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(listH / 2 - ScaleI(3.0f))};
    DrawRectangleRec(btnDestUp, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnDestUp, 1.0f, UI_PANEL_BORDER);
    DrawTextBold("^", (int)btnDestUp.x + ScaleI(6.0f), (int)btnDestUp.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);
    DrawRectangleRec(btnDestDown, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnDestDown, 1.0f, UI_PANEL_BORDER);
    DrawTextBold("v", (int)btnDestDown.x + ScaleI(6.0f), (int)btnDestDown.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);

    curY += listH + ScaleI(10.0f);

    // Thông tin lộ trình Dijkstra tính toán
    Rectangle bannerRoute = {(float)tx, (float)curY, (float)tw, ScaleF(28.0f)};
    if (!app->currentPath.empty()) {
        DrawRectangleRec(bannerRoute, Color{15, 50, 40, 220});
        DrawRectangleLinesEx(bannerRoute, 1.0f, Color{60, 200, 130, 200});
        string rInfo = "Lo trinh hop le: " + to_string(app->currentPath.size()) + " ga | Tau: " + prefix + to_string(app->selectedTrainNum);
        DrawTextBold(rInfo.c_str(), tx + ScaleI(8.0f), curY + ScaleI(6.0f), ScaleI(13.0f), Color{120, 255, 180, 255});
    } else {
        DrawRectangleRec(bannerRoute, Color{60, 20, 20, 220});
        DrawRectangleLinesEx(bannerRoute, 1.0f, Color{255, 70, 70, 200});
        DrawTextBold("CANH BAO: Khong co duong di (Ga bi tac nghen)!", tx + ScaleI(8.0f), curY + ScaleI(6.0f), ScaleI(13.0f), Color{255, 140, 140, 255});
    }
    curY += ScaleI(34.0f);

    // ── NÚT LƯU CHUYẾN TÀU (SAVE) ─────────────────────────────────────────────
    Rectangle btnSave = {(float)tx, (float)curY, (float)tw, ScaleF(40.0f)};
    bool hovSave = CheckCollisionPointRec(mouse, btnSave);
    DrawRectangleRec(btnSave, hovSave ? Color{35, 120, 70, 255} : Color{25, 95, 55, 240});
    DrawRectangleLinesEx(btnSave, 1.5f, Color{80, 240, 140, 255});
    DrawTextBold("+ LUU CHUYEN TAU NAY (ENTER)", (int)btnSave.x + ScaleI(28.0f), (int)btnSave.y + ScaleI(11.0f), ScaleI(15.0f), Color{200, 255, 220, 255});
    curY += ScaleI(48.0f);

    // ── NÚT XUẤT PHÁT TẤT CẢ (RUN) ───────────────────────────────────────────
    Rectangle btnLaunch = {(float)tx, (float)curY, (float)tw, ScaleF(42.0f)};
    bool hovLaunch = CheckCollisionPointRec(mouse, btnLaunch);
    DrawRectangleRec(btnLaunch, hovLaunch ? Color{180, 115, 25, 255} : Color{150, 95, 20, 240});
    DrawRectangleLinesEx(btnLaunch, 1.8f, UI_TEXT_GOLD);
    string lTxt = ">> XUAT PHAT (" + to_string(app->pendingFleet.size()) + " TAU CHO) [SPACE]";
    DrawTextBold(lTxt.c_str(), (int)btnLaunch.x + ScaleI(20.0f), (int)btnLaunch.y + ScaleI(12.0f), ScaleI(15.0f), Color{255, 245, 180, 255});
}

void UIPanel::drawRightPanel(SimulationApp* app) {
    int pw = app->getPanelWidth();
    int px = app->screenW - pw - ScaleI(14.0f);
    int py = ScaleI(52.0f);
    int ph = app->screenH - py - ScaleI(54.0f);

    drawModernPanel(px, py, pw, ph, "GIAM SAT & DIEU HANH MANG LUOI");

    int tx = px + ScaleI(14.0f);
    int tw = pw - ScaleI(28.0f);
    int curY = py + ScaleI(46.0f);

    const auto& stations = MapDatabase::getStations();
    Vector2 mouse = GetMousePosition();

    // ── ĐỀ MỤC 1: CHỌN GA BỊ TẮC NGHẼN / SỰ CỐ ──────────────────────────────
    DrawTextBold("1. GA BI TAC NGHEN / SU CO:", tx, curY, ScaleI(15.0f), UI_TEXT_GOLD);
    curY += ScaleI(20.0f);
    DrawTextF("Click dong de Bat/Tat su co (tranh ga):", tx, curY, ScaleI(12.0f), UI_TEXT_SECONDARY);
    curY += ScaleI(18.0f);

    int blockListH = ScaleI(125.0f);
    Rectangle blockBox = {(float)tx, (float)curY, (float)tw, (float)blockListH};
    DrawRectangleRec(blockBox, Color{12, 18, 32, 220});
    DrawRectangleLinesEx(blockBox, 1.0f, UI_PANEL_BORDER);

    int bRowH = ScaleI(30.0f);
    int bVisibleRows = blockListH / bRowH;
    int scrollBlk = clamp(app->scrollBlocked, 0, max(0, (int)stations.size() - bVisibleRows));

    for (int i = 0; i < bVisibleRows && (scrollBlk + i) < (int)stations.size(); ++i) {
        int idx = scrollBlk + i;
        Station* st = app->graph.getStation(stations[idx].id);
        bool blk = st ? st->isBlocked : false;

        Rectangle rRow = {(float)tx + 2.0f, (float)(curY + i * bRowH + 1), (float)tw - ScaleF(28.0f), (float)bRowH - 2.0f};
        bool hov = CheckCollisionPointRec(mouse, rRow);

        if (blk) {
            DrawRectangleRec(rRow, Color{60, 20, 20, 220});
            DrawRectangleLinesEx(rRow, 1.0f, Color{255, 60, 60, 180});
        } else if (hov) {
            DrawRectangleRec(rRow, Color{25, 40, 65, 200});
        }

        string sLabel = stations[idx].id + ": " + stations[idx].name;
        DrawTextBold(sLabel.c_str(), tx + ScaleI(8.0f), curY + i * bRowH + ScaleI(7.0f), ScaleI(12.0f), blk ? Color{255, 120, 120, 255} : (hov ? UI_TEXT_ACCENT : UI_TEXT_PRIMARY));

        // Nút toggle trạng thái to, rõ
        Rectangle rTgl = {(float)(tx + tw - ScaleI(140.0f)), (float)(curY + i * bRowH + ScaleI(3.0f)), ScaleF(110.0f), ScaleF(23.0f)};
        DrawRectangleRec(rTgl, blk ? Color{200, 40, 40, 245} : Color{35, 52, 78, 220});
        DrawRectangleLinesEx(rTgl, 1.2f, blk ? Color{255, 120, 120, 255} : Color{70, 105, 150, 190});
        const char* tTxt = blk ? "TAC NGHEN" : "BINH THUONG";
        DrawTextBold(tTxt, (int)(rTgl.x + ScaleF(10.0f)), (int)(rTgl.y + ScaleF(4.0f)), ScaleI(11.0f), blk ? Color{255, 255, 255, 255} : UI_TEXT_SECONDARY);
    }

    // Nút cuộn cho bảng sự cố
    Rectangle btnBlkUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(blockListH / 2 - ScaleI(3.0f))};
    Rectangle btnBlkDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + blockListH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(blockListH / 2 - ScaleI(3.0f))};
    DrawRectangleRec(btnBlkUp, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnBlkUp, 1.0f, UI_PANEL_BORDER);
    DrawTextBold("^", (int)btnBlkUp.x + ScaleI(6.0f), (int)btnBlkUp.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);
    DrawRectangleRec(btnBlkDown, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnBlkDown, 1.0f, UI_PANEL_BORDER);
    DrawTextBold("v", (int)btnBlkDown.x + ScaleI(6.0f), (int)btnBlkDown.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);

    curY += blockListH + ScaleI(10.0f);
    DrawLine(px + ScaleI(8.0f), curY, px + pw - ScaleI(8.0f), curY, UI_PANEL_BORDER);
    curY += ScaleI(10.0f);

    // ── ĐỀ MỤC 2: DANH SÁCH TÀU ĐÃ SAVE KHI ADD ──────────────────────────────
    string fleetHeader = "2. DANH SACH TAU DA LUU (" + to_string(app->pendingFleet.size()) + "):";
    DrawTextBold(fleetHeader.c_str(), tx, curY, ScaleI(15.0f), UI_TEXT_GOLD);
    curY += ScaleI(20.0f);

    int fleetListH = ScaleI(125.0f);
    Rectangle fleetBox = {(float)tx, (float)curY, (float)tw, (float)fleetListH};
    DrawRectangleRec(fleetBox, Color{12, 18, 32, 220});
    DrawRectangleLinesEx(fleetBox, 1.0f, UI_PANEL_BORDER);

    if (app->pendingFleet.empty()) {
        DrawTextF("Chua co tau nao duoc luu.", tx + ScaleI(10.0f), curY + ScaleI(16.0f), ScaleI(13.0f), UI_TEXT_SECONDARY);
        DrawTextF("Chon ga & bam [LUU CHUYEN TAU] de them.", tx + ScaleI(10.0f), curY + ScaleI(38.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);
    } else {
        int fRowH = ScaleI(30.0f);
        int fVis = fleetListH / fRowH;
        int scrollSaved = clamp(app->scrollSaved, 0, max(0, (int)app->pendingFleet.size() - fVis));

        for (int i = 0; i < fVis && (scrollSaved + i) < (int)app->pendingFleet.size(); ++i) {
            int idx = scrollSaved + i;
            const auto& qd = app->pendingFleet[idx];
            Color tc = (qd.type == TrainType::MRT) ? Color{255, 80, 80, 255} : (qd.type == TrainType::LRT ? Color{255, 195, 40, 255} : Color{80, 225, 110, 255});

            DrawCircle(tx + ScaleI(12.0f), curY + i * fRowH + ScaleI(15.0f), ScaleF(5.0f), tc);
            string rowTxt = qd.trainId + ": " + qd.startStation->id + " -> " + qd.endStation->id;
            DrawTextBold(rowTxt.c_str(), tx + ScaleI(24.0f), curY + i * fRowH + ScaleI(7.0f), ScaleI(13.0f), tc);

            // Nút xóa riêng lẻ [X]
            Rectangle btnDel = {(float)(tx + tw - ScaleI(44.0f)), (float)(curY + i * fRowH + ScaleI(4.0f)), ScaleF(22.0f), ScaleF(22.0f)};
            bool hovDel = CheckCollisionPointRec(mouse, btnDel);
            DrawRectangleRec(btnDel, hovDel ? Color{140, 30, 30, 240} : Color{80, 25, 25, 220});
            DrawRectangleLinesEx(btnDel, 1.0f, Color{240, 70, 70, 220});
            DrawTextBold("x", (int)btnDel.x + ScaleI(6.0f), (int)btnDel.y + ScaleI(3.0f), ScaleI(12.0f), Color{255, 200, 200, 255});
        }

        // Nút cuộn cho bảng tàu lưu
        Rectangle btnSavedUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(fleetListH / 2 - ScaleI(3.0f))};
        Rectangle btnSavedDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + fleetListH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(fleetListH / 2 - ScaleI(3.0f))};
        DrawRectangleRec(btnSavedUp, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnSavedUp, 1.0f, UI_PANEL_BORDER);
        DrawTextBold("^", (int)btnSavedUp.x + ScaleI(6.0f), (int)btnSavedUp.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);
        DrawRectangleRec(btnSavedDown, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnSavedDown, 1.0f, UI_PANEL_BORDER);
        DrawTextBold("v", (int)btnSavedDown.x + ScaleI(6.0f), (int)btnSavedDown.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);
    }

    curY += fleetListH + ScaleI(10.0f);
    DrawLine(px + ScaleI(8.0f), curY, px + pw - ScaleI(8.0f), curY, UI_PANEL_BORDER);
    curY += ScaleI(10.0f);

    // ── ĐỀ MỤC 3: BẢNG THÔNG BÁO TÀU NÀO ĐÃ ĐẾN NƠI ─────────────────────────
    string arrHeader = "3. THONG BAO TAU DA DEN NOI (" + to_string(app->arrivedFleet.size()) + "):";
    DrawTextBold(arrHeader.c_str(), tx, curY, ScaleI(15.0f), UI_TEXT_GOLD);
    curY += ScaleI(20.0f);

    int arrListH = ph - (curY - py) - ScaleI(14.0f);
    Rectangle arrBox = {(float)tx, (float)curY, (float)tw, (float)arrListH};
    DrawRectangleRec(arrBox, Color{12, 18, 32, 220});
    DrawRectangleLinesEx(arrBox, 1.0f, UI_PANEL_BORDER);

    if (app->arrivedFleet.empty()) {
        DrawTextF("Chua co tau nao hoan thanh hanh trinh.", tx + ScaleI(10.0f), curY + ScaleI(18.0f), ScaleI(13.0f), UI_TEXT_SECONDARY);
    } else {
        int aRowH = ScaleI(32.0f);
        int aVis = arrListH / aRowH;
        int scrollArr = clamp(app->scrollArrived, 0, max(0, (int)app->arrivedFleet.size() - aVis));

        for (int i = 0; i < aVis && (scrollArr + i) < (int)app->arrivedFleet.size(); ++i) {
            int idx = scrollArr + i;
            const auto& arr = app->arrivedFleet[idx];

            Rectangle rRow = {(float)tx + 2.0f, (float)(curY + i * aRowH + 1), (float)tw - ScaleF(28.0f), (float)aRowH - 2.0f};
            DrawRectangleRec(rRow, Color{18, 55, 35, 230});
            DrawRectangleLinesEx(rRow, 1.0f, Color{60, 200, 110, 180});

            DrawTextBold("[v]", tx + ScaleI(8.0f), curY + i * aRowH + ScaleI(7.0f), ScaleI(14.0f), Color{100, 255, 160, 255});
            string line = arr.trainId + " da cap ben: " + arr.destName;
            DrawTextBold(line.c_str(), tx + ScaleI(32.0f), curY + i * aRowH + ScaleI(7.0f), ScaleI(13.0f), Color{220, 255, 230, 255});
        }

        // Nút cuộn cho bảng về đích
        Rectangle btnArrUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(arrListH / 2 - ScaleI(3.0f))};
        Rectangle btnArrDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + arrListH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(arrListH / 2 - ScaleI(3.0f))};
        DrawRectangleRec(btnArrUp, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnArrUp, 1.0f, UI_PANEL_BORDER);
        DrawTextBold("^", (int)btnArrUp.x + ScaleI(6.0f), (int)btnArrUp.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);
        DrawRectangleRec(btnArrDown, Color{25, 42, 72, 230}); DrawRectangleLinesEx(btnArrDown, 1.0f, UI_PANEL_BORDER);
        DrawTextBold("v", (int)btnArrDown.x + ScaleI(6.0f), (int)btnArrDown.y + ScaleI(12.0f), ScaleI(13.0f), UI_TEXT_PRIMARY);
    }
}

void UIPanel::drawStatusBar(SimulationApp* app) {
    int bh = ScaleI(46.0f), by = app->screenH - bh;
    DrawRectangle(0, by, app->screenW, bh, UI_PANEL_COLOR);
    DrawLine(0, by, app->screenW, by, UI_PANEL_BORDER);
    DrawCircle(ScaleI(20.0f), by + bh / 2, ScaleF(6.0f), Color{0, 240, 160, 255});
    DrawTextBold(app->statusMsg.c_str(), ScaleI(36.0f), by + ScaleI(13.0f), ScaleI(14.0f), UI_TEXT_PRIMARY);

    string stat = "FPS: " + to_string(GetFPS()) + " | [R] Reset | [SPACE] Xuat phat | [ENTER] Luu";
    DrawTextBold(stat.c_str(), (int)(app->screenW - MeasureTextBold(stat.c_str(), ScaleI(13.0f)).x - ScaleI(20.0f)), by + ScaleI(14.0f), ScaleI(13.0f), UI_TEXT_SECONDARY);
}

bool UIPanel::handleMouseInteraction(SimulationApp* app) {
    Vector2 m = GetMousePosition();
    float wheel = GetMouseWheelMove();

    int pw = app->getPanelWidth();
    int leftPx = ScaleI(14.0f), leftPy = ScaleI(52.0f), leftPh = app->screenH - leftPy - ScaleI(54.0f);
    Rectangle leftRec = {(float)leftPx, (float)leftPy, (float)pw, (float)leftPh};

    int rightPx = app->screenW - pw - ScaleI(14.0f), rightPy = ScaleI(52.0f), rightPh = app->screenH - rightPy - ScaleI(54.0f);
    Rectangle rightRec = {(float)rightPx, (float)rightPy, (float)pw, (float)rightPh};

    const auto& stations = MapDatabase::getStations();

    // ── XỬ LÝ TRÊN BẢNG BÊN TRÁI ─────────────────────────────────────────────
    if (CheckCollisionPointRec(m, leftRec)) {
        int tx = leftPx + ScaleI(14.0f);
        int tw = pw - ScaleI(28.0f);
        int curY = leftPy + ScaleI(46.0f);

        curY += ScaleI(24.0f); // Tiêu đề 1
        float btnGap = ScaleF(8.0f);
        float btnW = ((float)tw - btnGap * 2.0f) / 3.0f;
        float btnH = ScaleF(38.0f);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            TrainType types[] = {TrainType::MRT, TrainType::LRT, TrainType::LRT_TOURIST};
            for (int i = 0; i < 3; ++i) {
                Rectangle r = {(float)tx + i * (btnW + btnGap), (float)curY, btnW, btnH};
                if (CheckCollisionPointRec(m, r)) {
                    app->selectTrainType(types[i]);
                    return true;
                }
            }
        }
        curY += (int)btnH + ScaleI(10.0f);

        curY += ScaleI(18.0f); // Tiêu đề chọn số hiệu
        int maxNum = (app->activeTrainType == TrainType::MRT) ? 4 : ((app->activeTrainType == TrainType::LRT) ? 11 : 4);
        int pillsPerRow = (app->activeTrainType == TrainType::LRT) ? 6 : 4;
        float pillGap = ScaleF(5.0f);
        float pillH = ScaleF(28.0f);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (int n = 1; n <= maxNum; ++n) {
                int row = (n - 1) / pillsPerRow;
                int col = (n - 1) % pillsPerRow;
                int itemsInThisRow = (row == 0 && app->activeTrainType == TrainType::LRT) ? 6 : (app->activeTrainType == TrainType::LRT ? 5 : 4);
                float pW = ((float)tw - pillGap * (float)(itemsInThisRow - 1)) / (float)itemsInThisRow;
                float pX = (float)tx + col * (pW + pillGap);
                float pY = (float)curY + row * (pillH + ScaleF(5.0f));
                Rectangle pr = {pX, pY, pW, pillH};
                if (CheckCollisionPointRec(m, pr)) {
                    app->selectTrainNum(n);
                    return true;
                }
            }
        }
        curY += (app->activeTrainType == TrainType::LRT ? ScaleI(66.0f) : ScaleI(34.0f)) + ScaleI(8.0f);
        curY += ScaleI(20.0f); // Separator + Tiêu đề 2

        curY += ScaleI(20.0f); // Tiêu đề 2 text
        curY += ScaleI(36.0f); // Banner ga đầu

        int listH = ScaleI(115.0f);
        Rectangle origBox = {(float)tx, (float)curY, (float)tw, (float)listH};

        if (wheel != 0 && CheckCollisionPointRec(m, origBox)) {
            app->scrollOrigin = clamp(app->scrollOrigin - (int)wheel, 0, max(0, (int)stations.size() - 4));
            return true;
        }

        int rowH = ScaleI(28.0f);
        int visibleRows = listH / rowH;
        int scrollOrig = clamp(app->scrollOrigin, 0, max(0, (int)stations.size() - visibleRows));

        Rectangle btnOrigUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(listH / 2 - ScaleI(3.0f))};
        Rectangle btnOrigDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + listH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(listH / 2 - ScaleI(3.0f))};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(m, btnOrigUp)) {
                app->scrollOrigin = max(0, app->scrollOrigin - 1);
                return true;
            }
            if (CheckCollisionPointRec(m, btnOrigDown)) {
                app->scrollOrigin = min((int)stations.size() - visibleRows, app->scrollOrigin + 1);
                return true;
            }
            for (int i = 0; i < visibleRows && (scrollOrig + i) < (int)stations.size(); ++i) {
                Rectangle rRow = {(float)tx + 2.0f, (float)(curY + i * rowH + 1), (float)tw - ScaleF(28.0f), (float)rowH - 2.0f};
                if (CheckCollisionPointRec(m, rRow)) {
                    app->setOriginIndex(scrollOrig + i);
                    return true;
                }
            }
        }

        curY += listH + ScaleI(18.0f);
        curY += ScaleI(20.0f); // Tiêu đề 3 text
        curY += ScaleI(36.0f); // Banner ga cuối

        Rectangle destBox = {(float)tx, (float)curY, (float)tw, (float)listH};

        if (wheel != 0 && CheckCollisionPointRec(m, destBox)) {
            app->scrollDest = clamp(app->scrollDest - (int)wheel, 0, max(0, (int)stations.size() - 4));
            return true;
        }

        int scrollDest = clamp(app->scrollDest, 0, max(0, (int)stations.size() - visibleRows));
        Rectangle btnDestUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(listH / 2 - ScaleI(3.0f))};
        Rectangle btnDestDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + listH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(listH / 2 - ScaleI(3.0f))};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(m, btnDestUp)) {
                app->scrollDest = max(0, app->scrollDest - 1);
                return true;
            }
            if (CheckCollisionPointRec(m, btnDestDown)) {
                app->scrollDest = min((int)stations.size() - visibleRows, app->scrollDest + 1);
                return true;
            }
            for (int i = 0; i < visibleRows && (scrollDest + i) < (int)stations.size(); ++i) {
                Rectangle rRow = {(float)tx + 2.0f, (float)(curY + i * rowH + 1), (float)tw - ScaleF(28.0f), (float)rowH - 2.0f};
                if (CheckCollisionPointRec(m, rRow)) {
                    app->setDestIndex(scrollDest + i);
                    return true;
                }
            }
        }

        curY += listH + ScaleI(44.0f);
        Rectangle btnSave = {(float)tx, (float)curY, (float)tw, ScaleF(40.0f)};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(m, btnSave)) {
            app->saveCurrentTrainToFleet();
            return true;
        }

        curY += ScaleI(48.0f);
        Rectangle btnLaunch = {(float)tx, (float)curY, (float)tw, ScaleF(42.0f)};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(m, btnLaunch)) {
            app->launchAllQueuedTrains();
            return true;
        }

        return true;
    }

    // ── XỬ LÝ TRÊN BẢNG BÊN PHẢI ────────────────────────────────────────────
    if (CheckCollisionPointRec(m, rightRec)) {
        int tx = rightPx + ScaleI(14.0f);
        int tw = pw - ScaleI(28.0f);
        int curY = rightPy + ScaleI(46.0f);

        curY += ScaleI(38.0f);
        int blockListH = ScaleI(125.0f);
        Rectangle blockBox = {(float)tx, (float)curY, (float)tw, (float)blockListH};

        if (wheel != 0 && CheckCollisionPointRec(m, blockBox)) {
            app->scrollBlocked = clamp(app->scrollBlocked - (int)wheel, 0, max(0, (int)stations.size() - 4));
            return true;
        }

        int bRowH = ScaleI(30.0f);
        int bVisibleRows = blockListH / bRowH;
        int scrollBlk = clamp(app->scrollBlocked, 0, max(0, (int)stations.size() - bVisibleRows));

        Rectangle btnBlkUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(blockListH / 2 - ScaleI(3.0f))};
        Rectangle btnBlkDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + blockListH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(blockListH / 2 - ScaleI(3.0f))};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(m, btnBlkUp)) {
                app->scrollBlocked = max(0, app->scrollBlocked - 1);
                return true;
            }
            if (CheckCollisionPointRec(m, btnBlkDown)) {
                app->scrollBlocked = min((int)stations.size() - bVisibleRows, app->scrollBlocked + 1);
                return true;
            }
            // Click bất kỳ đâu trên hàng để toggle trạng thái
            for (int i = 0; i < bVisibleRows && (scrollBlk + i) < (int)stations.size(); ++i) {
                int idx = scrollBlk + i;
                Rectangle rRow = {(float)tx + 2.0f, (float)(curY + i * bRowH + 1), (float)tw - ScaleF(28.0f), (float)bRowH - 2.0f};
                if (CheckCollisionPointRec(m, rRow)) {
                    app->toggleBlockedStation(stations[idx].id);
                    return true;
                }
            }
        }

        curY += blockListH + ScaleI(20.0f);
        curY += ScaleI(20.0f); // Tiêu đề tàu đã lưu
        int fleetListH = ScaleI(125.0f);
        Rectangle fleetBox = {(float)tx, (float)curY, (float)tw, (float)fleetListH};

        if (wheel != 0 && CheckCollisionPointRec(m, fleetBox)) {
            app->scrollSaved = clamp(app->scrollSaved - (int)wheel, 0, max(0, (int)app->pendingFleet.size() - 4));
            return true;
        }

        int fRowH = ScaleI(30.0f);
        int fVis = fleetListH / fRowH;
        int scrollSaved = clamp(app->scrollSaved, 0, max(0, (int)app->pendingFleet.size() - fVis));

        Rectangle btnSavedUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(fleetListH / 2 - ScaleI(3.0f))};
        Rectangle btnSavedDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + fleetListH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(fleetListH / 2 - ScaleI(3.0f))};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(m, btnSavedUp)) {
                app->scrollSaved = max(0, app->scrollSaved - 1);
                return true;
            }
            if (CheckCollisionPointRec(m, btnSavedDown)) {
                app->scrollSaved = max(0, min((int)app->pendingFleet.size() - fVis, app->scrollSaved + 1));
                return true;
            }
            for (int i = 0; i < fVis && (scrollSaved + i) < (int)app->pendingFleet.size(); ++i) {
                int idx = scrollSaved + i;
                Rectangle btnDel = {(float)(tx + tw - ScaleI(44.0f)), (float)(curY + i * fRowH + ScaleI(4.0f)), ScaleF(22.0f), ScaleF(22.0f)};
                if (CheckCollisionPointRec(m, btnDel)) {
                    app->removeSavedTrain(idx);
                    return true;
                }
            }
        }

        curY += fleetListH + ScaleI(20.0f);
        curY += ScaleI(20.0f); // Tiêu đề tàu về đích
        int arrListH = rightPh - (curY - rightPy) - ScaleI(14.0f);
        Rectangle arrBox = {(float)tx, (float)curY, (float)tw, (float)arrListH};

        if (wheel != 0 && CheckCollisionPointRec(m, arrBox)) {
            app->scrollArrived = clamp(app->scrollArrived - (int)wheel, 0, max(0, (int)app->arrivedFleet.size() - 4));
            return true;
        }

        int aRowH = ScaleI(32.0f);
        int aVis = arrListH / aRowH;
        Rectangle btnArrUp   = {(float)(tx + tw - ScaleI(24.0f)), (float)curY + ScaleF(2.0f), ScaleF(22.0f), (float)(arrListH / 2 - ScaleI(3.0f))};
        Rectangle btnArrDown = {(float)(tx + tw - ScaleI(24.0f)), (float)(curY + arrListH / 2 + ScaleI(1.0f)), ScaleF(22.0f), (float)(arrListH / 2 - ScaleI(3.0f))};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(m, btnArrUp)) {
                app->scrollArrived = max(0, app->scrollArrived - 1);
                return true;
            }
            if (CheckCollisionPointRec(m, btnArrDown)) {
                app->scrollArrived = max(0, min((int)app->arrivedFleet.size() - aVis, app->scrollArrived + 1));
                return true;
            }
        }

        return true;
    }

    return false;
}

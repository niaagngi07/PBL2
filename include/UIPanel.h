#ifndef UI_PANEL_H
#define UI_PANEL_H

#include "raylib.h"
#include <string>
#include <vector>

using namespace std;

class SimulationApp;

class UIPanel {
public:
    // Vẽ khung panel kính mờ hiện đại kèm thanh tiêu đề
    static void drawModernPanel(int x, int y, int w, int h, const char* title);

    // Bảng bên trái: Chọn loại tàu, ga đầu, ga cuối, nút Save và nút Run
    static void drawLeftPanel(SimulationApp* app);

    // Bảng bên phải: Ga tắc nghẽn/sự cố, danh sách tàu đã save, bảng thông báo tàu đã đến nơi
    static void drawRightPanel(SimulationApp* app);

    // Thanh trạng thái dưới đáy màn hình
    static void drawStatusBar(SimulationApp* app);

    // Xử lý sự kiện click chuột và cuộn chuột trên các panel UI
    static bool handleMouseInteraction(SimulationApp* app);
};

#endif // UI_PANEL_H

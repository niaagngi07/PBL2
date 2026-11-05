#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include <string>
#include <vector>
#include <cmath>
#include "raylib.h"

using namespace std;

// Bảng màu UI Cyberpunk / High-Tech GIS dùng chung cho toàn hệ thống
static const Color UI_BG_DARK         = Color{ 10,  14,  24, 255};
static const Color UI_PANEL_COLOR      = Color{ 15,  22,  38, 240};
static const Color UI_PANEL_BORDER     = Color{ 45,  70, 115, 220};
static const Color UI_PANEL_HEADER     = Color{ 22,  35,  60, 255};
static const Color UI_TEXT_PRIMARY     = Color{235, 242, 255, 255};
static const Color UI_TEXT_SECONDARY   = Color{145, 168, 205, 255};
static const Color UI_TEXT_ACCENT      = Color{ 90, 210, 255, 255};
static const Color UI_TEXT_GOLD        = Color{255, 215,  70, 255};
static const Color UI_HIGHLIGHT_PATH   = Color{  0, 230, 255, 255};

// Hệ số scale UI toàn cục (chuẩn 1.0f tại 1080p, tự động nhân tỷ lệ cho 2.5K/4K)
extern float gUiScale;

inline float ScaleF(float val) { return val * gUiScale; }
inline int   ScaleI(float val) { return (int)round(val * gUiScale); }

// Font toàn cục sắc nét
extern Font gFont;
extern Font gFontBold;
extern bool gFontLoaded;

// Khởi tạo và giải phóng font chất lượng cao
void InitRenderFonts();
void UnloadRenderFonts();

// Vẽ text với font UI sắc nét (có mipmap, không mờ)
void DrawTextF(const char* text, int x, int y, int size, Color color);
void DrawTextBold(const char* text, int x, int y, int size, Color color);

// Đo kích thước text
Vector2 MeasureTextF(const char* text, int size);
Vector2 MeasureTextBold(const char* text, int size);

#endif // RENDER_UTILS_H

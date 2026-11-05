#include "RenderUtils.h"
#include <string>

using namespace std;

float gUiScale = 1.0f;
Font gFont = {};
Font gFontBold = {};
bool gFontLoaded = false;

void InitRenderFonts() {
    // Thử nạp Segoe UI (font UI tiêu chuẩn cực nét của Windows) hoặc Arial
    const char* regularPaths[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/consola.ttf"
    };
    const char* boldPaths[] = {
        "C:/Windows/Fonts/segoeuib.ttf",
        "C:/Windows/Fonts/arialbd.ttf",
        "C:/Windows/Fonts/consolab.ttf"
    };

    // Nạp glyphs ở độ phân giải siêu nét (64px) phục vụ hoàn hảo màn hình 2.5K & 4K
    for (const char* path : regularPaths) {
        if (FileExists(path)) {
            gFont = LoadFontEx(path, 64, nullptr, 0);
            if (gFont.texture.id != 0) {
                GenTextureMipmaps(&gFont.texture);
                SetTextureFilter(gFont.texture, TEXTURE_FILTER_TRILINEAR);
                gFontLoaded = true;
                break;
            }
        }
    }

    for (const char* path : boldPaths) {
        if (FileExists(path)) {
            gFontBold = LoadFontEx(path, 64, nullptr, 0);
            if (gFontBold.texture.id != 0) {
                GenTextureMipmaps(&gFontBold.texture);
                SetTextureFilter(gFontBold.texture, TEXTURE_FILTER_TRILINEAR);
                break;
            }
        }
    }
}

void UnloadRenderFonts() {
    if (gFontLoaded) {
        if (gFont.texture.id != 0) UnloadFont(gFont);
        if (gFontBold.texture.id != 0) UnloadFont(gFontBold);
        gFontLoaded = false;
    }
}

void DrawTextF(const char* text, int x, int y, int size, Color color) {
    if (gFontLoaded && gFont.texture.id != 0) {
        DrawTextEx(gFont, text, {(float)x, (float)y}, (float)size, 1.0f, color);
    } else {
        DrawText(text, x, y, size, color);
    }
}

void DrawTextBold(const char* text, int x, int y, int size, Color color) {
    if (gFontLoaded && gFontBold.texture.id != 0) {
        DrawTextEx(gFontBold, text, {(float)x, (float)y}, (float)size, 1.0f, color);
    } else if (gFontLoaded && gFont.texture.id != 0) {
        DrawTextEx(gFont, text, {(float)x, (float)y}, (float)size, 1.0f, color);
    } else {
        DrawText(text, x, y, size, color);
    }
}

Vector2 MeasureTextF(const char* text, int size) {
    if (gFontLoaded && gFont.texture.id != 0) {
        return MeasureTextEx(gFont, text, (float)size, 1.0f);
    }
    return {(float)MeasureText(text, size), (float)size};
}

Vector2 MeasureTextBold(const char* text, int size) {
    if (gFontLoaded && gFontBold.texture.id != 0) {
        return MeasureTextEx(gFontBold, text, (float)size, 1.0f);
    } else if (gFontLoaded && gFont.texture.id != 0) {
        return MeasureTextEx(gFont, text, (float)size, 1.0f);
    }
    return {(float)MeasureText(text, size), (float)size};
}

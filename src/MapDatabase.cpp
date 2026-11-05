#include "MapDatabase.h"

using namespace std;

const vector<StationData>& MapDatabase::getStations() {
    static const vector<StationData> stations = {
        // ── MRT 1 (Tuyến Đỏ Cao Tốc) ──────────────────────────────────────────
        {"MRT1_1",    "MRT1 Tay Bac (Lien Chieu)",          171.0f,  133.0f},
        {"LRT3_B",    "LRT 3 - Giao MRT1 Tay Bac",          333.0f,  267.0f},
        {"LRT1_B",    "LRT 1 - Lien Chieu Nam",             403.0f,  319.0f},
        {"MRT1_2",    "MRT1 Trung Tam (San Bay Tay)",       539.0f,  391.0f},
        {"LRT2_B",    "LRT 2 - Hai Chau Bac",               625.0f,  361.0f},
        {"MRT1_3",    "MRT1 Song Han (Cau Rong)",           703.0f,  375.0f},

        // ── MRT 2 (Tuyến Xanh Dương Cao Tốc Ven Biển) ─────────────────────────
        {"MRT2_1",    "MRT2 Bac (Cang Tien Sa / Son Tra)",  692.0f,   60.0f},
        {"LRT_DL2_1", "LRT DL 2 - Ban Dao Son Tra",         699.0f,  217.0f},
        {"MRT2_2",    "MRT2 Trung Tam (Bo Dong Song Han)",  783.0f,  413.0f},
        {"LRT8_B",    "LRT 8 - My An",                      859.0f,  498.0f},
        {"LRT11_B",   "LRT 11 - Giao MRT2 Nam",             839.0f,  669.0f},
        {"MRT2_3",    "MRT2 Nam (Ngu Hanh Son - Hoi An)",   996.0f, 1085.0f},

        // ── LRT 1 (Xanh lá - Tuyến 1: Bắc Liên Chiểu) ─────────────────────────
        {"LRT1_A",    "LRT 1 - Cuc Bac (Deo Hai Van)",      267.0f,   34.0f},

        // ── LRT 2 (Xanh lá - Tuyến 2: Đông Sân Bay) ───────────────────────────
        {"LRT2_A",    "LRT 2 - San Bay Dong",               644.0f,  437.0f},

        // ── LRT 3 (Xanh lá - Tuyến 3: Tây Bắc) ────────────────────────────────
        {"LRT3_A",    "LRT 3 - Tay Lien Chieu",             180.0f,  394.0f},

        // ── LRT 4 (Xanh lá - Tuyến 4: Bờ Tây Sông Hàn) ───────────────────────
        {"LRT4_A",    "LRT 4 - Bo Tay Song Han",            626.0f,  277.0f},

        // ── LRT 5 (Xanh lá - Tuyến 5: Dọc Sân Bay Tây) ────────────────────────
        {"LRT5_A",    "LRT 5 - Nam San Bay",                576.0f,  516.0f},

        // ── LRT 6 (Xanh lá - Tuyến 6: Tây Nam Cẩm Lệ / Hòa Vang) ──────────────
        {"LRT6_A",    "LRT 6 - Tay Nam Hoa Vang",           340.0f,  822.0f},
        {"LRT6_B",    "LRT 6 - Giao Cam Le",                584.0f,  611.0f},

        // ── LRT 7 (Xanh lá - Tuyến 7: Vịnh Đà Nẵng) ───────────────────────────
        {"LRT7_A",    "LRT 7 - Bo Vinh Da Nang",            398.0f,  191.0f},
        {"LRT7_B",    "LRT 7 - Thanh Khe",                  510.0f,  299.0f},

        // ── LRT 8 (Xanh lá - Tuyến 8: Đông Nam) ───────────────────────────────
        {"LRT8_A",    "LRT 8 - Dong Cam Le",                741.0f,  581.0f},

        // ── LRT 9 (Xanh lá - Tuyến 9: Tây Xa Hòa Vang) ────────────────────────
        {"LRT9_A",    "LRT 9 - Tay Hoa Vang",                63.0f,  562.0f},
        {"LRT9_B",    "LRT 9 - Trung Tam Hoa Vang",         350.0f,  642.0f},

        // ── LRT 10 (Xanh lá - Tuyến 10: Vành Đai Tây) ─────────────────────────
        {"LRT10_A",   "LRT 10 - Nam Lien Chieu",            199.0f,  478.0f},
        {"LRT10_B",   "LRT 10 - Giao MRT1 Tay",             363.0f,  234.0f},

        // ── LRT 11 (Xanh lá - Tuyến 11: Nam Hòa Xuân) ─────────────────────────
        {"LRT11_A",   "LRT 11 - Hoa Xuan",                  650.0f,  720.0f},

        // ── LRT DL 1 (Vàng - Tuyến Du Lịch 1: Bờ Vịnh Nguyễn Tất Thành) ───────
        {"LRT_DL1_1", "LRT DL 1 - Vinh Da Nang Tay",        460.0f,  234.0f},
        {"LRT_DL1_2", "LRT DL 1 - Vinh Da Nang Dong",       626.0f,  277.0f},

        // ── LRT DL 2 (Vàng - Tuyến Du Lịch 2: Bán Đảo & Cầu Sông Hàn) ─────────
        {"LRT_DL2_2", "LRT DL 2 - Cau Thuan Phuoc",         798.0f,  304.0f},

        // ── LRT DL 3 (Vàng - Tuyến Du Lịch 3: Bãi Biển Mỹ Khê) ────────────────
        {"LRT_DL3_1", "LRT DL 3 - Bien Pham Van Dong",      816.0f,  399.0f},
        {"LRT_DL3_2", "LRT DL 3 - Bien My Khe Nam",         910.0f,  633.0f}
    };
    return stations;
}

const vector<TrackData>& MapDatabase::getTracks() {
    static const vector<TrackData> tracks = {
        // ── 1. TRỤC MRT 1 (Trục Đỏ Cao Tốc 80 km/h) ──────────────────────────
        {"MRT1_1", "LRT3_B",  6.2, 80.0},
        {"LRT3_B", "LRT1_B",  3.8, 80.0},
        {"LRT1_B", "MRT1_2",  5.5, 80.0},
        {"MRT1_2", "LRT2_B",  4.1, 80.0},
        {"LRT2_B", "MRT1_3",  3.6, 80.0},

        // ── 2. TRỤC MRT 2 (Trục Xanh Dương Cao Tốc 80 km/h) ───────────────────
        {"MRT2_1",    "LRT_DL2_1",  6.5, 80.0},
        {"LRT_DL2_1", "MRT2_2",     8.2, 80.0},
        {"MRT2_2",    "LRT8_B",     4.3, 80.0},
        {"LRT8_B",    "LRT11_B",    6.9, 80.0},
        {"LRT11_B",   "MRT2_3",    17.5, 80.0},

        // ── 3. LIÊN TUYẾN CẦU RỒNG QUA SÔNG HÀN (Nối MRT 1 sang MRT 2) ───────────
        {"MRT1_3", "MRT2_2", 3.2, 65.0},

        // ── 4. KHU VỰC TÂY BẮC (Liên Chiểu & Vành Đai Tây - 60 km/h) ──────────
        {"LRT1_A",  "LRT1_B",  11.5, 60.0},
        {"LRT3_A",  "LRT3_B",   6.8, 60.0},
        {"LRT10_B", "LRT3_B",   2.8, 60.0},
        {"LRT10_A", "LRT10_B", 10.5, 60.0},

        // ── 5. KHU VỰC HÒA VANG & CẨM LỆ (Hành lang phía Tây & Tây Nam) ────────
        {"LRT9_A",  "LRT10_A",  7.5, 60.0}, 
        {"LRT10_A", "LRT9_B",   8.2, 60.0}, 
        {"LRT9_B",  "LRT6_A",   9.0, 60.0}, 
        {"LRT9_B",  "LRT5_A",  11.0, 60.0}, 
        {"LRT9_B",  "LRT6_B",  10.5, 60.0}, 

        // Các liên kết Cẩm Lệ - Sân Bay - Hòa Xuân - Ngũ Hành Sơn
        {"LRT6_B",  "LRT5_A",   4.5, 60.0},
        {"LRT6_B",  "LRT8_A",   6.8, 60.0},
        {"LRT8_A",  "LRT8_B",   5.5, 60.0},
        {"LRT6_B",  "LRT11_A",  6.0, 60.0},
        {"LRT11_A", "LRT11_B",  8.0, 60.0},

        // ── 6. KHU VỰC TRUNG TÂM VÀ VEN SÔNG HÀN ─────────────────────────────
        {"LRT5_A",  "MRT1_2",   5.2, 60.0},
        {"LRT2_A",  "LRT2_B",   3.5, 60.0},
        {"LRT4_A",  "LRT2_B",   3.4, 60.0},
        {"LRT4_A",  "MRT1_3",   3.6, 60.0},
        {"LRT7_A",  "LRT7_B",   5.8, 60.0},
        {"LRT7_B",  "MRT1_2",   4.2, 60.0},

        // ── 7. TUYẾN LRT DU LỊCH 1 - 3 (Ven biển & sông Hàn - 40 km/h) ─────────
        {"LRT_DL1_1", "LRT_DL1_2", 6.8, 40.0},
        {"LRT_DL1_1", "LRT7_A",    3.2, 40.0},
        {"LRT_DL1_2", "LRT4_A",    1.2, 40.0},

        {"LRT_DL2_1", "LRT_DL2_2", 5.2, 40.0},
        {"LRT_DL2_2", "MRT2_2",    4.3, 40.0},

        {"LRT_DL3_1", "LRT_DL3_2", 9.5, 40.0},
        {"LRT_DL3_1", "MRT2_2",    2.1, 40.0},
        {"LRT_DL3_2", "LRT11_B",   3.6, 40.0}
    };
    return tracks;
}

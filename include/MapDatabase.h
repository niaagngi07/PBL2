#ifndef MAP_DATABASE_H
#define MAP_DATABASE_H

#include <string>
#include <vector>

using namespace std;

// Cấu trúc dữ liệu định nghĩa một nhà ga trong cơ sở dữ liệu
struct StationData {
    string id;      // Mã định danh kỹ thuật (ví dụ: "MRT1_1", "LRT9_A")
    string name;    // Tên đầy đủ của nhà ga
    float  imgX;    // Tọa độ X trên bản đồ quy hoạch gốc (1038x1249 pixel)
    float  imgY;    // Tọa độ Y trên bản đồ quy hoạch gốc (1038x1249 pixel)
};

// Cấu trúc dữ liệu định nghĩa một đoạn ray liên kết giữa 2 ga kề nhau
struct TrackData {
    string srcId;       // Mã ga xuất phát
    string destId;      // Mã ga đích đến
    double distance;    // Chiều dài đoạn đường (km)
    double speedLimit;  // Tốc độ tối đa cho phép (km/h): 80 (MRT), 60 (LRT), 40 (DL)
};

// Lớp quản lý cơ sở dữ liệu mạng lưới đường sắt TP. Đà Nẵng
class MapDatabase {
public:
    // Trích xuất toàn bộ cơ sở dữ liệu 34 nhà ga
    static const vector<StationData>& getStations();

    // Trích xuất toàn bộ cơ sở dữ liệu các đoạn ray kết nối
    static const vector<TrackData>& getTracks();
};

#endif // MAP_DATABASE_H

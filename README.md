# HỆ THỐNG ĐIỀU PHỐI TÀU HỎA DÂN DỤNG TP. ĐÀ NẴNG (PBL2)
> **Đồ án cơ sở 2 (PBL2) - Khoa Công nghệ Thông tin - Trường Đại học Bách Khoa, Đại học Đà Nẵng**  
> **Tác giả:** niaagngi07 (nghialee1904@gmail.com)  
> **Công nghệ:** C++17, Raylib 5.0, CMake, MinGW UCRT64  

---

## 1. Giới thiệu Đề tài
Dự án mô phỏng mạng lưới và điều phối giao thông đường sắt đô thị thông minh theo thời gian thực tại TP. Đà Nẵng. Hệ thống vận hành theo kiến trúc hướng đối tượng kết hợp phân tầng chuẩn công nghiệp (**Layered Architecture / MVC**), tích hợp trực quan hóa đồ họa 2D tốc độ cao bằng thư viện **Raylib**.

### Quy mô mạng lưới:
- **02 Tuyến MRT Cao Tốc (Đỏ):** Vận tốc tối đa $80\text{ km/h}$, cấp ưu tiên 1 (Cao nhất).
- **11 Tuyến LRT Đô Thị (Vàng Cam):** Vận tốc tối đa $60\text{ km/h}$, cấp ưu tiên 2.
- **03 Tuyến LRT Du Lịch (Xanh Lá):** Vận tốc tối đa $40\text{ km/h}$, cấp ưu tiên 3.
- **34 Nhà ga kỹ thuật** và **hơn 50 cung ray liên kết** hai chiều phủ khắp địa bàn thành phố trên nền bản đồ số hóa tỷ lệ chuẩn ($1038 \times 1249$ pixel).

---

## 2. Các Tính năng Nổi bật

1. **Giao diện Điều phối Hai Bảng (Dual-Panel Interface):**
   - **Bảng bên trái (Thiết lập & Lập lịch):** Chọn loại tàu (MRT, LRT, DL) với màu nhận diện và số hiệu tàu, chọn ga xuất phát & ga đích đến từ danh sách cuộn 34 nhà ga, lưu chuyến tàu vào hàng đợi, khởi hành hạm đội đồng loạt.
   - **Bảng bên phải (Giám sát & Quản lý sự cố):** Danh sách 34 nhà ga kèm nút toggle `[BÌNH THƯỜNG]` / `[TẮC NGHẼN]`, danh sách các đoàn tàu đã lưu trong hàng đợi, bảng thông báo tàu hoàn thành cập bến theo thời gian thực.
2. **Hệ thống Đường ray Đôi 2 chiều (Bi-directional Double Track):**
   - Mô hình 2 làn ray song song độc lập, tính toán vector pháp tuyến đơn vị $\hat{n} = (-dy/L, dx/L)$ với độ lệch $w = 4.2\text{ px}$.
   - Tà-vẹt (sleepers) vuông góc định kỳ mỗi $16\text{ px}$ và mũi tên chỉ hướng riêng biệt trên từng làn ray.
   - Tàu di chuyển chuẩn tâm làn bên phải, 2 tàu ngược chiều tránh nhau an toàn 100%.
3. **Thuật toán Định tuyến Ưu tiên (Priority Dijkstra with Priority Tuning):**
   - Vận hành bằng cấu trúc **Min-Heap** (`std::priority_queue`).
   - Tàu MRT được ưu tiên khuyến khích chạy trên trục cao tốc ($\times 0.85$), phạt nặng khi rẽ vào tuyến du lịch hẹp ($\times 2.5$).
   - Tàu du lịch ưu tiên chạy trên các tuyến ven biển và sông Hàn ($\times 0.8$).
4. **Cơ chế Chặn ray Tự động (Mid-Journey Block Signaling):**
   - Không cho phép 2 đoàn tàu cùng chạy trên một đoạn ray cùng chiều tại cùng một thời điểm.
   - Tàu đến sau dừng đỗ tại ga chờ tàu đi trước đến ga tiếp theo mới được xuất phát.
   - Phân cấp ưu tiên tự động: MRT > LRT > LRT DL.
5. **Tự động Tái định tuyến Thời gian thực khi có Sự cố (Dynamic Re-routing):**
   - Khi một nhà ga bị tắc nghẽn, các tàu đang lưu thông tự động tìm đường vòng tránh điểm nghẽn.
   - Nếu mạng lưới bị cô lập hoàn toàn không còn đường vòng, tàu dừng đỗ an toàn tại ga trước đó, **tuyệt đối không đi vào ga bị nghẽn**.

---

## 3. Cấu trúc Thư mục Dự án

```
PBL2/
├── include/                     # Header files (Khai báo lớp và nguyên mẫu hàm)
│   ├── Graph.h                  # Đồ thị mạng lưới & Dijkstra phân cấp ưu tiên
│   ├── MapDatabase.h            # Cơ sở dữ liệu 34 ga và 50+ đoạn ray TP. Đà Nẵng
│   ├── RenderUtils.h            # Font Segoe UI mipmapped & Responsive Scaling
│   ├── SegmentTree.h            # Cây phân đoạn truy vấn cự ly O(log N)
│   ├── SimulationApp.h          # Bộ điều phối trung tâm & State Machine
│   ├── Station.h                # Model đỉnh nhà ga & danh sách ray kề
│   ├── StationView.h            # Trực quan hóa hình học & hiệu ứng ga sự cố
│   ├── Track.h                  # Model cung ray có hướng & trọng số
│   ├── TrackView.h              # Trực quan hóa ray đôi 2 chiều, tà-vẹt, mũi tên
│   ├── Train.h                  # Model đoàn tàu & hàng đợi lộ trình FIFO
│   ├── TrainView.h              # Hoạt ảnh tàu chạy làn phải, vệt sáng, nhường ray
│   └── UIPanel.h                # Bảng điều khiển Trái & Phải hiện đại
├── src/                         # Source files (Hiện thực logic mã nguồn)
│   ├── Graph.cpp
│   ├── main.cpp
│   ├── MapDatabase.cpp
│   ├── RenderUtils.cpp
│   ├── SegmentTree.cpp
│   ├── SimulationApp.cpp
│   ├── Station.cpp
│   ├── StationView.cpp
│   ├── Track.cpp
│   ├── TrackView.cpp
│   ├── Train.cpp
│   ├── TrainView.cpp
│   └── UIPanel.cpp
├── CMakeLists.txt               # Kịch bản dựng dự án tự động bằng CMake
├── DaNang_Map_Clean.png         # Bản đồ vệ tinh TP. Đà Nẵng độ phân giải 1038x1249
├── Screenshot 2026-09-24 170712.png # Bản đồ vệ tinh nền ứng dụng
├── PBL2_BaoCao_DieuPhoiTauHoa_DaNang.pdf # Báo cáo PDF chi tiết toàn diện
├── TONG_HOP_CHUC_NANG_VA_CAU_TRUC_MODULES.md # Báo cáo kỹ thuật chi tiết
├── .gitignore                   # Cấu hình bỏ qua file nhị phân & file tạm
└── README.md                    # Tài liệu giới thiệu dự án
```

---

## 4. Hướng dẫn Biên dịch & Chạy Ứng dụng

### Yêu cầu môi trường:
- Hệ điều hành: Windows 10/11 (64-bit)
- Trình biên dịch: MinGW-w64 UCRT64 (GCC hỗ trợ C++17)
- CMake: Phiên bản 3.10 trở lên
- Thư viện Raylib 5.0 (được cấu hình tự động liên kết trong CMakeLists.txt)

### Các bước biên dịch:
```powershell
# 1. Clone repository về máy
git clone https://github.com/niaagngi07/PBL2.git
cd PBL2

# 2. Tạo thư mục build và cấu hình CMake
cmake -B build -G "MinGW Makefiles"

# 3. Biên dịch dự án song song
cmake --build build --parallel 4

# 4. Chạy chương trình mô phỏng
./build/TrainDispatchSystem.exe
```

---

## 5. Bảng Phím tắt Thao tác Nhanh

| Phím tắt / Thao tác | Chức năng điều phối tương ứng |
| :--- | :--- |
| **`Chuột trái (Click)`** | Chọn loại tàu, số hiệu, ga đầu, ga cuối, bật/tắt sự cố, xuất phát hạm đội |
| **`Con lăn chuột`** | Cuộn mượt danh sách ga đầu, ga cuối, ga sự cố, danh sách lưu và về đích |
| **`[1]` / `[2]` / `[3]`** | Đổi nhanh loại tàu: `[1]` MRT Cao tốc, `[2]` LRT Đô thị, `[3]` LRT Du lịch |
| **`[ENTER]` / `[L]`** | Lưu cấu hình đoàn tàu hiện tại vào danh sách hàng đợi (Saved Fleet) |
| **`[SPACE]` / `[X]`** | **Xuất phát đồng loạt** tất cả tàu trong danh sách chờ theo thứ tự ưu tiên |
| **`[C]`** | Xóa sạch danh sách hàng đợi các tàu đang chờ xuất phát |
| **`[R]`** | **Reset toàn hệ thống:** Dọn dẹp tàu đang chạy, khôi phục trạng thái bình thường |
| **`[M]`** | Bật / Tắt lớp ảnh nền bản đồ vệ tinh TP. Đà Nẵng |
| **`[F11]` / `[F]`** | Bật / Tắt chế độ hiển thị toàn màn hình (Fullscreen Responsive) |

---

## 6. Tài liệu Báo cáo Kỹ thuật
Vui lòng tham khảo tập tin [PBL2_BaoCao_DieuPhoiTauHoa_DaNang.pdf](PBL2_BaoCao_DieuPhoiTauHoa_DaNang.pdf) để xem báo cáo trình bày chi tiết về cấu trúc dữ liệu, giải thuật toán học và các biểu đồ kiến trúc hệ thống.

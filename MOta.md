# TỔNG QUAN ĐỒ ÁN: TỐI ƯU HÓA XẾP HÀNG CONTAINER 3D (NHÓM 14)

Dự án này giải quyết bài toán **3D Container Loading Problem (3D-CLP)**: Sắp xếp một danh sách các kiện hàng có kích thước khác nhau vào một hoặc nhiều thùng chứa (container) sao cho tối ưu hóa không gian (Fill Rate) và giá trị hàng hóa (Total Value).

---

## 📂 1. CẤU TRÚC THƯ MỤC CHI TIẾT

### 🛠 Thư mục `src/` (Thuật toán lõi)
Chứa mã nguồn C++ xử lý logic chính của bài toán.
- **`models.h`**: Định nghĩa các đối tượng `Item` (kiện hàng), `Container` (thùng chứa), và các cấu trúc hỗ trợ như `Point3D`, `Space`. Hỗ trợ xoay kiện hàng trong 6 hướng không gian 3D.
- **`dataset_loader.cpp/h`**: Module chuyên dụng để đọc dữ liệu từ file `.txt`. Có khả năng tự động phân tích Header để lấy thông số container.
- **`knapsack_algorithms.cpp/h`**: Các thuật toán lựa chọn kiện hàng tối ưu dựa trên khối lượng và giá trị hàng hóa:
    - **DP (Dynamic Programming)**: Tìm lời giải tối ưu tuyệt đối về giá trị.
    - **Greedy**: Lựa chọn nhanh dựa trên tỷ trọng Giá trị/Khối lượng.
    - **Branch and Bound**: Nhánh cận để tìm lời giải tối ưu cho bộ dữ liệu vừa và nhỏ.
- **`packing_algorithms.cpp/h`**: Các chiến lược hình học để đặt kiện hàng vào không gian 3D:
    - **First Fit (FF)** & **Best Fit (BF)**: Tìm vị trí trống đầu tiên hoặc tốt nhất.
    - **FFD & BFD**: Sắp xếp vật phẩm giảm dần trước khi xếp để tăng hiệu quả lấp đầy.
    - **Extreme Points (EP)**: Sử dụng các điểm cực trị để quản lý không gian trống, cho phép lấp đầy các khe hở nhỏ.
- **`meta_heuristics.cpp/h`**: Các thuật toán tìm kiếm meta-heuristic nâng cao:
    - **Genetic Algorithm (GA)**: Tiến hóa quần thể thứ tự xếp hàng để tìm kết quả tốt nhất.
    - **Simulated Annealing (SA)**: Luyện kim mô phỏng để tránh rơi vào tối ưu cục bộ.
- **`main.cpp`**: Giao diện dòng lệnh chính, cho phép người dùng chạy demo chi tiết từng bước.

### 📜 Thư mục `scripts/` (Công cụ phân tích & Đánh giá)
Chứa các script tự động hóa toàn bộ quy trình nghiên cứu.
- **`generate_data.cpp`**: Tự động sinh dữ liệu test ngẫu nhiên (50, 100, 500 items) và tự động tạo thư mục `data/`.
- **`benchmark.cpp`**: Hệ thống đo lường hiệu năng thực tế. Chạy tất cả các tổ hợp thuật toán trên dữ liệu thật để lấy số liệu về: **Tỉ lệ lấp đầy (%)**, **Thời gian (ms)**, và **Tổng giá trị ($)**.
- **`plot_benchmark.cpp`**: Tích hợp với Python để vẽ biểu đồ so sánh từ kết quả Benchmark.
- **`parse_*.cpp`**: Các parser hỗ trợ các bộ dữ liệu chuẩn quốc tế (OR-Library, BED-BPP, ESICUP).

### 📊 Thư mục `data/` & `results/`
- **`data/`**: Chứa các file đầu vào `.txt` và file kết quả tọa độ `output_result.txt`.
- **`results/`**: Chứa file tổng hợp `benchmark_results.csv` và thư mục `plots/` chứa các biểu đồ đồ thị báo cáo.

---

## 🚀 2. QUY TRÌNH VẬN HÀNH (WORKFLOW)

### Bước 1: Chuẩn bị dữ liệu
```bash
g++ scripts/generate_data.cpp -o generate_data.exe
./generate_data.exe
```

### Bước 2: Đánh giá thuật toán (Benchmark)
Biên dịch tất cả mã nguồn thuật toán kết hợp với script đo lường:
```bash
g++ scripts/benchmark.cpp src/*.cpp -Isrc -o benchmark.exe
./benchmark.exe
```

### Bước 3: Vẽ biểu đồ báo cáo
```bash
g++ scripts/plot_benchmark.cpp -o plot_benchmark.exe
./plot_benchmark.exe
```

### Bước 4: Chạy Demo chi tiết
```bash
g++ src/*.cpp -Isrc -o main.exe
./main.exe
```

---

## 📈 3. CÁC CẢI TIẾN QUAN TRỌNG (V2.0)

1. **Tối ưu hóa hình học**: Thuật toán Packing được cải tiến bước nhảy và logic bỏ qua vùng overlap, giúp xử lý 500 items trong vài mil giây (nhanh gấp 100 lần bản cũ).
2. **Benchmark thực tế**: Loại bỏ logic mô phỏng số liệu giả, thay bằng việc gọi trực tiếp các hàm thuật toán từ `src/`.
3. **Tự động hóa môi trường**: Hệ thống tự động kiểm tra và tạo các thư mục `data/`, `results/`, `plots/` nếu chúng chưa tồn tại.
4. **Vẽ biểu đồ chuyên nghiệp**: Sử dụng `Seaborn` và `Matplotlib` (Python) để xuất ra các đồ thị chất lượng cao cho báo cáo khoa học.

---
*Dự án được phát triển bởi Nhóm 14 - Đồ án Phân tích và Thiết kế Thuật toán.*

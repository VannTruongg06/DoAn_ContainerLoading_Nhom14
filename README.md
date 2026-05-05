# 📦 TỐI ƯU HÓA XẾP HÀNG CONTAINER 3D (NHÓM 14)

Dự án giải quyết bài toán **3D Container Loading Problem (3D-CLP)**: Sắp xếp tối ưu các kiện hàng vào thùng chứa sao cho tận dụng tối đa không gian (Fill Rate) và giá trị (Total Value). Hệ thống kết hợp sức mạnh tính toán của **C++** và khả năng hiển thị trực quan của **Three.js**.

---

## ✨ Tính năng nổi bật

- **Quy trình 2 giai đoạn**: 
    1. **Knapsack Phase**: Lựa chọn hàng hóa tối ưu dựa trên trọng tải và giá trị.
    2. **Packing Phase**: Sắp xếp hình học 3D chính xác.
- **Xử lý hình học nâng cao**: 
    - Hỗ trợ **xoay kiện hàng 6 hướng (6-axis rotation)**.
    - Thuật toán **Extreme Points (EP)** giúp quản lý không gian trống linh hoạt.
- **Thuật toán đa dạng**: Hỗ trợ từ Greedy, DP đến các giải thuật Meta-heuristic (Di truyền - GA, Luyện kim - SA).
- **Hệ thống Benchmark**: Tự động đo lường hiệu năng, thời gian thực thi và tỉ lệ lấp đầy trên hàng chục bộ dữ liệu cùng lúc.
- **Mô phỏng 3D trực quan**: Giao diện Web hiện đại, cho phép xoay, thu phóng và kiểm tra va chạm vật lý.

---

## 📂 Cấu trúc dự án

```text
├── src/                # Mã nguồn C++ (Thuật toán lõi)
├── ui/                 # Giao diện Web Mô phỏng 3D (Three.js)
├── scripts/            # Công cụ hỗ trợ (Benchmark, Data gen)
├── data/               
│   ├── academic/       # Bộ dữ liệu chuẩn quốc tế (BED-BPP, ESICUP)
│   ├── custom_tests/   # Các kịch bản test đặc biệt (Perfect fit, Overweight...)
│   ├── demo_backup/    # CÁC KẾT QUẢ ĐÃ CHẠY SẴN (Dùng cho demo nhanh)
│   └── random/         # Dữ liệu ngẫu nhiên từ 50-1000 items
└── results/            # Kết quả benchmark và biểu đồ phân tích
```

---

## 🛠 Hướng dẫn vận hành

### 1. Biên dịch & Chuẩn bị
```powershell
.\build.bat        # Biên dịch hệ thống
.\setup_data.bat   # Chuẩn bị dữ liệu mẫu
```

### 2. Chạy thuật toán (Manual)
Mở `main.exe` và nhập các lựa chọn theo Menu:
- **Knapsack**: 1. DP | 2. Greedy | 3. Branch & Bound
- **Packing**: 1. FF | 2. BF | 3. FFD | 4. BFD | 5. EP | 6. GA | 7. SA

### 3. Xem mô phỏng 3D
Mở `ui/index.html` bằng **Live Server**, nhấn **"Tải file JSON"** và chọn file kết quả.

---

## 📊 Kịch bản Demo & Kết quả sẵn có (`data/demo_backup/`)

Để đảm bảo buổi thuyết trình diễn ra trôi chảy, nhóm đã chuẩn bị sẵn các kết quả tối ưu. Bạn có thể tải trực tiếp các file này vào giao diện 3D:

| Tên file kết quả | File đầu vào gốc | Thuật toán sử dụng | Ý nghĩa Demo |
| :--- | :--- | :--- | :--- |
| **academic_best.json** | ESICUP (579 items) | **DP + EP** | Hiệu suất cao trên dữ liệu thực tế quốc tế. |
| **perfect_fit.json** | 8 khối lập phương | **DP + EP** | Chứng minh độ chính xác hình học 100%. |
| **ga_smart.json** | 100 items ngẫu nhiên | **Greedy + GA** | Khả năng tối ưu hóa của Meta-heuristic (Di truyền). |
| **heavy_load.json** | Overweight test | **DP + EP** | Kiểm soát tải trọng (An toàn vận chuyển). |
| **speed_1000.json** | 1000 items ngẫu nhiên | **Greedy + FFD** | Tốc độ xử lý dữ liệu lớn của C++ (< 30ms). |

---

## 📈 Kiểm thử diện rộng (Benchmark)

Để lấy số liệu cho báo cáo (Độ phức tạp, Trade-off):
1. **Smart Benchmark (Khuyến nghị)**: Chạy `.\smart_benchmark.exe`. Đây là hệ thống phân tầng giúp chạy 21 tổ hợp thuật toán trên dữ liệu nhỏ và các thuật toán nhanh trên dữ liệu lớn, tránh treo máy.
2. **Standard Benchmark**: Chạy `.\benchmark.exe` để quét toàn bộ dataset (lưu ý có thể chậm với dữ liệu cực lớn).
3. Kết quả lưu tại `results/smart_benchmark.csv` hoặc `results/benchmark_results.csv`.

---
**Nhóm 14 - Đồ án Phân tích và Thiết kế Thuật toán**

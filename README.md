# 📦 TỐI ƯU HÓA XẾP HÀNG CONTAINER 3D (NHÓM 14)

Dự án giải quyết bài toán **3D Container Loading Problem (3D-CLP)**: Sắp xếp tối ưu các kiện hàng vào thùng chứa sao cho tận dụng tối đa không gian (Fill Rate) và giá trị (Total Value). Kết hợp sức mạnh tính toán của **C++** và khả năng hiển thị trực quan của **Three.js**.

---

## ✨ Tính năng nổi bật

- **Thuật toán đa dạng**: Hỗ trợ từ thuật toán tham lam (Greedy), quy hoạch động (DP) đến các giải thuật Meta-heuristic (Di truyền - GA, Luyện kim - SA).
- **Mô phỏng 3D trực quan**: Giao diện Web hiện đại cho phép xoay, thu phóng và quan sát chi tiết từng kiện hàng trong không gian 3D.
- **Xử lý hình học chính xác**: Hỗ trợ xoay kiện hàng 6 hướng (6-axis rotation) và kiểm tra va chạm vật lý.
- **Dữ liệu thực tế**: Tích hợp các bộ dữ liệu chuẩn quốc tế như **BED-BPP**, **OR-Library** và **ESICUP**.

---

## 📂 Cấu trúc dự án

```text
├── src/                # Mã nguồn C++ (Thuật toán lõi)
│   ├── models.h        # Định nghĩa Item, Container, Space
│   ├── packing_algorithms.cpp # Chiến lược hình học (FF, BF, EP...)
│   ├── knapsack_algorithms.cpp # Lọc hàng tối ưu (DP, Greedy, B&B)
│   └── meta_heuristics.cpp    # Giải thuật GA, SA
├── ui/                 # Giao diện Web Mô phỏng 3D
│   ├── index.html      # Giao diện chính (TailwindCSS)
│   └── script.js       # Logic render 3D (Three.js)
├── scripts/            # Công cụ hỗ trợ (Data gen, Benchmark, Parsers)
├── data/               # Dữ liệu đầu vào (.txt) và đầu ra (.json)
├── results/            # Kết quả phân tích và biểu đồ
└── build.bat           # File tự động biên dịch
```

---

## 🛠 Hướng dẫn cài đặt & Chạy

### 1. Yêu cầu môi trường
- **C++**: Trình biên dịch `g++` (hỗ trợ C++11 trở lên).
- **Web**: Trình duyệt hiện đại (Chrome, Edge, Firefox). Nên có extension **Live Server** trên VS Code.

### 2. Biên dịch và Chuẩn bị dữ liệu
Chạy các file script tự động sau (trên Windows):
```powershell
.\build.bat        # Biên dịch toàn bộ file C++ thành .exe
.\setup_data.bat   # Sinh dữ liệu mẫu và parse dữ liệu thực tế
```

### 3. Chạy thuật toán C++
Mở `main.exe`, nhập đường dẫn file dữ liệu và chọn thuật toán:
- Ví dụ: `data/random/input_100_items.txt`
- Chương trình sẽ xuất kết quả ra `data/output_3d.json`.

### 4. Xem mô phỏng 3D
- Mở `ui/index.html` bằng **Live Server**.
- Nhấn **"Tải file kết quả (JSON)"** và chọn file `data/output_3d.json` vừa tạo.

---

## 🧠 Danh sách thuật toán

| Nhóm | Thuật toán | Đặc điểm |
| :--- | :--- | :--- |
| **Knapsack** | DP, Greedy, Branch & Bound | Lọc hàng dựa trên giá trị và tải trọng. |
| **Packing** | First Fit, Best Fit, FFD, BFD | Xếp hàng vào vị trí trống đầu tiên/tốt nhất. |
| **Geometry** | Extreme Points (EP) | Tối ưu hóa các điểm cực trị để lấp đầy khe hở. |
| **Heuristic** | Genetic Algorithm, Simulated Annealing | Tìm kiếm không gian lời giải lớn để tối ưu Fill Rate. |

---

## 📊 Dữ liệu thử nghiệm
Dự án cung cấp sẵn các kịch bản test:
- `data/random/`: 50, 100, 500 kiện hàng ngẫu nhiên.
- `data/scenarios/`: Các trường hợp đặc biệt (Hàng cồng kềnh, hàng siêu nặng).
- `data/academic/`: Dữ liệu chuẩn từ các kho hàng quốc tế.

---
**Nhóm 14 - Đồ án Phân tích và Thiết kế Thuật toán**


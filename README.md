# Đồ Án Container Loading - Nhóm 14

Dự án giải quyết bài toán tối ưu hóa không gian xếp hàng vào Container 3D sử dụng các thuật toán: Quy hoạch động (DP), Tham lam (Greedy), Nhánh cận (Branch and Bound), Heuristics (First Fit, Best Fit, Extreme Points), và Meta-heuristics (Genetic Algorithm, Simulated Annealing).

---

## 📂 Cấu trúc thư mục

- `src/`: Chứa mã nguồn thuật toán lõi **C++** (Dành cho TV2 & TV3).
  - Gồm các file lõi: `models.h`, `dataset_loader.h`, `knapsack_algorithms.cpp`, v.v.
- `scripts/`: Chứa mã nguồn **C++** xử lý Data Pipeline và Benchmarking (Dành cho TV1 & TV4).
  - Tự động hóa sinh dữ liệu, cào dữ liệu, đo lường và vẽ biểu đồ.
- `data/`: Chứa các bộ dữ liệu Test (Dễ, TB, Khó) và các dataset thực tế.
- `results/`: Chứa file `csv` kết quả chạy benchmark và các file ảnh `.png` đồ thị báo cáo.
- `docs/`: Chứa báo cáo Word và slide thuyết trình.

---

## 🚀 Hướng dẫn quy trình làm việc (FULL C++)

### 👨‍💻 Dành cho TV1 (Người làm Dữ liệu)
Biên dịch và chạy các file `.cpp` trong thư mục `scripts/` để sinh dữ liệu:

1. **Tạo dữ liệu ngẫu nhiên (3 mức độ):**
   ```bash
   g++ scripts/generate_data.cpp -o generate_data.exe
   ./generate_data.exe
   ```
   *Kết quả:* Tạo ra 3 file `input_50_items.txt`, `input_100_items.txt`, `input_500_items.txt` trong thư mục `data/`.

2. **Parse các Dataset học thuật & thực tế:**
   ```bash
   g++ scripts/parse_orlib.cpp -o parse_orlib.exe
   ./parse_orlib.exe
   
   g++ scripts/parse_bedbpp.cpp -o parse_bedbpp.exe
   ./parse_bedbpp.exe
   ```

### 👨‍💻 Dành cho TV2 & TV3 (Người code Thuật toán C++)
Chúng ta đã có module `DatasetLoader` xử lý phần đọc file.

**Cách dùng trong thuật toán của bạn:**
```cpp
#include "dataset_loader.h"

int main() {
    string path = "data/input_50_items.txt";
    Container cont = DatasetLoader::loadContainer(path);
    vector<Item> items = DatasetLoader::loadItems(path);
}
```

**Biên dịch toàn bộ dự án cốt lõi:**
```bash
g++ src/*.cpp -Isrc -o main.exe
./main.exe
```

### 👨‍💻 Dành cho TV4 (Người chạy Benchmark & Làm báo cáo)

1. **Chạy Đo lường (Đánh giá Fill Rate %, Time ms, Total Value $):**
   ```bash
   g++ scripts/benchmark.cpp -o benchmark.exe
   ./benchmark.exe
   ```
   *Kết quả:* Sẽ xuất ra file `results/benchmark_results.csv`.

2. **Vẽ Biểu đồ đồ thị:**
   *(Ghi chú: Lệnh C++ này sẽ gọi hệ thống vẽ biểu đồ)*
   ```bash
   g++ scripts/plot_benchmark.cpp -o plot_benchmark.exe
   ./plot_benchmark.exe
   ```
   *Kết quả:* Các ảnh biểu đồ chuyên nghiệp sẽ được xuất ra ở thư mục `results/plots/`.

---

## 🔗 Quản lý Git (Nhánh tính năng)
Để đẩy toàn bộ bộ Data và Script này lên kho lưu trữ chung, hãy chạy các lệnh sau trong Terminal:

```bash
git checkout -b feature/data-test
git add data/ scripts/ results/ README.md src/
git commit -m "Hoan thien Data Pipeline Full C++ theo yeu cau"
git push -u origin feature/data-test
```

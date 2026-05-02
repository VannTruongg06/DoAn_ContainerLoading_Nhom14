#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

using namespace std;

// Cấu trúc tạm để sinh dữ liệu
struct GeneratedItem {
    int id;
    int w, h, d;
    int weight;
    int value;
};

// Hàm ghi file theo chuẩn mới
void writeDataset(const string& filename, int c_w, int c_h, int c_d, int maxWeight, const vector<GeneratedItem>& items) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Loi tao file: " << filename << endl;
        return;
    }

    file << "# Container_Width Container_Height Container_Depth Max_Weight\n";
    file << c_w << " " << c_h << " " << c_d << " " << maxWeight << "\n";
    file << "# ID Width Height Depth Weight Value\n";

    for (const auto& item : items) {
        file << item.id << " " << item.w << " " << item.h << " " 
             << item.d << " " << item.weight << " " << item.value << "\n";
    }

    file.close();
    cout << "  [+] Da tao file: " << filename << "\n";
}

int main() {
    cout << "===============================================\n";
    cout << "      DATA GENERATOR (C++ VERSION)             \n";
    cout << "===============================================\n\n";

    // Tạo các thư mục
    #ifdef _WIN32
        system("if not exist data\\random mkdir data\\random");
        system("if not exist data\\scenarios mkdir data\\scenarios");
        system("if not exist data\\edge_cases mkdir data\\edge_cases");
    #else
        system("mkdir -p data/random data/scenarios data/edge_cases");
    #endif

    random_device rd;
    mt19937 rng(rd());

    int C_W = 100, C_H = 100, C_D = 200, MAX_W = 5000;

    // 1. Data/random (50, 100, 500 items)
    for (int num : {50, 100, 500}) {
        vector<GeneratedItem> items;
        uniform_int_distribution<int> dist_size(5, 50);
        uniform_real_distribution<double> dist_density(0.002, 0.008);
        for (int i = 1; i <= num; ++i) {
            GeneratedItem item;
            item.id = i;
            item.w = dist_size(rng);
            item.h = dist_size(rng);
            item.d = dist_size(rng);
            long long volume = item.w * item.h * item.d;
            item.weight = max(1, (int)(volume * dist_density(rng)));
            item.value = item.weight * (rng() % 10 + 5);
            items.push_back(item);
        }
        writeDataset("data/random/input_" + to_string(num) + "_items.txt", C_W, C_H, C_D, MAX_W, items);
    }

    // 2. Scenarios
    // Uniform (Dễ nhất cho thuật toán): Các kiện hàng có kích thước gần bằng nhau.
    {
        vector<GeneratedItem> items;
        uniform_int_distribution<int> dist_size(15, 20); // Gần bằng nhau
        for (int i = 1; i <= 100; ++i) {
            GeneratedItem item = {i, dist_size(rng), dist_size(rng), dist_size(rng), 15, 100};
            items.push_back(item);
        }
        writeDataset("data/scenarios/uniform.txt", C_W, C_H, C_D, MAX_W, items);
    }

    // Diverse (Thử thách khả năng lấp đầy khe hở): Kích thước lệch nhau nhiều (to - nhỏ).
    {
        vector<GeneratedItem> items;
        uniform_int_distribution<int> dist_small(5, 12);
        uniform_int_distribution<int> dist_large(40, 70);
        for (int i = 1; i <= 100; ++i) {
            bool is_large = (rng() % 5 == 0); // 20% large
            int size_w = is_large ? dist_large(rng) : dist_small(rng);
            int size_h = is_large ? dist_large(rng) : dist_small(rng);
            int size_d = is_large ? dist_large(rng) : dist_small(rng);
            GeneratedItem item = {i, size_w, size_h, size_d, (int)(size_w * size_h * size_d * 0.005) + 1, 100};
            items.push_back(item);
        }
        writeDataset("data/scenarios/diverse.txt", C_W, C_H, C_D, MAX_W, items);
    }

    // Weight-Heavy (Nặng nhưng nhỏ): Hết tải trọng trước khi hết chỗ.
    {
        vector<GeneratedItem> items;
        uniform_int_distribution<int> dist_size(10, 15); // Nhỏ
        for (int i = 1; i <= 80; ++i) {
            GeneratedItem item = {i, dist_size(rng), dist_size(rng), dist_size(rng), 150, 400}; // Nặng (150kg/món)
            items.push_back(item);
        }
        writeDataset("data/scenarios/weight_heavy.txt", C_W, C_H, C_D, MAX_W, items);
    }

    // Voluminous (Cồng kềnh nhưng nhẹ): Hết chỗ trước khi hết tải trọng.
    {
        vector<GeneratedItem> items;
        uniform_int_distribution<int> dist_size(40, 75); // Cồng kềnh
        for (int i = 1; i <= 40; ++i) {
            GeneratedItem item = {i, dist_size(rng), dist_size(rng), dist_size(rng), 5, 20}; // Rất nhẹ (5kg/món)
            items.push_back(item);
        }
        writeDataset("data/scenarios/voluminous.txt", C_W, C_H, C_D, MAX_W, items);
    }

    // 3. Edge Cases
    {
        vector<GeneratedItem> items;
        // Quá khổ
        items.push_back({1, 300, 50, 50, 100, 100}); 
        // Khối lượng âm
        items.push_back({2, 20, 20, 20, -50, 100});
        // Bình thường
        items.push_back({3, 30, 30, 30, 10, 50});
        writeDataset("data/edge_cases/dirty_data.txt", C_W, C_H, C_D, MAX_W, items);
    }

    cout << "=> HOAN THANH! Cac file da nam trong thu muc data/\n";
    cout << "===============================================\n";

    return 0;
}

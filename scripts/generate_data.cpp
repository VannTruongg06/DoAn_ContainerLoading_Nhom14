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

// Hàm sinh 1 kiện hàng
GeneratedItem generateItem(int id, mt19937& rng) {
    GeneratedItem item;
    item.id = id;

    // Kích thước (5 - 50 cm) luôn nhỏ hơn thùng (100cm)
    uniform_int_distribution<int> dist_size(5, 50);
    item.w = dist_size(rng);
    item.h = dist_size(rng);
    item.d = dist_size(rng);

    // Tính thể tích & khối lượng
    long long volume = item.w * item.h * item.d;
    uniform_real_distribution<double> dist_density(0.002, 0.008);
    int weight = max(1, (int)(volume * dist_density(rng)));
    item.weight = min(weight, 80); // Tối đa 80kg/kiện

    // Tính giá trị (Có tỉ lệ premium để Knapsack phân loại hàng xịn)
    int base_value = item.weight * (rng() % 18 + 8); // x8 đến x25
    int premium_factors[] = {1, 1, 2, 3}; // 50% cơ hội nhân giá trị
    int premium = premium_factors[rng() % 4];
    item.value = max(10, base_value * premium);

    return item;
}

// Hàm ghi file
void writeDataset(const string& filename, const string& label, int numItems, mt19937& rng) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Loi tao file: " << filename << endl;
        return;
    }

    // Ghi Header
    file << "# ContainerLoading Dataset - " << label << "\n";
    file << "# Container: 100x100x100 cm  MaxWeight: 500\n";
    file << "# Items: " << numItems << "\n";
    file << "# Columns: ID  Width  Height  Depth  Weight  Value\n";
    file << "#-------------------------------------------------------\n";

    // Ghi Data
    long long total_vol = 0;
    int total_weight = 0;
    long long total_value = 0;

    for (int i = 1; i <= numItems; ++i) {
        GeneratedItem item = generateItem(i, rng);
        file << item.id << "\t" << item.w << "\t" << item.h << "\t" 
             << item.d << "\t" << item.weight << "\t" << item.value << "\n";
        
        total_vol += (item.w * item.h * item.d);
        total_weight += item.weight;
        total_value += item.value;
    }

    file.close();
    cout << "  [+] Da tao file: " << filename << " (" << label << ")\n";
    cout << "      Tong the tich: " << total_vol << " cm3, Tong Khoi luong: " 
         << total_weight << " kg, Tong Gia tri: $" << total_value << "\n\n";
}

int main() {
    cout << "===============================================\n";
    cout << "      DATA GENERATOR (C++ VERSION)             \n";
    cout << "===============================================\n\n";

    random_device rd;
    mt19937 rng(rd());

    string dir = "data/";
    
    writeDataset(dir + "input_50_items.txt", "Easy (50 items)", 50, rng);
    writeDataset(dir + "input_100_items.txt", "Medium (100 items)", 100, rng);
    writeDataset(dir + "input_500_items.txt", "Hard (500 items)", 500, rng);

    cout << "=> HOAN THANH! Cac file da nam trong thu muc data/\n";
    cout << "===============================================\n";

    return 0;
}

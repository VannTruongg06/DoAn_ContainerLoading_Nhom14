#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

using namespace std;

// Tạo dữ liệu giả lập cho BED-BPP (Do file JSON 78MB không khả thi lưu trên Git và khó parse bằng C++ chay)
void generateBedBppDemo() {
    random_device rd;
    mt19937 rng(77); // Seed cố định để data không đổi

    string dir = "data/bedbpp/";
    // Lưu ý: C++ cần tạo sẵn thư mục, hãy đảm bảo thư mục data/bedbpp đã tồn tại
    
    for (int i = 1; i <= 3; ++i) {
        string filename = dir + "bedbpp_order_demo_" + to_string(i) + ".txt";
        ofstream file(filename);
        
        if (!file.is_open()) {
            cerr << "[Loi] Khong the tao file: " << filename << " (Ban da tao thu muc data/bedbpp/ chua?)\n";
            continue;
        }

        uniform_int_distribution<int> dist_items(30, 60);
        int num_items = dist_items(rng);

        // Header
        file << "# BED-BPP Converted Order: demo_" << i << "\n";
        file << "# Container: 590x239x235 cm  MaxWeight: 28000 kg\n";
        file << "# Items: " << num_items << "\n";
        file << "# Columns: ID  Width  Height  Depth  Weight  Value\n";
        file << "#-------------------------------------------------------\n";

        uniform_int_distribution<int> dist_wh(20, 120);
        uniform_int_distribution<int> dist_h(20, 80);
        uniform_int_distribution<int> dist_wgt(5, 50);

        for (int j = 1; j <= num_items; ++j) {
            int w = dist_wh(rng);
            int h = dist_h(rng);
            int d = dist_wh(rng);
            int weight = dist_wgt(rng);
            int value = weight * (rng() % 16 + 5);

            file << j << "\t" << w << "\t" << h << "\t" << d << "\t" << weight << "\t" << value << "\n";
        }

        file.close();
        cout << "  -> Da tao BED-BPP Demo Order " << i << " (" << num_items << " items)\n";
    }
}

int main() {
    cout << "===============================================\n";
    cout << "  BED-BPP DATASET PARSER & GENERATOR (C++)     \n";
    cout << "===============================================\n";
    
    cout << "[-] Dang chay che do DEMO (gia lap file BED-BPP JSON)\n";
    generateBedBppDemo();
    
    cout << "\n===============================================\n";
    cout << "  DONE! Du lieu da luu vao thu muc data/bedbpp/\n";
    cout << "===============================================\n";
    
    return 0;
}

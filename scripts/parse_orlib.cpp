#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

using namespace std;

void generateOrlibDemo() {
    random_device rd;
    mt19937 rng(99);

    string dir = "data/orlib/";
    
    // Giả lập 3 bài toán của thpack1
    for (int p = 1; p <= 3; ++p) {
        int c_w = 587, c_h = 220, c_d = 233;
        
        string filename = dir + "thpack1_prob" + to_string(p) + ".txt";
        ofstream file(filename);
        
        if (!file.is_open()) {
            cerr << "[Loi] Khong the tao file: " << filename << " (Ban da tao thu muc data/orlib/ chua?)\n";
            continue;
        }

        uniform_int_distribution<int> dist_items(30, 80);
        int num_items = dist_items(rng);

        file << "# OR-Library Converted (Demo)\n";
        file << "# Container: " << c_w << "x" << c_h << "x" << c_d << " cm  MaxWeight: 1000000 kg\n";
        file << "# Items: " << num_items << "\n";
        file << "# Columns: ID  Width  Height  Depth  Weight  Value\n";
        file << "#-------------------------------------------------------\n";

        uniform_int_distribution<int> dist_size(10, 100);

        for (int i = 1; i <= num_items; ++i) {
            int w = dist_size(rng);
            int h = dist_size(rng);
            int d = dist_size(rng);
            int weight = (w * h * d) / 1000 + 1;
            int value = weight * (rng() % 6 + 10); // 10 đến 15

            file << i << "\t" << w << "\t" << h << "\t" << d << "\t" << weight << "\t" << value << "\n";
        }

        file.close();
        cout << "  -> Da tao DEMO ORLib: thpack1_prob" << p << ".txt (" << num_items << " items)\n";
    }
}

int main() {
    cout << "===============================================\n";
    cout << "  OR-LIBRARY DATASET PARSER (C++ VERSION)      \n";
    cout << "===============================================\n";
    
    cout << "[-] Dang chay che do DEMO (gia lap file thpack)\n";
    generateOrlibDemo();
    
    cout << "\n===============================================\n";
    cout << "  DONE! Du lieu da luu vao thu muc data/orlib/\n";
    cout << "===============================================\n";
    
    return 0;
}

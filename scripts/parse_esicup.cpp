#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

using namespace std;

// Các loại thùng chuẩn ESICUP
struct EsicupContainer {
    int w, h, d, maxW;
    string name;
};

void generateEsicupDemo() {
    random_device rd;
    mt19937 rng(2015);

    vector<EsicupContainer> containers = {
        {235, 239, 590, 24000, "20ft"},
        {235, 239, 1203, 30000, "40ft"}
    };

    string dir = "data/esicup/";
    
    for (int i = 1; i <= 3; ++i) {
        EsicupContainer cont = containers[rng() % 2];
        
        uniform_int_distribution<int> dist_items(200, 800); // ESICUP thường rất nhiều items
        int num_items = dist_items(rng);

        string filename = dir + "esicup_inst_" + to_string(i) + ".txt";
        ofstream file(filename);
        
        if (!file.is_open()) {
            cerr << "[Loi] Khong the tao file: " << filename << " (Ban da tao thu muc data/esicup/ chua?)\n";
            continue;
        }

        // Header
        file << "# ESICUP 2015 Simulated Instance " << i << "\n";
        file << "# Container: " << cont.name << " (" << cont.w << "x" << cont.h << "x" << cont.d << " cm) MaxWeight: " << cont.maxW << " kg\n";
        file << "# Items: " << num_items << "\n";
        file << "# Columns: ID  Width  Height  Depth  Weight  Value\n";
        file << "#-------------------------------------------------------\n";

        uniform_int_distribution<int> dist_size(10, 60);
        uniform_int_distribution<int> dist_wgt(1, 20);

        for (int j = 1; j <= num_items; ++j) {
            int w = dist_size(rng);
            int h = dist_size(rng);
            int d = dist_size(rng);
            int weight = dist_wgt(rng);
            int value = weight * (rng() % 11 + 5);

            file << j << "\t" << w << "\t" << h << "\t" << d << "\t" << weight << "\t" << value << "\n";
        }

        file.close();
        cout << "  -> Da tao ESICUP Instance " << i << " (" << num_items << " items, " << cont.name << ")\n";
    }
}

int main() {
    cout << "===============================================\n";
    cout << "  ESICUP MULTI-CONTAINER DATASET PARSER (C++)  \n";
    cout << "===============================================\n";
    
    cout << "[-] Dang chay che do DEMO (gia lap file ESICUP)\n";
    generateEsicupDemo();
    
    cout << "\n===============================================\n";
    cout << "  DONE! Du lieu da luu vao thu muc data/esicup/\n";
    cout << "===============================================\n";
    
    return 0;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

using namespace std;

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

    string dir = "data/academic/esicup/";
    
    for (int i = 1; i <= 3; ++i) {
        EsicupContainer cont = containers[rng() % 2];
        
        uniform_int_distribution<int> dist_items(200, 800);
        int num_items = dist_items(rng);

        string filename = dir + "esicup_inst_" + to_string(i) + ".txt";
        ofstream file(filename);
        
        if (!file.is_open()) {
            cerr << "[Loi] Khong the tao file: " << filename << "\n";
            continue;
        }

        file << "# Container_Width Container_Height Container_Depth Max_Weight\n";
        file << cont.w << " " << cont.h << " " << cont.d << " " << cont.maxW << "\n";
        file << "# ID Width Height Depth Weight Value\n";

        uniform_int_distribution<int> dist_size(10, 60);
        uniform_int_distribution<int> dist_wgt(1, 20);

        for (int j = 1; j <= num_items; ++j) {
            int w = dist_size(rng);
            int h = dist_size(rng);
            int d = dist_size(rng);
            int weight = dist_wgt(rng);
            int value = weight * (rng() % 11 + 5);

            file << j << " " << w << " " << h << " " << d << " " << weight << " " << value << "\n";
        }

        file.close();
        cout << "  -> Da tao ESICUP Instance " << i << " (" << num_items << " items, " << cont.name << ")\n";
    }
}

int main() {
    cout << "===============================================\n";
    cout << "  ESICUP MULTI-CONTAINER DATASET PARSER (C++)  \n";
    cout << "===============================================\n";
    
    #ifdef _WIN32
        system("if not exist data\\academic\\esicup mkdir data\\academic\\esicup");
    #else
        system("mkdir -p data/academic/esicup");
    #endif

    cout << "[-] Nguon du lieu: ESICUP (Euro Special Interest Group on Cutting and Packing)\n";
    cout << "[-] Link: https://www.esicup.org/\n";
    cout << "[-] Dang chay che do DEMO (Gia lap cac thung Container 20ft/40ft chuan)\n";
    generateEsicupDemo();
    
    cout << "\n===============================================\n";
    cout << "  DONE! Du lieu da luu vao thu muc data/academic/esicup/\n";
    cout << "===============================================\n";
    
    return 0;
}

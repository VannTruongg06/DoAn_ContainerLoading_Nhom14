#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

using namespace std;

void generateOrlibDemo() {
    random_device rd;
    mt19937 rng(99);

    string dir = "data/academic/orlib/";
    
    for (int p = 1; p <= 3; ++p) {
        int c_w = 587, c_h = 220, c_d = 233;
        
        string filename = dir + "thpack1_prob" + to_string(p) + ".txt";
        ofstream file(filename);
        
        if (!file.is_open()) {
            cerr << "[Loi] Khong the tao file: " << filename << "\n";
            continue;
        }

        uniform_int_distribution<int> dist_items(30, 80);
        int num_items = dist_items(rng);

        file << "# Container_Width Container_Height Container_Depth Max_Weight\n";
        file << c_w << " " << c_h << " " << c_d << " 1000000\n";
        file << "# ID Width Height Depth Weight Value\n";

        uniform_int_distribution<int> dist_size(10, 100);

        for (int i = 1; i <= num_items; ++i) {
            int w = dist_size(rng);
            int h = dist_size(rng);
            int d = dist_size(rng);
            int weight = (w * h * d) / 1000 + 1;
            int value = weight * (rng() % 6 + 10); 

            file << i << " " << w << " " << h << " " << d << " " << weight << " " << value << "\n";
        }

        file.close();
        cout << "  -> Da tao DEMO ORLib: thpack1_prob" << p << ".txt (" << num_items << " items)\n";
    }
}

int main() {
    cout << "===============================================\n";
    cout << "  OR-LIBRARY DATASET PARSER (C++ VERSION)      \n";
    cout << "===============================================\n";
    
    #ifdef _WIN32
        system("if not exist data\\academic\\orlib mkdir data\\academic\\orlib");
    #else
        system("mkdir -p data/academic/orlib");
    #endif

    cout << "[-] Nguon du lieu: OR-Library (J.E. Beasley)\n";
    cout << "[-] Link: http://people.brunel.ac.uk/~mastjjb/jbe/orlib/thpackinfo.html\n";
    cout << "[-] Dang chay che do DEMO (Gia lap 3 bai toan mau tu ORLib)\n";
    generateOrlibDemo();
    
    cout << "\n===============================================\n";
    cout << "  DONE! Du lieu da luu vao thu muc data/academic/orlib/\n";
    cout << "===============================================\n";
    
    return 0;
}

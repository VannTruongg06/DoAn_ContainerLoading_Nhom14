#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include "models.h"
#include "knapsack_dp.h"
#include "greedy_packing.h"
#include "genetic_algorithm.h"

using namespace std;

// Hàm đọc dữ liệu từ file
vector<Item> readData(const string& filename) {
    vector<Item> items;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Loi: Khong the mo file " << filename << endl;
        return items;
    }

    string line;
    // Bo qua dong tieu de neu co (comment bang #)
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        stringstream ss(line);
        int id, w, h, d, we, val;
        if (ss >> id >> w >> h >> d >> we >> val) {
            items.emplace_back(id, w, h, d, we, val);
        }
    }
    file.close();
    cout << "Da doc " << items.size() << " mat hang tu file.\n";
    return items;
}

// Hàm in kết quả ra Console và ghi ra file
void exportResult(const Container& container, const string& filename) {
    ofstream file(filename);
    cout << "\n--- KET QUA XEP HANG ---\n";
    cout << left << setw(5) << "ID" << setw(10) << "Toa do (x,y,z)" << setw(15) << "Kich thuoc" << endl;

    for (const auto& item : container.packedItems) {
        if (item.isPacked) {
            string coord = "(" + to_string(item.x) + "," + to_string(item.y) + "," + to_string(item.z) + ")";
            string size = to_string(item.width) + "x" + to_string(item.height) + "x" + to_string(item.depth);
            
            cout << left << setw(5) << item.id << setw(15) << coord << setw(15) << size << endl;
            if (file.is_open()) {
                file << item.id << " " << item.x << " " << item.y << " " << item.z << " " 
                     << item.width << " " << item.height << " " << item.depth << endl;
            }
        }
    }
    if (file.is_open()) file.close();
    cout << "Da xuat ket qua ra file: " << filename << endl;
}

int main() {
    // 1. Khoi tao Container (VD: kich thuoc 100x100x100, tai trong 500)
    Container myContainer(100, 100, 100, 500);

    // 2. Doc du lieu tu file
    vector<Item> items = readData("data/input_50_items.txt");
    if (items.empty()) {
        cout << "Danh sach mat hang trong. Ket thuc.\n";
        return 1;
    }

    // 3. Chay luong thuat toan (Vi du: Loc bang DP -> Xep bang Greedy)
    cout << "\nBat dau chay thuat toan...\n";
    auto start = chrono::high_resolution_clock::now();

    // Buoc A: Loc don hang toi uu theo khoi luong (TV2)
    solveKnapsackDP(items, myContainer.maxWeight);

    // Buoc B: Xep hang vao container (TV2 hoac TV3)
    // O day ta goi thu Greedy truoc
    greedyPacking(myContainer, items);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // 4. Xuat ket qua
    exportResult(myContainer, "data/output_result.txt");
    cout << "\nTong thoi gian thuc hien: " << fixed << setprecision(4) << elapsed.count() << " giay.\n";

    return 0;
}

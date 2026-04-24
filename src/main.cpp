#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include "models.h"
#include "knapsack_algorithms.h"
#include "packing_algorithms.h"
#include "meta_heuristics.h"
#include "dataset_loader.h"

using namespace std;

void exportResult(const Container& container, const string& filename) {
    ofstream file(filename);
    cout << "\n--- KET QUA XEP HANG ---\n";
    cout << left << setw(5) << "ID" << setw(15) << "Toa do (x,y,z)" << setw(15) << "Kich thuoc" << endl;

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
    double fillRate = (double)container.getUsedVolume() / container.getMaxVolume() * 100;
    cout << "-------------------------------------------\n";
    cout << "Ti le lap day: " << fixed << setprecision(2) << fillRate << "%\n";
}

int main() {
    cout << "===============================================\n";
    cout << "       DO AN: TOI UU HOA XEP HANG CONTAINER    \n";
    cout << "===============================================\n";

    string filepath;
    cout << "\nNhap duong dan file dataset.\n(vd: data/input_50_items.txt, data/bedbpp/bedbpp_order_demo_1.txt): ";
    cin >> filepath;

    // Sử dụng C++ Pro: DatasetLoader chuyên nghiệp để đọc Header tự động
    Container myContainer = DatasetLoader::loadContainer(filepath);
    vector<Item> items = DatasetLoader::loadItems(filepath);

    if (items.empty()) { 
        cout << "[!] Khong co du lieu hoac file khong ton tai!\n"; 
        return 1; 
    }

    cout << "\n[+] Da load " << items.size() << " kien hang.\n";
    cout << "[+] Container: " << myContainer.width << "x" << myContainer.height << "x" << myContainer.depth 
         << " (MaxWeight: " << myContainer.maxWeight << ")\n";

    int choiceK, choiceP;
    // Menu 1: Chon thuat toan loc hang
    cout << "\n[1] CHON THUAT TOAN LOC HANG (KNAPSACK):\n";
    cout << "1. Quy hoach dong (DP - Toi uu nhat)\n";
    cout << "2. Tham lam (Greedy - Nhanh nhat)\n";
    cout << "3. Nhanh can (Branch and Bound)\n";
    cout << "Nhap lua chon: "; cin >> choiceK;

    // Menu 2: Chon thuat toan xep hang
    cout << "\n[2] CHON THUAT TOAN XEP HANG (PACKING):\n";
    cout << "1. First Fit (FF)\n";
    cout << "2. Best Fit (BF)\n";
    cout << "3. First Fit Decreasing (FFD)\n";
    cout << "4. Best Fit Decreasing (BFD)\n";
    cout << "5. Extreme Points (EP - Nang cao)\n";
    cout << "6. Giai thuat Di truyen (Genetic Algorithm)\n";
    cout << "7. Luyen kim mo phong (Simulated Annealing)\n";
    cout << "Nhap lua chon: "; cin >> choiceP;

    cout << "\nDang thuc hien thuat toan...\n";
    auto start = chrono::high_resolution_clock::now();

    // Thuc thi Menu 1
    vector<Item> selectedItems;
    if (choiceK == 1) selectedItems = solveKnapsackDP(items, myContainer.maxWeight);
    else if (choiceK == 2) selectedItems = solveKnapsackGreedy(items, myContainer.maxWeight);
    else selectedItems = solveKnapsackBranchAndBound(items, myContainer.maxWeight);

    // Thuc thi Menu 2
    vector<Container> result;
    if (choiceP >= 1 && choiceP <= 5) {
        Strategy s;
        if (choiceP == 1) s = FIRST_FIT;
        else if (choiceP == 2) s = BEST_FIT;
        else if (choiceP == 3) s = FFD;
        else if (choiceP == 4) s = BFD;
        else s = EXTREME_POINT;
        result = solveBasicPacking(selectedItems, myContainer, s);
    } else if (choiceP == 6) {
        result = solveGeneticAlgorithm(selectedItems, myContainer);
    } else {
        result = solveSimulatedAnnealing(selectedItems, myContainer);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    if (!result.empty()) {
        exportResult(result[0], "data/output_result.txt");
    }

    cout << "Thoi gian thuc hien: " << fixed << setprecision(4) << elapsed.count() << " giay.\n";
    cout << "===============================================\n";

    return 0;
}

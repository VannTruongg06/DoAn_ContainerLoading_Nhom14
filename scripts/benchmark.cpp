#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstdlib>

// Import mã nguồn từ src (giả định biên dịch cùng các file .cpp trong src)
#include "../src/models.h"
#include "../src/dataset_loader.h"
#include "../src/knapsack_algorithms.h"
#include "../src/packing_algorithms.h"
#include "../src/meta_heuristics.h"

using namespace std;

int main() {
    cout << "===============================================\n";
    cout << "  C++ REAL BENCHMARK ENGINE (ACTUAL ALGORITHMS)\n";
    cout << "===============================================\n";

    // Tạo thư mục results nếu chưa có
    #ifdef _WIN32
        system("if not exist results mkdir results");
    #else
        system("mkdir -p results");
    #endif

    vector<string> datasets;
    ifstream testFile("data/all_tests.txt");
    if (testFile.is_open()) {
        string line;
        while (getline(testFile, line)) {
            if (!line.empty() && line[0] != '#') {
                datasets.push_back(line);
            }
        }
        testFile.close();
    } else {
        cerr << "[Loi] Khong the mo file data/all_tests.txt\n";
        return 1;
    }

    // Cấu hình các bộ test (Knapsack + Packing)
    struct TestConfig {
        string kName;
        string pName;
        int kType; // 1: DP, 2: Greedy, 3: BnB
        Strategy pStrat;
    };

    vector<TestConfig> configs = {
        {"Greedy", "FF", 2, FIRST_FIT},
        {"Greedy", "BF", 2, BEST_FIT},
        {"Greedy", "FFD", 2, FFD},
        {"Greedy", "BFD", 2, BFD},
        {"DP", "FFD", 1, FFD},
        {"DP", "BFD", 1, BFD},
        {"DP", "EP", 1, EXTREME_POINT},
        {"BnB", "FFD", 3, FFD},
        {"BnB", "EP", 3, EXTREME_POINT}
    };

    ofstream csv("results/benchmark_results.csv");
    if (!csv.is_open()) {
        cerr << "[Loi] Khong the tao file csv trong thu muc results/\n";
        return 1;
    }

    csv << "Dataset,Items,Knapsack,Packing,FillRate(%),Time(ms),TotalValue($)\n";

    for (const auto& dPath : datasets) {
        cout << "\n[+] Dang chay test tren: " << dPath << "...\n";
        
        // Load dữ liệu thật
        Container baseCont = DatasetLoader::loadContainer(dPath);
        vector<Item> allItems = DatasetLoader::loadItems(dPath);
        
        if (allItems.empty()) {
            cout << "  [!] Bo qua: File khong ton tai hoac rong.\n";
            continue;
        }

        int numItems = (int)allItems.size();

        // --- Chạy các tổ hợp Knapsack + Packing ---
        for (const auto& cfg : configs) {
            // Bỏ qua BnB trên dataset lớn (>200 items) vì thời gian chạy quá lâu
            if (cfg.kType == 3 && numItems > 200) {
                cout << "  -> " << cfg.kName << " + " << cfg.pName << "... SKIP (>200 items)\n";
                continue;
            }

            cout << "  -> " << cfg.kName << " + " << cfg.pName << "... ";

            auto start = chrono::high_resolution_clock::now();

            // 1. Chạy Knapsack thật
            vector<Item> selected;
            if (cfg.kType == 1) selected = solveKnapsackDP(allItems, baseCont.maxWeight);
            else if (cfg.kType == 2) selected = solveKnapsackGreedy(allItems, baseCont.maxWeight);
            else selected = solveKnapsackBranchAndBound(allItems, baseCont.maxWeight);

            // 2. Chạy Packing thật
            vector<Container> packed = solveBasicPacking(selected, baseCont, cfg.pStrat);
            
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            // 3. Tính toán kết quả thực tế
            double fillRate = 0;
            long long totalVal = 0;
            if (!packed.empty()) {
                fillRate = (double)packed[0].getUsedVolume() / packed[0].getMaxVolume() * 100;
                for(const auto& item : packed[0].packedItems) totalVal += item.value;
            }

            csv << dPath << "," << numItems << "," << cfg.kName << "," 
                << cfg.pName << "," << fixed << setprecision(2) << fillRate << "," 
                << duration << "," << totalVal << "\n";
                
            cout << "Xong (" << fixed << setprecision(2) << duration << " ms)\n";
        }

        // --- Chạy Meta-heuristics: Genetic Algorithm ---
        {
            cout << "  -> GA (Genetic Algorithm)... ";
            auto start = chrono::high_resolution_clock::now();
            vector<Container> packed = solveGeneticAlgorithm(allItems, baseCont);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            double fillRate = 0;
            long long totalVal = 0;
            if (!packed.empty()) {
                fillRate = (double)packed[0].getUsedVolume() / packed[0].getMaxVolume() * 100;
                for(const auto& item : packed[0].packedItems) totalVal += item.value;
            }
            csv << dPath << "," << numItems << ",GA,GA," << fixed << setprecision(2) 
                << fillRate << "," << duration << "," << totalVal << "\n";
            cout << "Xong (" << fixed << setprecision(2) << duration << " ms)\n";
        }

        // --- Chạy Meta-heuristics: Simulated Annealing ---
        {
            cout << "  -> SA (Simulated Annealing)... ";
            auto start = chrono::high_resolution_clock::now();
            vector<Container> packed = solveSimulatedAnnealing(allItems, baseCont);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            double fillRate = 0;
            long long totalVal = 0;
            if (!packed.empty()) {
                fillRate = (double)packed[0].getUsedVolume() / packed[0].getMaxVolume() * 100;
                for(const auto& item : packed[0].packedItems) totalVal += item.value;
            }
            csv << dPath << "," << numItems << ",SA,SA," << fixed << setprecision(2) 
                << fillRate << "," << duration << "," << totalVal << "\n";
            cout << "Xong (" << fixed << setprecision(2) << duration << " ms)\n";
        }
    }

    csv.close();
    cout << "\n===============================================\n";
    cout << "  HOAN THANH! Ket qua luu tai: results/benchmark_results.csv\n";
    cout << "===============================================\n";

    return 0;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

// Import mã nguồn từ src
#include "../src/models.h"
#include "../src/dataset_loader.h"
#include "../src/knapsack_algorithms.h"
#include "../src/packing_algorithms.h"
#include "../src/meta_heuristics.h"

using namespace std;

struct Result {
    string dataset;
    int items;
    string scenario;
    double fillRate;
    double timeMs;
    long long totalValue;
};

void printResult(const Result& res) {
    cout << "  -> " << left << setw(20) << res.scenario 
         << " | Value: " << setw(8) << res.totalValue 
         << " | Fill: " << fixed << setprecision(2) << setw(6) << res.fillRate << "%"
         << " | Time: " << setw(8) << res.timeMs << " ms" << endl;
}

int main() {
    cout << "===============================================================\n";
    cout << "          BENCHMARK SYSTEM FOR CHAPTER 2 & 3 REPORT            \n";
    cout << "===============================================================\n";

    // Tạo thư mục results nếu chưa có
    #ifdef _WIN32
        system("if not exist results mkdir results");
    #else
        system("mkdir -p results");
    #endif

    ofstream csv("results/benchmark_report_data.csv");
    csv << "Dataset,NumItems,Scenario,TotalValue($),FillRate(%),Time(ms)\n";

    // Danh sách datasets cho Time Complexity Test
    vector<string> timeDatasets = {
        "data/random/input_50_items.txt",
        "data/random/input_100_items.txt",
        "data/random/input_200_items.txt",
        "data/random/input_500_items.txt",
        "data/random/input_1000_items.txt"
    };

    cout << "\n[1] TEST DO PHUC TAP THOI GIAN (TIME COMPLEXITY)\n";
    cout << "Sử dụng cấu hình: Greedy + First Fit (Nhanh nhất)\n";
    cout << "---------------------------------------------------------------\n";

    for (const auto& dPath : timeDatasets) {
        Container baseCont = DatasetLoader::loadContainer(dPath);
        vector<Item> allItems = DatasetLoader::loadItems(dPath);
        if (allItems.empty()) continue;

        auto start = chrono::high_resolution_clock::now();
        
        vector<Item> selected = solveKnapsackGreedy(allItems, baseCont.maxWeight);
        vector<Container> packed = solveBasicPacking(selected, baseCont, FIRST_FIT);
        
        auto end = chrono::high_resolution_clock::now();
        double duration = chrono::duration<double, milli>(end - start).count();

        double fillRate = 0;
        long long totalVal = 0;
        if (!packed.empty()) {
            fillRate = (double)packed[0].getUsedVolume() / packed[0].getMaxVolume() * 100;
            for(const auto& item : packed[0].packedItems) totalVal += item.value;
        }

        Result res = {dPath, (int)allItems.size(), "TimeComplexity", fillRate, duration, totalVal};
        cout << "  n = " << setw(4) << allItems.size() << " | Time: " << fixed << setprecision(2) << duration << " ms" << endl;
        csv << dPath << "," << allItems.size() << "," << "TimeComplexity," << totalVal << "," << fillRate << "," << duration << "\n";
    }

    cout << "\n[2] TEST SO SANH SU DANH DOI (TRADE-OFF)\n";
    cout << "Sử dụng dataset: data/random/input_100_items.txt\n";
    cout << "---------------------------------------------------------------\n";

    string tradeOffDataset = "data/random/input_100_items.txt";
    Container baseCont = DatasetLoader::loadContainer(tradeOffDataset);
    vector<Item> allItems = DatasetLoader::loadItems(tradeOffDataset);

    if (!allItems.empty()) {
        // Lần 1: Nhanh (Greedy + First Fit)
        {
            auto start = chrono::high_resolution_clock::now();
            vector<Item> selected = solveKnapsackGreedy(allItems, baseCont.maxWeight);
            vector<Container> packed = solveBasicPacking(selected, baseCont, FIRST_FIT);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            double fillRate = 0; long long totalVal = 0;
            if (!packed.empty()) {
                fillRate = (double)packed[0].getUsedVolume() / packed[0].getMaxVolume() * 100;
                for(const auto& item : packed[0].packedItems) totalVal += item.value;
            }
            Result res = {tradeOffDataset, 100, "Fast (Greedy+FF)", fillRate, duration, totalVal};
            printResult(res);
            csv << tradeOffDataset << ",100," << res.scenario << "," << totalVal << "," << fillRate << "," << duration << "\n";
        }

        // Lần 2: Tối ưu (BnB + Extreme Points + SA)
        {
            auto start = chrono::high_resolution_clock::now();
            vector<Item> selected = solveKnapsackBranchAndBound(allItems, baseCont.maxWeight);
            vector<Container> packed = solveSimulatedAnnealing(selected, baseCont, EXTREME_POINT);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            double fillRate = 0; long long totalVal = 0;
            if (!packed.empty()) {
                fillRate = (double)packed[0].getUsedVolume() / packed[0].getMaxVolume() * 100;
                for(const auto& item : packed[0].packedItems) totalVal += item.value;
            }
            Result res = {tradeOffDataset, 100, "Optimized (BnB+EP+SA)", fillRate, duration, totalVal};
            printResult(res);
            csv << tradeOffDataset << ",100," << res.scenario << "," << totalVal << "," << fillRate << "," << duration << "\n";
        }

        // Lần 3: Siêu tối ưu (DP + Extreme Points + GA)
        {
            auto start = chrono::high_resolution_clock::now();
            vector<Item> selected = solveKnapsackDP(allItems, baseCont.maxWeight);
            vector<Container> packed = solveGeneticAlgorithm(selected, baseCont, EXTREME_POINT);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            double fillRate = 0; long long totalVal = 0;
            if (!packed.empty()) {
                fillRate = (double)packed[0].getUsedVolume() / packed[0].getMaxVolume() * 100;
                for(const auto& item : packed[0].packedItems) totalVal += item.value;
            }
            Result res = {tradeOffDataset, 100, "HighlyOpt (DP+EP+GA)", fillRate, duration, totalVal};
            printResult(res);
            csv << tradeOffDataset << ",100," << res.scenario << "," << totalVal << "," << fillRate << "," << duration << "\n";
        }
    }

    csv.close();
    cout << "\n===============================================================\n";
    cout << "  HOAN THANH! Ket qua da duoc luu tai results/benchmark_report_data.csv\n";
    cout << "  Cậu có thể dùng file CSV này để vẽ biểu đồ cho Chương 2 & 3.\n";
    cout << "===============================================================\n";

    return 0;
}

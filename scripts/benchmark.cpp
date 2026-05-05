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

namespace {

struct BenchmarkMetrics {
    int containersUsed = 0;
    int packedItems = 0;
    long long usedVolume = 0;
    long long totalVolume = 0;
    long long totalValue = 0;
    double fillRate = 0.0;
};

BenchmarkMetrics summarizePacking(const vector<Container>& packed) {
    BenchmarkMetrics metrics;
    for (const auto& cont : packed) {
        if (cont.packedItems.empty()) continue;
        ++metrics.containersUsed;
        metrics.packedItems += static_cast<int>(cont.packedItems.size());
        metrics.usedVolume += cont.getUsedVolume();
        metrics.totalVolume += cont.getMaxVolume();
        for (const auto& item : cont.packedItems) {
            metrics.totalValue += item.value;
        }
    }
    if (metrics.totalVolume > 0) {
        metrics.fillRate = static_cast<double>(metrics.usedVolume) / metrics.totalVolume * 100.0;
    }
    return metrics;
}

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

void writeCsvRow(ofstream& csv, const string& dPath, int items, const string& kName, const string& pName, const BenchmarkMetrics& m, double time) {
    // Format: Dataset,Items,Knapsack,Packing,ContainersUsed,PackedItems,FillRate(%),Time(ms),TotalValue($),UsedVolume,CapacityVolume
    csv << dPath << "," << items << "," << kName << "," << pName << "," 
        << m.containersUsed << "," << m.packedItems << "," << fixed << setprecision(2) << m.fillRate << "," 
        << time << "," << m.totalValue << "," << m.usedVolume << "," << m.totalVolume << "\n";
}

} // namespace

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

    ofstream csv("results/benchmark_results.csv");
    if (!csv.is_open()) {
        cerr << "[Loi] Khong the tao file csv trong thu muc results/\n";
        return 1;
    }
    csv << "Dataset,Items,Knapsack,Packing,ContainersUsed,PackedItems,FillRate(%),Time(ms),TotalValue($),UsedVolume,CapacityVolume\n";

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
        vector<Container> packed = solveBasicPacking(selected, baseCont, FIRST_FIT, 20); // Step=20 cho benchmark nhanh
        auto end = chrono::high_resolution_clock::now();
        double duration = chrono::duration<double, milli>(end - start).count();

        BenchmarkMetrics metrics = summarizePacking(packed);
        cout << "  n = " << setw(4) << allItems.size() << " | Time: " << fixed << setprecision(2) << duration << " ms" << endl;
        writeCsvRow(csv, dPath, (int)allItems.size(), "Greedy", "FF", metrics, duration);
    }

    cout << "\n[2] TEST SO SANH SU DANH DOI (TRADE-OFF)\n";
    cout << "Sử dụng dataset: data/random/input_100_items.txt\n";
    cout << "---------------------------------------------------------------\n";

    string tradeOffDataset = "data/random/input_100_items.txt";
    Container baseCont = DatasetLoader::loadContainer(tradeOffDataset);
    vector<Item> allItems = DatasetLoader::loadItems(tradeOffDataset);

    if (!allItems.empty()) {
        int n = (int)allItems.size();
        // Lần 1: Nhanh (Greedy + First Fit)
        {
            auto start = chrono::high_resolution_clock::now();
            vector<Item> selected = solveKnapsackGreedy(allItems, baseCont.maxWeight);
            vector<Container> packed = solveBasicPacking(selected, baseCont, FIRST_FIT);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            BenchmarkMetrics metrics = summarizePacking(packed);
            printResult({tradeOffDataset, n, "Fast (Greedy+FF)", metrics.fillRate, duration, metrics.totalValue});
            writeCsvRow(csv, tradeOffDataset, n, "Greedy", "First-Fit", metrics, duration);
        }

        // Lần 2: Tối ưu (BnB + Extreme Points + SA)
        {
            auto start = chrono::high_resolution_clock::now();
            vector<Item> selected = solveKnapsackBranchAndBound(allItems, baseCont.maxWeight);
            vector<Container> packed = solveSimulatedAnnealing(selected, baseCont, EXTREME_POINT);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            BenchmarkMetrics metrics = summarizePacking(packed);
            printResult({tradeOffDataset, n, "Optimized (BnB+EP+SA)", metrics.fillRate, duration, metrics.totalValue});
            writeCsvRow(csv, tradeOffDataset, n, "BnB", "EP-SA", metrics, duration);
        }

        // Lần 3: Siêu tối ưu (DP + Extreme Points + GA)
        {
            auto start = chrono::high_resolution_clock::now();
            vector<Item> selected = solveKnapsackDP(allItems, baseCont.maxWeight);
            vector<Container> packed = solveGeneticAlgorithm(selected, baseCont, EXTREME_POINT);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            BenchmarkMetrics metrics = summarizePacking(packed);
            printResult({tradeOffDataset, n, "HighlyOpt (DP+EP+GA)", metrics.fillRate, duration, metrics.totalValue});
            writeCsvRow(csv, tradeOffDataset, n, "DP", "EP-GA", metrics, duration);
        }
    }

    csv.close();
    cout << "\n===============================================================\n";
    cout << "  HOAN THANH! Ket qua da duoc luu tai results/benchmark_results.csv\n";
    cout << "  Cau co the dung scripts/plot_benchmark.cpp de ve bieu do.\n";
    cout << "===============================================================\n";

    return 0;
}

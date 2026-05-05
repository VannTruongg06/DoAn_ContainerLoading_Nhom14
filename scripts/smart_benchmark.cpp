#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>

// Import ma nguon tu src
#include "../src/models.h"
#include "../src/dataset_loader.h"
#include "../src/knapsack_algorithms.h"
#include "../src/packing_algorithms.h"
#include "../src/meta_heuristics.h"

using namespace std;

struct Metric {
    string tier;
    string knapsack;
    string packing;
    string dataset;
    double fillRate;
    double timeMs;
    long long selectedValue; // Gia tri do Knapsack chon (Stage 1)
    long long packedValue;   // Gia tri thuc te xep duoc (Stage 1+2)
};

void logResult(ofstream& csv, const Metric& m) {
    csv << m.tier << "," << m.knapsack << "," << m.packing << "," << m.dataset << "," 
        << fixed << setprecision(2) << m.fillRate << "," << m.timeMs << "," 
        << m.selectedValue << "," << m.packedValue << "\n";
    cout << "  [" << m.tier << "] " << left << setw(8) << m.knapsack << " + " << setw(10) << m.packing 
         << " | K-Val: " << setw(8) << m.selectedValue << " | P-Val: " << setw(8) << m.packedValue << endl;
}

int main() {
    cout << "===============================================================\n";
    cout << "          SMART TIERED BENCHMARK SYSTEM (V3 - CORRECTED)       \n";
    cout << "===============================================================\n";

    ofstream csv("results/smart_benchmark.csv");
    csv << "Tier,Knapsack,Packing,Dataset,FillRate,TimeMs,SelectedValue,TotalValue\n";

    // 1. TIER 1: SMALL DATA (n=50)
    string smallData = "data/random/input_50_items.txt"; 
    Container cont1 = DatasetLoader::loadContainer(smallData);
    vector<Item> items1 = DatasetLoader::loadItems(smallData);

    cout << "\n[TIER 1] Small Data (n=50) - Running all 21 combinations...\n";
    string kAlgos[] = {"DP", "Greedy", "BnB"};
    string pAlgos[] = {"FF", "BF", "FFD", "BFD", "EP", "GA", "SA"};

    for (int k = 1; k <= 3; ++k) {
        // Tinh gia tri Knapsack truoc (STAGE 1)
        vector<Item> selected;
        if (k == 1) selected = solveKnapsackDP(items1, cont1.maxWeight);
        else if (k == 2) selected = solveKnapsackGreedy(items1, cont1.maxWeight);
        else selected = solveKnapsackBranchAndBound(items1, cont1.maxWeight, 1000000); // Tang len 1tr node cho chinh xac

        long long kVal = 0;
        for(const auto& it : selected) kVal += it.value;

        for (int p = 1; p <= 7; ++p) {
            auto start = chrono::high_resolution_clock::now();
            vector<Container> result;
            if (p <= 5) {
                result = solveBasicPacking(selected, cont1, (Strategy)(p-1), 5);
            } else if (p == 6) result = solveGeneticAlgorithm(selected, cont1, EXTREME_POINT, 20, 10);
            else result = solveSimulatedAnnealing(selected, cont1, EXTREME_POINT, 50);

            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();
            
            double fill = 0; long long pVal = 0;
            if (!result.empty()) {
                fill = (double)result[0].getUsedVolume()/result[0].getMaxVolume()*100;
                for(const auto& item : result[0].packedItems) pVal += item.value;
            }
            logResult(csv, {"Tier1", kAlgos[k-1], pAlgos[p-1], "small_50", fill, duration, kVal, pVal});
        }
    }

    // 2. TIER 2: MEDIUM DATA (n=100)
    string medData = "data/random/input_100_items.txt";
    Container cont2 = DatasetLoader::loadContainer(medData);
    vector<Item> items2 = DatasetLoader::loadItems(medData);

    cout << "\n[TIER 2] Medium Data (n=100)...\n";
    for (int k = 1; k <= 2; ++k) {
        vector<Item> selected = (k==1) ? solveKnapsackDP(items2, cont2.maxWeight) : solveKnapsackGreedy(items2, cont2.maxWeight);
        long long kVal = 0;
        for(const auto& it : selected) kVal += it.value;

        for (int p = 1; p <= 5; ++p) {
            auto start = chrono::high_resolution_clock::now();
            vector<Container> result = solveBasicPacking(selected, cont2, (Strategy)(p-1), 10);
            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();
            double fill = result.empty() ? 0 : (double)result[0].getUsedVolume()/result[0].getMaxVolume()*100;
            long long pVal = 0;
            if(!result.empty()) for(const auto& it : result[0].packedItems) pVal += it.value;
            logResult(csv, {"Tier2", kAlgos[k-1], pAlgos[p-1], "med_100", fill, duration, kVal, pVal});
        }
    }

    // 3. TIER 3: LARGE DATA (ESICUP)
    string largeData = "data/academic/esicup/ku_leuven_esicup_A_AVF_RIR_STK_container_CT40H.txt";
    Container cont3 = DatasetLoader::loadContainer(largeData);
    vector<Item> items3 = DatasetLoader::loadItems(largeData);

    cout << "\n[TIER 3] Large Data (ESICUP)...\n";
    vector<Item> selected = solveKnapsackGreedy(items3, cont3.maxWeight);
    long long kVal = 0; for(const auto& it : selected) kVal += it.value;

    string pFastNames[] = {"FFD", "EP"};
    Strategy sFast[] = {FFD, EXTREME_POINT};
    for (int p = 0; p < 2; ++p) {
        auto start = chrono::high_resolution_clock::now();
        vector<Container> result = solveBasicPacking(selected, cont3, sFast[p], 50);
        auto end = chrono::high_resolution_clock::now();
        double duration = chrono::duration<double, milli>(end - start).count();
        double fill = result.empty() ? 0 : (double)result[0].getUsedVolume()/result[0].getMaxVolume()*100;
        long long pVal = 0;
        if(!result.empty()) for(const auto& it : result[0].packedItems) pVal += it.value;
        logResult(csv, {"Tier3", "Greedy", pFastNames[p], "esicup_A", fill, duration, kVal, pVal});
    }

    csv.close();
    cout << "\nDone! CSV updated with SelectedValue and TotalValue.\n";
    return 0;
}

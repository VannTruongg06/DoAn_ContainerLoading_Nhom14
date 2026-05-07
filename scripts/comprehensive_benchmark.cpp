#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <map>
#include <filesystem>
#include <algorithm>

// Import source code from src/
#include "../src/models.h"
#include "../src/dataset_loader.h"
#include "../src/knapsack_algorithms.h"
#include "../src/packing_algorithms.h"
#include "../src/meta_heuristics.h"

using namespace std;
namespace fs = std::filesystem;

struct Result {
    string datasetName;
    double fillRate;
    double timeMs;
    long long totalValue;
};

struct CombinationResult {
    string kAlgo;
    string pAlgo;
    vector<Result> detailedResults;
    double avgFill = 0;
    double avgTime = 0;
    long long avgValue = 0;
};

int main() {
    cout << "===============================================================\n";
    cout << "      EXPANDED ALGORITHM BENCHMARK (21 COMBOS x 10 DATASETS)   \n";
    cout << "===============================================================\n";

    // Select 10 diverse datasets
    vector<string> datasets = {
        "data/academic/esicup/ku_leuven_esicup_A_AVF_RIR_STK_container_CT40H.txt",
        "data/academic/esicup/ku_leuven_esicup_A_NSA_BUR_STK_container_CT40H.txt",
        "data/academic/esicup/ku_leuven_esicup_B_CHE_GC_PEX_container_CT40H.txt",
        "data/academic/esicup/ku_leuven_esicup_X_FSI_GC_EXP_container_CT40H.txt",
        "data/academic/esicup/ku_leuven_esicup_A_CVP_GC_PEX_container_CT40H.txt",
        "data/academic/bedbpp/bedbpp_order_00105283_euro-pallet.txt",
        "data/academic/bedbpp/bedbpp_order_00106702_rollcontainer.txt",
        "data/academic/bedbpp/bedbpp_order_00101567_euro-pallet.txt",
        "data/academic/bedbpp/bedbpp_order_00100623_rollcontainer.txt",
        "data/academic/bedbpp/bedbpp_order_00109666_euro-pallet.txt"
    };

    string kNames[] = {"DP", "Greedy", "BnB"};
    string pNames[] = {"FF", "BF", "FFD", "BFD", "EP", "GA", "SA"};

    vector<CombinationResult> allCombos;
    for (int k = 0; k < 3; ++k) {
        for (int p = 0; p < 7; ++p) {
            CombinationResult combo;
            combo.kAlgo = kNames[k];
            combo.pAlgo = pNames[p];
            allCombos.push_back(combo);
        }
    }

    for (size_t d = 0; d < datasets.size(); ++d) {
        string dPath = datasets[d];
        string dName = fs::path(dPath).filename().string();
        cout << "\n[" << (d + 1) << "/10] Dataset: " << dName << endl;
        
        Container baseCont = DatasetLoader::loadContainer(dPath);
        vector<Item> allItems = DatasetLoader::loadItems(dPath);

        if (allItems.empty()) {
            cout << "  [!] Skipping empty or missing dataset.\n";
            continue;
        }

        // Cache Knapsack results per dataset
        vector<vector<Item>> kResults(3);
        for (int k = 0; k < 3; ++k) {
            if (k == 0) kResults[k] = solveKnapsackDP(allItems, baseCont.maxWeight);
            else if (k == 1) kResults[k] = solveKnapsackGreedy(allItems, baseCont.maxWeight);
            else kResults[k] = solveKnapsackBranchAndBound(allItems, baseCont.maxWeight, 300000); 
        }

        int comboIdx = 0;
        for (int k = 0; k < 3; ++k) {
            for (int p = 0; p < 7; ++p) {
                auto start = chrono::high_resolution_clock::now();
                
                vector<Container> result;
                if (p < 5) {
                    int step = (allItems.size() > 200) ? 100 : 20;
                    result = solveBasicPacking(kResults[k], baseCont, (Strategy)p, step);
                } else if (p == 5) {
                    result = solveGeneticAlgorithm(kResults[k], baseCont, EXTREME_POINT, 10, 5);
                } else {
                    result = solveSimulatedAnnealing(kResults[k], baseCont, EXTREME_POINT, 20);
                }

                auto end = chrono::high_resolution_clock::now();
                double duration = chrono::duration<double, milli>(end - start).count();

                Result res = {dName, 0.0, duration, 0};
                if (!result.empty()) {
                    res.fillRate = (double)result[0].getUsedVolume() / result[0].getMaxVolume() * 100.0;
                    for (const auto& item : result[0].packedItems) res.totalValue += item.value;
                }

                allCombos[comboIdx].detailedResults.push_back(res);
                comboIdx++;
                if (comboIdx % 7 == 0) cout << "." << flush;
            }
        }
        cout << " Done." << endl;
    }

    // Final Report Export
    ofstream file("results/comprehensive_report_10_datasets.csv");
    file << "Combination,Knapsack,Packing,Dataset,FillRate_%,Time_ms,Value_$\n";

    for (auto& combo : allCombos) {
        double totalFill = 0, totalTime = 0;
        long long totalVal = 0;

        for (const auto& r : combo.detailedResults) {
            file << combo.kAlgo << "+" << combo.pAlgo << "," 
                 << combo.kAlgo << "," 
                 << combo.pAlgo << "," 
                 << r.datasetName << "," 
                 << fixed << setprecision(2) << r.fillRate << "," 
                 << r.timeMs << "," 
                 << r.totalValue << "\n";
            
            totalFill += r.fillRate;
            totalTime += r.timeMs;
            totalVal += r.totalValue;
        }

        if (!combo.detailedResults.empty()) {
            combo.avgFill = totalFill / combo.detailedResults.size();
            combo.avgTime = totalTime / combo.detailedResults.size();
            combo.avgValue = totalVal / combo.detailedResults.size();
            
            // Add Average Row for each combination
            file << "AVERAGE_" << combo.kAlgo << "+" << combo.pAlgo << "," 
                 << combo.kAlgo << "," 
                 << combo.pAlgo << "," 
                 << "ALL_10_DATASETS," 
                 << fixed << setprecision(2) << combo.avgFill << "," 
                 << combo.avgTime << "," 
                 << combo.avgValue << "\n";
        }
        file << ",,,,,, \n"; // Spacer row
    }
    file.close();

    // Summary table for console
    cout << "\nSummary of Averages (Top 5 combinations):\n";
    cout << "---------------------------------------------------------------\n";
    cout << setw(15) << "Combo" << setw(15) << "Avg Fill (%)" << setw(15) << "Avg Time (ms)" << endl;
    
    // Sort to show top
    sort(allCombos.begin(), allCombos.end(), [](const CombinationResult& a, const CombinationResult& b) {
        return a.avgFill > b.avgFill;
    });

    for (int i = 0; i < 5 && i < allCombos.size(); ++i) {
        cout << setw(15) << (allCombos[i].kAlgo + "+" + allCombos[i].pAlgo) 
             << setw(15) << fixed << setprecision(2) << allCombos[i].avgFill 
             << setw(15) << allCombos[i].avgTime << endl;
    }

    cout << "\n[SUCCESS] Full 210-row report exported to: results/comprehensive_report_10_datasets.csv\n";
    return 0;
}

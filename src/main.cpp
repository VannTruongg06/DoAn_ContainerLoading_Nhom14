#include <iostream>
#include <vector>
#include <chrono>
#include "models.h"
#include "knapsack_dp.h"
#include "greedy_packing.h"
#include "genetic_algorithm.h"

int main() {
    std::cout << "--- Chuong trinh Container Loading Nhom 12 ---\n";

    // 1. Khoi tao Container va danh sach Item (Mau)
    Container myContainer(100, 100, 100, 500);
    std::vector<Item> items = {
        Item(1, 10, 10, 10, 50),
        Item(2, 20, 20, 20, 100)
    };

    // 2. Chay thuat toan Knapsack DP (Loc don hang)
    auto startDP = std::chrono::high_resolution_clock::now();
    solveKnapsackDP(items, myContainer.maxWeight);
    auto endDP = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedDP = endDP - startDP;
    std::cout << "Thoi gian chay DP: " << elapsedDP.count() << "s\n";

    // 3. Chay thuat toan Greedy Packing
    auto startGreedy = std::chrono::high_resolution_clock::now();
    greedyPacking(myContainer, items);
    auto endGreedy = std::chrono::high_resolution_clock::now();
    std::cout << "Xong Greedy Packing.\n";

    // 4. Chay thuat toan Genetic Algorithm
    auto startGA = std::chrono::high_resolution_clock::now();
    geneticAlgorithmPacking(myContainer, items);
    auto endGA = std::chrono::high_resolution_clock::now();
    std::cout << "Xong Genetic Algorithm.\n";

    return 0;
}

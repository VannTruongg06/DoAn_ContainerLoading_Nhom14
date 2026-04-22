#ifndef KNAPSACK_ALGORITHMS_H
#define KNAPSACK_ALGORITHMS_H

#include <vector>
#include "models.h"

// 1. Giải bằng Quy hoạch động (DP Bottom-up)
std::vector<Item> solveKnapsackDP(const std::vector<Item>& items, int maxWeight);

// 2. Giải bằng Thuật toán Tham lam (Greedy theo Value/Weight)
std::vector<Item> solveKnapsackGreedy(std::vector<Item> items, int maxWeight);

// 3. Giải bằng Nhánh cận (Branch and Bound)
std::vector<Item> solveKnapsackBranchAndBound(const std::vector<Item>& items, int maxWeight);

#endif
#ifndef KNAPSACK_DP_H
#define KNAPSACK_DP_H

#include "models.h"
#include <vector>

// Hàm giải bài toán cái túi bằng DP để chọn lọc các mặt hàng tối ưu
void solveKnapsackDP(std::vector<Item>& items, int maxWeight);

#endif // KNAPSACK_DP_H

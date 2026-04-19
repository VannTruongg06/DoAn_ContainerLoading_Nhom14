#include "knapsack_dp.h"
#include <vector>
#include <algorithm>

using namespace std;

vector<Item> solveKnapsack(const vector<Item>& items, int maxWeight) {
    int n = items.size();
    // Khởi tạo bảng DP
    vector<vector<int>> dp(n + 1, vector<int>(maxWeight + 1, 0));

    // Vòng lặp DP
    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= maxWeight; w++) {
            if (items[i - 1].weight <= w) {
                // Chọn max giữa lấy và không lấy
                dp[i][w] = max(items[i - 1].value + dp[i - 1][w - items[i - 1].weight], dp[i - 1][w]);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    // Truy vết lấy danh sách Item
    vector<Item> selectedItems;
    int w = maxWeight;
    for (int i = n; i > 0; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            selectedItems.push_back(items[i - 1]);
            w -= items[i - 1].weight;
        }
    }
    return selectedItems;
}
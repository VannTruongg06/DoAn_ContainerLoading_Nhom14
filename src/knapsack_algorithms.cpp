#include "knapsack_algorithms.h"
#include <algorithm>
#include <queue>

using namespace std;

// 1. Quy Hoạch Động (DP) - Lựa chọn hàng theo Khối lượng và Giá trị
vector<Item> solveKnapsackDP(const vector<Item>& items, int maxWeight) {
    int n = items.size();
    vector<vector<int>> dp(n + 1, vector<int>(maxWeight + 1, 0));

    // Xây dựng bảng DP
    for (int i = 1; i <= n; ++i) {
        for (int w = 0; w <= maxWeight; ++w) {
            if (items[i - 1].weight <= w) {
                dp[i][w] = max(dp[i - 1][w], 
                               dp[i - 1][w - items[i - 1].weight] + items[i - 1].value);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    // Truy vết lấy kết quả
    vector<Item> selectedItems;
    int w = maxWeight;
    for (int i = n; i > 0 && w > 0; --i) {
        if (dp[i][w] != dp[i - 1][w]) {
            selectedItems.push_back(items[i - 1]);
            w -= items[i - 1].weight;
        }
    }
    return selectedItems;
}

// 2. Tham Lam (Greedy) - Sắp xếp theo tỷ lệ Value/Weight
vector<Item> solveKnapsackGreedy(vector<Item> items, int maxWeight) {
    // Sắp xếp giảm dần theo Value/Weight
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.getDensity() > b.getDensity();
    });

    vector<Item> selectedItems;
    int currentWeight = 0;

    for (const auto& item : items) {
        if (currentWeight + item.weight <= maxWeight) {
            selectedItems.push_back(item);
            currentWeight += item.weight;
        }
    }
    return selectedItems;
}

// Cấu trúc Node cho thuật toán Branch and Bound
struct Node {
    int level;
    int profit;
    int weight;
    double bound;
    vector<Item> currentSelection;
};

// Hàm tính cận trên (Bound) cho Branch and Bound
double bound(Node u, int n, int maxWeight, const vector<Item>& items) {
    if (u.weight >= maxWeight) return 0;
    double profit_bound = u.profit;
    int j = u.level + 1;
    int totweight = u.weight;

    while ((j < n) && (totweight + items[j].weight <= maxWeight)) {
        totweight += items[j].weight;
        profit_bound += items[j].value;
        j++;
    }
    if (j < n) {
        profit_bound += (maxWeight - totweight) * items[j].getDensity();
    }
    return profit_bound;
}

// 3. Nhánh Cận (Branch and Bound) - Best First Search
vector<Item> solveKnapsackBranchAndBound(const vector<Item>& items_in, int maxWeight) {
    vector<Item> items = items_in;
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.getDensity() > b.getDensity();
    });

    struct NodeComparator {
        bool operator()(const Node& a, const Node& b) const {
            return a.bound < b.bound;
        }
    };

    priority_queue<Node, vector<Node>, NodeComparator> Q;
    Node u, v;
    u.level = -1; u.profit = 0; u.weight = 0;
    u.bound = bound(u, (int)items.size(), maxWeight, items);
    Q.push(u);

    int maxProfit = 0;
    vector<Item> bestSelection;
    int n = items.size();

    while (!Q.empty()) {
        u = Q.top(); Q.pop();

        if (u.bound <= maxProfit) continue;
        if (u.level == n - 1) continue;

        v.level = u.level + 1;

        // Nhánh 1: Chọn item v.level
        v.weight = u.weight + items[v.level].weight;
        v.profit = u.profit + items[v.level].value;
        v.currentSelection = u.currentSelection;
        v.currentSelection.push_back(items[v.level]);

        if (v.weight <= maxWeight) {
            if (v.profit > maxProfit) {
                maxProfit = v.profit;
                bestSelection = v.currentSelection;
            }
            v.bound = bound(v, n, maxWeight, items);
            if (v.bound > maxProfit) Q.push(v);
        }

        // Nhánh 2: Không chọn item v.level
        v.weight = u.weight;
        v.profit = u.profit;
        v.currentSelection = u.currentSelection;
        v.bound = bound(v, n, maxWeight, items);
        if (v.bound > maxProfit) Q.push(v);
    }
    return bestSelection;
}
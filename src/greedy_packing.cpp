#include "greedy_packing.h"
#include <algorithm>

using namespace std;

void greedyPacking(Container& container, vector<Item>& items) {
    // Sắp xếp theo thể tích giảm dần
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.getVolume() > b.getVolume();
    });

    // Duyệt danh sách item
    for (auto& item : items) {
        bool placed = false;
        
        // Quét không gian theo z, y, x
        for (int z = 0; z <= container.depth - item.depth; z++) {
            for (int y = 0; y <= container.height - item.height; y++) {
                for (int x = 0; x <= container.width - item.width; x++) {
                    
                    item.x = x;
                    item.y = y;
                    item.z = z;
                    
                    // Kiểm tra va chạm
                    bool collision = false;
                    for (const auto& packedItem : container.packedItems) {
                        if (isOverlap(item, packedItem)) {
                            collision = true;
                            break;
                        }
                    }
                    
                    // Nếu không va chạm thì đặt vào
                    if (!collision) {
                        item.isPacked = true;
                        container.packedItems.push_back(item);
                        placed = true;
                        break;
                    }
                }
                if (placed) break;
            }
            if (placed) break;
        }
    }
}
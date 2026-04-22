#include "packing_algorithms.h"
#include <algorithm>
#include <limits>

using namespace std;

// Cập nhật danh sách điểm cực trị (Extreme Points) sau khi đặt 1 item
void updateExtremePoints(vector<Point3D>& ep, const Item& item, const Container& cont) {
    // Xóa các EP nằm bên trong item vừa đặt
    ep.erase(remove_if(ep.begin(), ep.end(), [&](const Point3D& p) {
        return (p.x >= item.x && p.x < item.x + item.width &&
                p.y >= item.y && p.y < item.y + item.height &&
                p.z >= item.z && p.z < item.z + item.depth);
    }), ep.end());

    // Thêm 3 điểm mới tạo ra bởi item (Max X, Max Y, Max Z)
    if (item.x + item.width < cont.width) ep.emplace_back(item.x + item.width, item.y, item.z);
    if (item.y + item.height < cont.height) ep.emplace_back(item.x, item.y + item.height, item.z);
    if (item.z + item.depth < cont.depth) ep.emplace_back(item.x, item.y, item.z + item.depth);

    // Loại bỏ các EP trùng lặp (đơn giản hóa)
    // Thực tế EPH cần chiếu điểm xuống (Project points) để tối ưu, đây là bản cơ bản
}

// 4-8. Hàm giải quyết bài toán xếp hàng với các chiến lược khác nhau
vector<Container> solveBasicPacking(vector<Item> items, Container baseCont, Strategy strat) {
    vector<Container> result;
    
    // Xử lý tiền sắp xếp cho FFD, BFD
    if (strat == FFD || strat == BFD) {
        sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
            return a.getVolume() > b.getVolume();
        });
    }

    // Khởi tạo container đầu tiên
    result.push_back(baseCont);

    for (auto& item : items) {
        bool placed = false;
        
        for (auto& cont : result) {
            if (cont.getCurrentWeight() + item.weight > cont.maxWeight) continue;

            if (strat == EXTREME_POINT) {
                // EXTREME POINT STRATEGY
                vector<Point3D> ep;
                if (cont.packedItems.empty()) ep.emplace_back(0, 0, 0);
                else {
                    // Tái tạo lại EP list (Mô phỏng)
                    ep.emplace_back(0,0,0);
                    for(const auto& pi : cont.packedItems) updateExtremePoints(ep, pi, cont);
                }

                int bestVol = numeric_limits<int>::max();
                Point3D bestPoint(-1, -1, -1);

                for (const auto& p : ep) {
                    if (p.x + item.width <= cont.width && p.y + item.height <= cont.height && p.z + item.depth <= cont.depth) {
                        item.x = p.x; item.y = p.y; item.z = p.z;
                        bool overlap = false;
                        for (const auto& pi : cont.packedItems) {
                            if (isOverlap(item, pi)) { overlap = true; break; }
                        }
                        if (!overlap) {
                            // Cải tiến: Chọn điểm tạo ra ít khoảng trống nhất (tương tự Best Fit trên tập EP)
                            int remainingSpace = (cont.width - (p.x + item.width)) + (cont.height - (p.y + item.height)) + (cont.depth - (p.z + item.depth));
                            if (remainingSpace < bestVol) {
                                bestVol = remainingSpace;
                                bestPoint = p;
                            }
                        }
                    }
                }
                
                if (bestPoint.x != -1) {
                    item.x = bestPoint.x; item.y = bestPoint.y; item.z = bestPoint.z;
                    item.isPacked = true;
                    cont.packedItems.push_back(item);
                    placed = true;
                    break;
                }
            } else {
                // FIRST FIT & BEST FIT STRATEGIES (Duyệt theo tọa độ cơ bản kiểu Shelf/Grid)
                int best_x = -1, best_y = -1, best_z = -1;
                int min_residual = numeric_limits<int>::max();

                for (int z = 0; z <= cont.depth - item.depth && !placed; z++) {
                    for (int y = 0; y <= cont.height - item.height && !placed; y++) {
                        for (int x = 0; x <= cont.width - item.width && !placed; x++) {
                            item.x = x; item.y = y; item.z = z;
                            bool overlap = false;
                            for (const auto& pi : cont.packedItems) {
                                if (isOverlap(item, pi)) { overlap = true; break; }
                            }

                            if (!overlap) {
                                if (strat == FIRST_FIT || strat == FFD) {
                                    item.isPacked = true;
                                    cont.packedItems.push_back(item);
                                    placed = true;
                                } else if (strat == BEST_FIT || strat == BFD) {
                                    int residual = (cont.width - x - item.width) + (cont.height - y - item.height) + (cont.depth - z - item.depth);
                                    if (residual < min_residual) {
                                        min_residual = residual;
                                        best_x = x; best_y = y; best_z = z;
                                    }
                                }
                            }
                        }
                    }
                }
                
                if ((strat == BEST_FIT || strat == BFD) && best_x != -1) {
                    item.x = best_x; item.y = best_y; item.z = best_z;
                    item.isPacked = true;
                    cont.packedItems.push_back(item);
                    placed = true;
                    break; // Đã xếp vào container hiện tại
                }
            }
            if (placed) break; // Chuyển sang item tiếp theo
        }

        // Nếu không xếp được vào các container hiện có, tạo container mới
        if (!placed) {
            Container newCont = baseCont;
            item.x = 0; item.y = 0; item.z = 0;
            item.isPacked = true;
            newCont.packedItems.push_back(item);
            result.push_back(newCont);
        }
    }
    return result;
}
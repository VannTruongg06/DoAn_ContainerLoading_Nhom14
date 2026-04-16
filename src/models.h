#ifndef MODELS_H
#define MODELS_H

#include <vector>
#include <string>

struct Item {
    int id;
    int width, height, depth;
    int weight;
    int value;  // [BỔ SUNG] Cước phí / Độ ưu tiên (Dùng cho bài toán Knapsack)
    
    // Tọa độ góc sau-dưới-trái sau khi xếp vào container
    int x, y, z;
    bool isPacked;
    int orientation; // 0: nguyen ban, 1-5: cac huong xoay khac (neu co)

    // Constructor cập nhật thêm value
    Item(int _id, int w, int h, int d, int we, int val) 
        : id(_id), width(w), height(h), depth(d), weight(we), value(val), 
          x(0), y(0), z(0), isPacked(false), orientation(0) {}

    // [BỔ SUNG] Hàm helper để các dev khác gọi ra tính thể tích cho nhanh
    long long getVolume() const {
        return (long long)width * height * depth;
    }
};

struct Container {
    int width, height, depth;
    int maxWeight;
    std::vector<Item> packedItems;

    Container(int w, int h, int d, int maxW) 
        : width(w), height(h), depth(d), maxWeight(maxW) {}
        
    // [BỔ SUNG] Hàm helper tính thể tích tổng của xe
    long long getMaxVolume() const {
        return (long long)width * height * depth;
    }
};

#endif // MODELS_H

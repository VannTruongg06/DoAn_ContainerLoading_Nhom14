#ifndef MODELS_H
#define MODELS_H

#include <vector>
#include <string>

struct Item {
    int id;
    int width, height, depth;   // Kích thước hiện tại (sau khi xoay)
    int o_w, o_h, o_d;          // Kích thước gốc (original)
    int weight;
    int value;
    
    int x, y, z;
    bool isPacked;
    int orientation; // 0-5: Các hướng xoay 3D

    Item(int _id, int w, int h, int d, int we, int val) 
        : id(_id), width(w), height(h), depth(d), o_w(w), o_h(h), o_d(d),
          weight(we), value(val), x(0), y(0), z(0), isPacked(false), orientation(0) {}

    long long getVolume() const {
        return (long long)width * height * depth;
    }

    // Hàm xoay kiện hàng (6 hướng trong không gian 3D)
    void rotate(int type) {
        orientation = type;
        switch(type) {
            case 0: width = o_w; height = o_h; depth = o_d; break;
            case 1: width = o_w; height = o_d; depth = o_h; break;
            case 2: width = o_h; height = o_w; depth = o_d; break;
            case 3: width = o_h; height = o_d; depth = o_w; break;
            case 4: width = o_d; height = o_w; depth = o_h; break;
            case 5: width = o_d; height = o_h; depth = o_w; break;
        }
    }
};

// Cấu trúc quản lý khoảng trống trong Container
struct Space {
    int x, y, z;
    int w, h, d;
    Space(int _x, int _y, int _z, int _w, int _h, int _d) 
        : x(_x), y(_y), z(_z), w(_w), h(_h), d(_d) {}
};

struct Container {
    int width, height, depth;
    int maxWeight;
    std::vector<Item> packedItems;

    Container(int w, int h, int d, int maxW) 
        : width(w), height(h), depth(d), maxWeight(maxW) {}
        
    long long getMaxVolume() const {
        return (long long)width * height * depth;
    }
};

// Hàm kiểm tra va chạm giữa 2 kiện hàng (Dùng chung cho Greedy và Genetic)
inline bool isOverlap(const Item& a, const Item& b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y &&
            a.z < b.z + b.depth && a.z + a.depth > b.z);
}

#endif // MODELS_H

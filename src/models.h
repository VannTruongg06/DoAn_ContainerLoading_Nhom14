#ifndef MODELS_H
#define MODELS_H

#include <vector>
#include <string>

struct Item {
    int id;
    int width, height, depth;
    int weight;
    // Tọa độ góc sau-dưới-trái sau khi xếp vào container
    int x, y, z;
    bool isPacked;

    Item(int _id, int w, int h, int d, int we) 
        : id(_id), width(w), height(h), depth(d), weight(we), x(0), y(0), z(0), isPacked(false) {}
};

struct Container {
    int width, height, depth;
    int maxWeight;
    std::vector<Item> packedItems;

    Container(int w, int h, int d, int maxW) 
        : width(w), height(h), depth(d), maxWeight(maxW) {}
};

#endif // MODELS_H

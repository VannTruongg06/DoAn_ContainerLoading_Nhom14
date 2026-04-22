#ifndef PACKING_ALGORITHMS_H
#define PACKING_ALGORITHMS_H

#include <vector>
#include "models.h"

// 4-8. Hàm chung giải quyết bài toán xếp hàng 3D dựa trên Strategy
std::vector<Container> solveBasicPacking(std::vector<Item> items, Container baseCont, Strategy strat);

#endif
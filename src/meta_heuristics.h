#ifndef META_HEURISTICS_H
#define META_HEURISTICS_H

#include <vector>
#include "models.h"

// 9. Giải thuật Di truyền (Genetic Algorithm)
std::vector<Container> solveGeneticAlgorithm(std::vector<Item> items, Container baseCont);

// 10. Thuật toán Luyện kim mô phỏng (Simulated Annealing)
std::vector<Container> solveSimulatedAnnealing(std::vector<Item> items, Container baseCont);

#endif
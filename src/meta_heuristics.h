#ifndef META_HEURISTICS_H
#define META_HEURISTICS_H

#include <vector>
#include "models.h"

// 9. Giải Thuật Di Truyền (GA)
std::vector<Container> solveGeneticAlgorithm(std::vector<Item> items, Container baseCont, Strategy strat = FFD, int generations = 50, int popSize = 20);

// 10. Thuật toán Luyện kim mô phỏng (Simulated Annealing)
std::vector<Container> solveSimulatedAnnealing(std::vector<Item> items, Container baseCont, Strategy strat = FIRST_FIT, int iterations = 100);

#endif

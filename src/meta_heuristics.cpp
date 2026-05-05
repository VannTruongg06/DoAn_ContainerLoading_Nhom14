#include "meta_heuristics.h"
#include "packing_algorithms.h"
#include <algorithm>
#include <random>
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std;

// Hàm đánh giá (Fitness) - Tỷ lệ lấp đầy trung bình của các Container
double calculateFitness(const vector<Container>& solution) {
    if (solution.empty()) return 0.0;
    double totalFill = 0;
    for (const auto& c : solution) {
        totalFill += (double)c.getUsedVolume() / c.getMaxVolume();
    }
    // Ưu tiên số lượng container ít nhất và lấp đầy cao nhất
    return totalFill / solution.size() + (1.0 / solution.size()); 
}

// 9. Giải Thuật Di Truyền (GA)
vector<Container> solveGeneticAlgorithm(vector<Item> items, Container baseCont, Strategy strat, int generations, int popSize) {
    random_device rd;
    mt19937 g(rd());

    // Khởi tạo quần thể
    vector<vector<Item>> population(popSize);
    for (int i = 0; i < popSize; i++) {
        population[i] = items;
        shuffle(population[i].begin(), population[i].end(), g);
    }

    vector<Container> bestSolution;
    double bestFitness = -1.0;

    for (int gen = 0; gen < generations; gen++) {
        vector<pair<double, vector<Item>>> fitnessList;

        // Đánh giá
        for (const auto& indiv : population) {
            vector<Container> sol = solveBasicPacking(indiv, baseCont, strat);
            double fit = calculateFitness(sol);
            fitnessList.push_back({fit, indiv});
            
            if (fit > bestFitness) {
                bestFitness = fit;
                bestSolution = sol;
            }
        }

        // Chọn lọc & Lai ghép (Crossover PMX đơn giản)
        sort(fitnessList.rbegin(), fitnessList.rend()); // Giảm dần
        vector<vector<Item>> newPopulation;
        
        // Giữ lại Elitism (Top 25%)
        for(int i = 0; i < popSize / 4; i++) {
            newPopulation.push_back(fitnessList[i].second);
        }

        // Lai ghép để bù đắp dân số
        while(newPopulation.size() < popSize) {
            const vector<Item>& p1 = fitnessList[rand() % (popSize/2)].second;
            const vector<Item>& p2 = fitnessList[rand() % (popSize/2)].second;
            
            // Lai ghép cắt điểm (Single-point Crossover)
            int split = rand() % p1.size();
            vector<Item> child;
            child.reserve(p1.size());
            
            // Copy phần đầu từ p1
            for(int i = 0; i < split; ++i) child.push_back(p1[i]);
            
            // Copy phần còn lại từ p2 nếu chưa có trong child
            for(const auto& item : p2) {
                bool exists = false;
                for(const auto& cItem : child) {
                    if(cItem.id == item.id) {
                        exists = true;
                        break;
                    }
                }
                if(!exists) child.push_back(item);
            }
            newPopulation.push_back(child);
        }

        // Đột biến (Swap Mutation)
        for (auto& indiv : newPopulation) {
            if (rand() % 100 < 10) { // 10% đột biến
                int idx1 = rand() % indiv.size();
                int idx2 = rand() % indiv.size();
                swap(indiv[idx1], indiv[idx2]);
            }
        }
        population = newPopulation;
    }

    return bestSolution;
}

// 10. Thuật toán Luyện kim mô phỏng (Simulated Annealing)
vector<Container> solveSimulatedAnnealing(vector<Item> items, Container baseCont, Strategy strat, int iterations) {
    double initialTemp = 1000.0;
    double finalTemp = 1.0;
    double coolingRate = pow(finalTemp / initialTemp, 1.0 / iterations);
    
    random_device rd;
    mt19937 g(rd());

    // Trạng thái hiện tại
    vector<Item> currentOrder = items;
    vector<Container> currentSolution = solveBasicPacking(currentOrder, baseCont, strat);
    double currentEnergy = -calculateFitness(currentSolution); // Tìm min của Energy (-Fitness)

    vector<Item> bestOrder = currentOrder;
    vector<Container> bestSolution = currentSolution;
    double bestEnergy = currentEnergy;

    double temp = initialTemp;
    for (int i = 0; i < iterations; ++i) {
        // Sinh trạng thái lân cận (Swap 2 items)
        vector<Item> newOrder = currentOrder;
        if (newOrder.size() >= 2) {
            int idx1 = rand() % newOrder.size();
            int idx2 = rand() % newOrder.size();
            swap(newOrder[idx1], newOrder[idx2]);
        }

        vector<Container> newSolution = solveBasicPacking(newOrder, baseCont, strat);
        double newEnergy = -calculateFitness(newSolution);

        // Chấp nhận trạng thái mới?
        if (newEnergy < currentEnergy) {
            currentOrder = newOrder;
            currentEnergy = newEnergy;
        } else {
            // Xác suất chấp nhận trạng thái xấu hơn
            double acceptanceProbability = exp((currentEnergy - newEnergy) / temp);
            uniform_real_distribution<double> dist(0.0, 1.0);
            if (dist(g) < acceptanceProbability) {
                currentOrder = newOrder;
                currentEnergy = newEnergy;
            }
        }

        // Cập nhật Best
        if (currentEnergy < bestEnergy) {
            bestEnergy = currentEnergy;
            bestSolution = solveBasicPacking(currentOrder, baseCont, strat);
        }

        // Làm lạnh
        temp *= coolingRate;
    }

    return bestSolution;
}

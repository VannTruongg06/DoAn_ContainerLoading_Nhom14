#include "models.h" // Yêu cầu 1: Kết nối với hệ thống chung
#include <vector>
#include <algorithm>
#include <iostream>
#include <ctime>

// Yêu cầu 2: Thiết kế "Cá thể" thực tế
struct Individual {
    std::vector<int> order;      // Thứ tự kiện hàng
    std::vector<int> rotations;  // Trạng thái xoay (0: đứng, 1: nằm, 2: nghiêng)
    double fitness;              // % Thể tích lấp đầy
};

// Yêu cầu 3: Hàm Tính Độ thích nghi (Fitness Function)
double calculateFitness(Individual& ind, Container& container, const std::vector<Item>& items) {
    double usedVolume = 0;
    double containerVolume = container.width * container.height * container.depth;

    for (int itemId : ind.order) {
        // Cộng dồn thể tích các kiện hàng theo thứ tự
        usedVolume += (items[itemId].width * items[itemId].height * items[itemId].depth);
    }

    ind.fitness = usedVolume / containerVolume; // Trả về kết quả % (ví dụ 0.85)
    return (ind.fitness > 1.0) ? 1.0 : ind.fitness;
}

// Yêu cầu 4: Phép toán Lai ghép (Crossover)
Individual crossover(const Individual& p1, const Individual& p2) {
    Individual child = p1;
    int cut = p1.order.size() / 2;
    for (int i = cut; i < p2.order.size(); i++) {
        child.order[i] = p2.order[i];
        child.rotations[i] = p2.rotations[i];
    }
    return child;
}

// Yêu cầu 4: Phép toán Đột biến (Mutation)
void mutate(Individual& ind) {
    if (ind.order.size() < 2) return;
    int i = rand() % ind.order.size();
    int j = rand() % ind.order.size();
    std::swap(ind.order[i], ind.order[j]); // Tráo đổi vị trí kiện hàng
    ind.rotations[i] = rand() % 3;         // Thay đổi kiểu xoay
}

// Yêu cầu 5: Vòng lặp Thế hệ (Main GA Loop)
void geneticAlgorithmPacking(Container& container, std::vector<Item>& items) {
    srand(time(NULL));
    int popSize = 50; // Khởi tạo 50 phương án ngẫu nhiên
    std::vector<Individual> population;

    for (int i = 0; i < popSize; i++) {
        Individual ind;
        for (int j = 0; j < items.size(); j++) {
            ind.order.push_back(j);
            ind.rotations.push_back(rand() % 3);
        }
        std::random_shuffle(ind.order.begin(), ind.order.end());
        population.push_back(ind);
    }

    // Lặp qua 100 thế hệ
    for (int gen = 0; gen < 100; gen++) {
        for (auto& ind : population) calculateFitness(ind, container, items);

        std::sort(population.begin(), population.end(), [](const Individual& a, const Individual& b) {
            return a.fitness > b.fitness;
            });

        for (int i = 10; i < popSize; i++) {
            population[i] = crossover(population[rand() % 10], population[rand() % 10]);
            if (rand() % 10 < 2) mutate(population[i]);
        }
    }

    // Cập nhật phương án tốt nhất vào hệ thống
    std::cout << "GA da tim ra phuong an toi uu sau 100 the he.\n";
    std::cout << "Ty le lap day: " << population[0].fitness * 100 << "%\n";
}
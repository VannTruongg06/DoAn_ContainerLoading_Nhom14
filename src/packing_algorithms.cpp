#include "packing_algorithms.h"

#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

namespace {

constexpr int kSearchStep = 5;

bool canFitInBounds(const Item& item, const Container& cont, int x, int y, int z) {
    return x >= 0 && y >= 0 && z >= 0 &&
           x + item.width <= cont.width &&
           y + item.height <= cont.height &&
           z + item.depth <= cont.depth;
}

bool hasOverlapAt(const Item& candidate, const Container& cont) {
    for (const auto& pi : cont.packedItems) {
        if (isOverlap(candidate, pi)) return true;
    }
    return false;
}

bool canPlaceAt(Item& candidate, const Container& cont, int x, int y, int z) {
    if (!canFitInBounds(candidate, cont, x, y, z)) return false;
    candidate.x = x;
    candidate.y = y;
    candidate.z = z;
    return !hasOverlapAt(candidate, cont);
}

bool tryPlaceFirstFitWithRotation(const Item& src, Container& cont, Item& placed, int step) {
    for (int rot = 0; rot < 6; ++rot) {
        Item candidate = src;
        candidate.rotate(rot);

        for (int z = 0; z <= cont.depth - candidate.depth; z += step) {
            for (int y = 0; y <= cont.height - candidate.height; y += step) {
                for (int x = 0; x <= cont.width - candidate.width; x += step) {
                    if (canPlaceAt(candidate, cont, x, y, z)) {
                        candidate.isPacked = true;
                        placed = candidate;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool tryPlaceBestFitWithRotation(const Item& src, Container& cont, Item& placed, int step) {
    int bestResidual = numeric_limits<int>::max();
    bool found = false;
    Item bestCandidate = src;

    for (int rot = 0; rot < 6; ++rot) {
        Item candidate = src;
        candidate.rotate(rot);

        for (int z = 0; z <= cont.depth - candidate.depth; z += step) {
            for (int y = 0; y <= cont.height - candidate.height; y += step) {
                for (int x = 0; x <= cont.width - candidate.width; x += step) {
                    if (!canPlaceAt(candidate, cont, x, y, z)) continue;

                    int residual = (cont.width - x - candidate.width) +
                                   (cont.height - y - candidate.height) +
                                   (cont.depth - z - candidate.depth);
                    if (residual < bestResidual) {
                        bestResidual = residual;
                        bestCandidate = candidate;
                        found = true;
                    }
                }
            }
        }
    }

    if (!found) return false;
    bestCandidate.isPacked = true;
    placed = bestCandidate;
    return true;
}

bool tryPlaceExtremePointWithRotation(const Item& src, Container& cont, Item& placed);

} // namespace

// Cap nhat danh sach diem cuc tri (Extreme Points) sau khi dat 1 item
void updateExtremePoints(vector<Point3D>& ep, const Item& item, const Container& cont) {
    ep.erase(remove_if(ep.begin(), ep.end(), [&](const Point3D& p) {
        return (p.x >= item.x && p.x < item.x + item.width &&
                p.y >= item.y && p.y < item.y + item.height &&
                p.z >= item.z && p.z < item.z + item.depth);
    }), ep.end());

    if (item.x + item.width < cont.width) ep.emplace_back(item.x + item.width, item.y, item.z);
    if (item.y + item.height < cont.height) ep.emplace_back(item.x, item.y + item.height, item.z);
    if (item.z + item.depth < cont.depth) ep.emplace_back(item.x, item.y, item.z + item.depth);
}

namespace {

bool tryPlaceExtremePointWithRotation(const Item& src, Container& cont, Item& placed) {
    vector<Point3D> ep;
    if (cont.packedItems.empty()) {
        ep.emplace_back(0, 0, 0);
    } else {
        ep.emplace_back(0, 0, 0);
        for (const auto& pi : cont.packedItems) updateExtremePoints(ep, pi, cont);
    }

    int bestResidual = numeric_limits<int>::max();
    bool found = false;
    Item bestCandidate = src;

    for (const auto& p : ep) {
        for (int rot = 0; rot < 6; ++rot) {
            Item candidate = src;
            candidate.rotate(rot);

            if (!canPlaceAt(candidate, cont, p.x, p.y, p.z)) continue;

            int residual = (cont.width - (p.x + candidate.width)) +
                           (cont.height - (p.y + candidate.height)) +
                           (cont.depth - (p.z + candidate.depth));
            if (residual < bestResidual) {
                bestResidual = residual;
                bestCandidate = candidate;
                found = true;
            }
        }
    }

    if (!found) return false;
    bestCandidate.isPacked = true;
    placed = bestCandidate;
    return true;
}

bool tryPlaceInEmptyContainer(const Item& src, Container& cont, Item& placed) {
    for (int rot = 0; rot < 6; ++rot) {
        Item candidate = src;
        candidate.rotate(rot);
        if (!canFitInBounds(candidate, cont, 0, 0, 0)) continue;
        candidate.x = 0;
        candidate.y = 0;
        candidate.z = 0;
        candidate.isPacked = true;
        placed = candidate;
        return true;
    }
    return false;
}

} // namespace

// 4-8. Ham giai quyet bai toan xep hang voi cac chien luoc khac nhau
vector<Container> solveBasicPacking(vector<Item> items, Container baseCont, Strategy strat, int step) {
    vector<Container> result;

    if (strat == FFD || strat == BFD) {
        sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
            return a.getVolume() > b.getVolume();
        });
    }

    result.push_back(baseCont);

    for (const auto& srcItem : items) {
        bool placed = false;

        for (auto& cont : result) {
            if (cont.getCurrentWeight() + srcItem.weight > cont.maxWeight) continue;

            Item placedItem = srcItem;
            bool ok = false;

            if (strat == EXTREME_POINT) {
                ok = tryPlaceExtremePointWithRotation(srcItem, cont, placedItem);
            } else if (strat == FIRST_FIT || strat == FFD) {
                ok = tryPlaceFirstFitWithRotation(srcItem, cont, placedItem, step);
            } else {
                ok = tryPlaceBestFitWithRotation(srcItem, cont, placedItem, step);
            }

            if (ok) {
                cont.packedItems.push_back(placedItem);
                placed = true;
                break;
            }
        }

        if (!placed) {
            Container newCont = baseCont;
            Item placedItem = srcItem;

            if (tryPlaceInEmptyContainer(srcItem, newCont, placedItem)) {
                newCont.packedItems.push_back(placedItem);
                result.push_back(newCont);
            } else {
                cerr << "[Packing] Bo qua item ID " << srcItem.id
                     << " vi khong the dat vao container moi (ke ca khi da xoay).\n";
            }
        }
    }

    return result;
}

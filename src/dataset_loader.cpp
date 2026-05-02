#include "dataset_loader.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace {

constexpr int kDefaultW = 100;
constexpr int kDefaultH = 100;
constexpr int kDefaultD = 100;
constexpr int kDefaultMaxWeight = 5000;

string trim(const string& s) {
    size_t first = 0;
    while (first < s.size() && isspace(static_cast<unsigned char>(s[first]))) ++first;
    if (first == s.size()) return "";

    size_t last = s.size() - 1;
    while (last > first && isspace(static_cast<unsigned char>(s[last]))) --last;
    return s.substr(first, last - first + 1);
}

bool isCommentOrEmpty(const string& rawLine) {
    string line = trim(rawLine);
    return line.empty() || line[0] == '#';
}

} // namespace

Container DatasetLoader::loadContainer(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Loi] Khong the mo file dataset: " << filename << "\n";
        return Container(kDefaultW, kDefaultH, kDefaultD, kDefaultMaxWeight);
    }

    string line;
    int lineNo = 0;
    while (getline(file, line)) {
        ++lineNo;
        if (isCommentOrEmpty(line)) continue;

        stringstream ss(trim(line));
        int w, h, d, maxWeight;
        if (ss >> w >> h >> d >> maxWeight) {
            if (w <= 0 || h <= 0 || d <= 0 || maxWeight <= 0) {
                cerr << "[Validation] LOI Header container khong hop le tai dong " << lineNo
                     << " trong file " << filename << ". Dung default container.\n";
                return Container(kDefaultW, kDefaultH, kDefaultD, kDefaultMaxWeight);
            }
            return Container(w, h, d, maxWeight);
        }

        cerr << "[Validation] LOI Header container sai dinh dang tai dong " << lineNo
             << " trong file " << filename << ". Dung default container.\n";
        return Container(kDefaultW, kDefaultH, kDefaultD, kDefaultMaxWeight);
    }

    cerr << "[Validation] Khong tim thay header container hop le trong file " << filename
         << ". Dung default container.\n";
    return Container(kDefaultW, kDefaultH, kDefaultD, kDefaultMaxWeight);
}

vector<Item> DatasetLoader::loadItems(const string& filename) {
    vector<Item> items;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Loi] Khong the mo file dataset: " << filename << "\n";
        return items;
    }

    Container c = loadContainer(filename);

    string line;
    bool skipFirstDataLine = true;
    int lineNo = 0;

    while (getline(file, line)) {
        ++lineNo;
        if (isCommentOrEmpty(line)) continue;

        if (skipFirstDataLine) {
            skipFirstDataLine = false;
            continue;
        }

        string clean = trim(line);
        stringstream ss(clean);
        int id, w, h, d, weight, val;

        if (ss >> id >> w >> h >> d >> weight >> val) {
            if (weight < 0) {
                cerr << "[Validation] CANH BAO dong " << lineNo << ": Item ID " << id
                     << " co khoi luong am (" << weight << "). Da gan ve 0.\n";
                weight = 0;
            }
            if (w <= 0 || h <= 0 || d <= 0) {
                cerr << "[Validation] LOI dong " << lineNo << ": Item ID " << id
                     << " co kich thuoc <= 0. Bo qua.\n";
                continue;
            }

            // Validate with rotation-aware rule: item is accepted if at least one orientation fits.
            vector<int> cDims = {c.width, c.height, c.depth};
            vector<int> iDims = {w, h, d};
            sort(cDims.begin(), cDims.end());
            sort(iDims.begin(), iDims.end());

            if (iDims[0] > cDims[0] || iDims[1] > cDims[1] || iDims[2] > cDims[2]) {
                cerr << "[Validation] LOI dong " << lineNo << ": Item ID " << id << " ("
                     << w << "x" << h << "x" << d << ") qua kho so voi Container ("
                     << c.width << "x" << c.height << "x" << c.depth << ").\n";
                continue;
            }

            items.emplace_back(id, w, h, d, weight, val);
        } else {
            cerr << "[Validation] LOI DINH DANG dong " << lineNo << ": \"" << clean
                 << "\". Bo qua dong loi.\n";
        }
    }

    return items;
}

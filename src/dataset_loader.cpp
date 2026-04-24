#include "dataset_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

using namespace std;

Container DatasetLoader::loadContainer(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Loi] Khong the mo file dataset: " << filename << "\n";
        // Trả về một Container mặc định nếu file lỗi
        return Container(100, 100, 100, 500); 
    }

    string line;
    // Regex lấy thông số WxHxD và MaxWeight từ header
    // Hỗ trợ cả 2 định dạng phổ biến:
    // 1. # Container: 590x239x235 cm  MaxWeight: 28000 kg
    // 2. # Container: 20ft (235x239x590 cm) MaxWeight: 24000 kg
    regex reContainer(R"(Container.*?(\d+)x(\d+)x(\d+).*?MaxWeight:\s*(\d+))");
    smatch match;

    while (getline(file, line)) {
        // Thoát khỏi vòng lặp nếu bắt đầu gặp dòng dữ liệu (không phải header bắt đầu bằng '#')
        if (!line.empty() && line[0] != '#') {
            break; 
        }
        
        if (regex_search(line, match, reContainer)) {
            int w = stoi(match[1]);
            int h = stoi(match[2]);
            int d = stoi(match[3]);
            int maxWeight = stoi(match[4]);
            return Container(w, h, d, maxWeight);
        }
    }
    
    // Nếu trong header không có thông số (vd file custom), dùng thông số mặc định
    return Container(100, 100, 100, 500); 
}

vector<Item> DatasetLoader::loadItems(const string& filename) {
    vector<Item> items;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Loi] Khong the mo file dataset: " << filename << "\n";
        return items;
    }

    string line;
    while (getline(file, line)) {
        // Bỏ qua dòng trống hoặc dòng chú thích Header
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        int id, w, h, d, weight, val;
        
        // Định dạng cột chuẩn: ID Width Height Depth Weight Value
        if (ss >> id >> w >> h >> d >> weight >> val) {
            items.emplace_back(id, w, h, d, weight, val);
        }
    }
    return items;
}

// read_unified.cpp
// ================
// C++ port of Python read_unified.py
// Dùng chung để đọc dữ liệu từ các nguồn (file test, orlib, bedbpp, esicup).
//
// Cách dùng:
//   auto container = load_container("data/input_50_items.txt");
//   auto items = load_items("data/input_50_items.txt");

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>

// Cấu trúc lưu thông số container
struct Container {
    int W = 100;
    int H = 100;
    int D = 100;
    int maxWeight = 500;
};

// Cấu trúc lưu một item
struct Item {
    int id;
    int w;
    int h;
    int d;
    int weight;
    int value;
};

// Parse một dòng header trả về Container (hoặc nullopt nếu không khớp)
std::optional<Container> parse_header_container(const std::string& line) {
    // Regex mẫu 1: Container: <name>? (WxHxD) MaxWeight: <weight>
    static const std::regex pattern1(
        R"(Container:\s*([\w]+)?\s*\(?(\d+)x(\d+)x(\d+)\)?\s*(?:cm)?\s*MaxWeight:\s*(\d+))",
        std::regex::icase
    );

    std::smatch match;
    if (std::regex_search(line, match, pattern1)) {
        Container c;
        c.W = std::stoi(match[2]);
        c.H = std::stoi(match[3]);
        c.D = std::stoi(match[4]);
        c.maxWeight = std::stoi(match[5]);
        return c;
    }

    // Fallback: Container: WxHxD cm MaxWeight: W
    static const std::regex pattern2(
        R"(Container:\s*(\d+)x(\d+)x(\d+)\s*(?:cm)?\s*MaxWeight:\s*(\d+))",
        std::regex::icase
    );
    if (std::regex_search(line, match, pattern2)) {
        Container c;
        c.W = std::stoi(match[1]);
        c.H = std::stoi(match[2]);
        c.D = std::stoi(match[3]);
        c.maxWeight = std::stoi(match[4]);
        return c;
    }
    return std::nullopt;
}

// Đọc thông số container từ header file
Container load_container(const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f.is_open()) {
        throw std::runtime_error("Không tìm thấy " + filepath);
    }

    std::string line;
    while (std::getline(f, line)) {
        // Xoá whitespace đầu cuối
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue; // dòng trống
        line = line.substr(start);

        if (line[0] == '#') {
            auto cont = parse_header_container(line);
            if (cont) {
                return *cont;
            }
        } else {
            // Hết phần header (gặp dòng không phải comment)
            break;
        }
    }

    // Trả về container mặc định nếu không tìm thấy trong header
    return Container{};
}

// Đọc danh sách item từ file
std::vector<Item> load_items(const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f.is_open()) {
        throw std::runtime_error("Không tìm thấy " + filepath);
    }

    std::vector<Item> items;
    std::string line;
    while (std::getline(f, line)) {
        // Loại bỏ khoảng trắng hai đầu
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);

        // Bỏ qua dòng comment
        if (line[0] == '#') continue;

        // Phân tách dòng thành các token số nguyên
        std::istringstream iss(line);
        Item it;
        if ((iss >> it.id >> it.w >> it.h >> it.d >> it.weight >> it.value)) {
            items.push_back(it);
        }
        // Nếu không đủ trường thì bỏ qua (lỗi parse)
    }
    return items;
}

// ------------------------------------------------------------------
// Chạy thử nghiệm nếu biên dịch file này độc lập
// ------------------------------------------------------------------
#ifdef UNIT_TEST_READ_UNIFIED
int main() {
    try {
        std::string test_file = "data/input_50_items.txt";
        // (điều chỉnh đường dẫn cho đúng với cấu trúc thư mục của bạn)

        Container c = load_container(test_file);
        std::cout << "Container: " << c.W << " x " << c.H << " x " << c.D
                  << "  MaxWeight: " << c.maxWeight << "\n";

        auto items = load_items(test_file);
        std::cout << "Loaded " << items.size() << " items.\n";
        if (!items.empty()) {
            const auto& first = items[0];
            std::cout << "First item: id=" << first.id
                      << " dimensions=" << first.w << "x" << first.h << "x" << first.d
                      << " weight=" << first.weight << " value=" << first.value << "\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "Lỗi: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
#endif
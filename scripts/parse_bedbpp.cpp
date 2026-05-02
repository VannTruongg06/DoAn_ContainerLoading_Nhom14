#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

namespace {

struct RawItem {
    int sequence = 0;
    int lengthMm = 0;
    int widthMm = 0;
    int heightMm = 0;
    double weightKg = 0.0;
};

struct ContainerSpec {
    int widthCm;
    int heightCm;
    int depthCm;
    int maxWeightKg;
};

const unordered_set<string> kRepresentativeOrders = {
    "00104461", "00100133", "00105537", "00107633", "00101567", "00106687", "00109666", "00105283",
    "00100090", "00108592", "00100139", "00106570", "00104836", "00100162", "00100623", "00106702"
};

ContainerSpec mapTargetToContainer(const string& target) {
    if (target == "euro-pallet") {
        // BED-BPP: base area 1200 x 800 mm, normalized to cm for fast benchmark loops.
        return {120, 200, 80, 1500};
    }
    if (target == "rollcontainer") {
        // BED-BPP: base area 800 x 700 mm, normalized to cm for fast benchmark loops.
        return {80, 180, 70, 700};
    }
    return {100, 180, 80, 1000};
}

int mmToCm(int mm) {
    return max(1, (mm + 5) / 10);
}

int kgToIntWeight(double kg) {
    if (kg < 0.0) return 0;
    int w = static_cast<int>(lround(kg));
    if (w == 0 && kg > 0.0) return 1;
    return w;
}

int scoreValue(int w, int h, int d, int weight) {
    long long volume = 1LL * w * h * d;
    return static_cast<int>(max(1LL, weight * 10LL + volume / 40LL));
}

class JsonCursor {
public:
    explicit JsonCursor(const string& json) : text(json), pos(0) {}

    void parse(bool exportAll,
               const string& outputDir,
               const string& manifestPath,
               int& writtenFiles,
               int& totalOrders) {
        ofstream manifest(manifestPath);
        if (!manifest.is_open()) {
            throw runtime_error("Khong the tao file manifest: " + manifestPath);
        }
        manifest << "order_id,target,total_items,exported,output_file\n";

        skipWhitespace();
        expect('{');

        while (true) {
            skipWhitespace();
            if (consume('}')) break;

            string orderId = parseString();
            skipWhitespace();
            expect(':');

            vector<RawItem> items;
            string target;
            parseOrder(items, target);
            ++totalOrders;

            bool shouldExport = exportAll || (kRepresentativeOrders.find(orderId) != kRepresentativeOrders.end());
            string outputFile;
            if (shouldExport) {
                outputFile = writeOrder(orderId, target, items, outputDir);
                if (!outputFile.empty()) {
                    ++writtenFiles;
                } else {
                    shouldExport = false;
                }
            }

            manifest << orderId << "," << target << "," << items.size() << ","
                     << (shouldExport ? "yes" : "no") << ","
                     << (shouldExport ? outputFile : "") << "\n";

            skipWhitespace();
            if (!consume(',')) {
                skipWhitespace();
                expect('}');
                break;
            }
        }
    }

private:
    const string& text;
    size_t pos;

    static string sanitizeName(const string& in) {
        string out;
        out.reserve(in.size());
        for (char c : in) {
            if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_') out.push_back(c);
            else out.push_back('_');
        }
        return out;
    }

    string writeOrder(const string& orderId,
                      const string& target,
                      const vector<RawItem>& rawItems,
                      const string& outputDir) {
        ContainerSpec cont = mapTargetToContainer(target);
        string safeTarget = sanitizeName(target.empty() ? "unknown" : target);
        string relPath = "bedbpp_order_" + orderId + "_" + safeTarget + ".txt";
        string fullPath = outputDir + "/" + relPath;

        ofstream out(fullPath);
        if (!out.is_open()) {
            cerr << "[Loi] Khong the tao file: " << fullPath << "\n";
            return "";
        }

        out << "# Source: BED-BPP raw JSON order_id=" << orderId << " target=" << target << "\n";
        out << "# Unit: centimeter (converted from mm), weight in kg\n";
        out << "# Container_Width Container_Height Container_Depth Max_Weight\n";
        out << cont.widthCm << " " << cont.heightCm << " " << cont.depthCm << " " << cont.maxWeightKg << "\n";
        out << "# ID Width Height Depth Weight Value\n";

        int itemId = 1;
        for (const auto& it : rawItems) {
            int w = mmToCm(it.lengthMm);
            int h = mmToCm(it.heightMm);
            int d = mmToCm(it.widthMm);
            int weight = kgToIntWeight(it.weightKg);

            if (w <= 0 || h <= 0 || d <= 0) continue;
            int value = scoreValue(w, h, d, weight);
            out << itemId++ << " " << w << " " << h << " " << d << " " << weight << " " << value << "\n";
        }

        return relPath;
    }

    void parseOrder(vector<RawItem>& items, string& target) {
        skipWhitespace();
        expect('{');
        while (true) {
            skipWhitespace();
            if (consume('}')) break;
            string key = parseString();
            skipWhitespace();
            expect(':');

            if (key == "item_sequence") {
                parseItemSequence(items);
            } else if (key == "properties") {
                parseProperties(target);
            } else {
                skipValue();
            }

            skipWhitespace();
            if (!consume(',')) {
                skipWhitespace();
                expect('}');
                break;
            }
        }
    }

    void parseItemSequence(vector<RawItem>& items) {
        skipWhitespace();
        expect('{');
        while (true) {
            skipWhitespace();
            if (consume('}')) break;
            parseString();
            skipWhitespace();
            expect(':');

            RawItem item;
            parseItem(item);
            items.push_back(item);

            skipWhitespace();
            if (!consume(',')) {
                skipWhitespace();
                expect('}');
                break;
            }
        }

        sort(items.begin(), items.end(), [](const RawItem& a, const RawItem& b) {
            return a.sequence < b.sequence;
        });
    }

    void parseItem(RawItem& item) {
        skipWhitespace();
        expect('{');
        while (true) {
            skipWhitespace();
            if (consume('}')) break;
            string key = parseString();
            skipWhitespace();
            expect(':');

            if (key == "length/mm") item.lengthMm = static_cast<int>(lround(parseNumber()));
            else if (key == "width/mm") item.widthMm = static_cast<int>(lround(parseNumber()));
            else if (key == "height/mm") item.heightMm = static_cast<int>(lround(parseNumber()));
            else if (key == "weight/kg") item.weightKg = parseNumber();
            else if (key == "sequence") item.sequence = static_cast<int>(lround(parseNumber()));
            else skipValue();

            skipWhitespace();
            if (!consume(',')) {
                skipWhitespace();
                expect('}');
                break;
            }
        }
    }

    void parseProperties(string& target) {
        skipWhitespace();
        expect('{');
        while (true) {
            skipWhitespace();
            if (consume('}')) break;
            string key = parseString();
            skipWhitespace();
            expect(':');

            if (key == "target") target = parseString();
            else skipValue();

            skipWhitespace();
            if (!consume(',')) {
                skipWhitespace();
                expect('}');
                break;
            }
        }
    }

    void skipValue() {
        skipWhitespace();
        if (pos >= text.size()) throw runtime_error("Unexpected EOF while skipping JSON value");

        char c = text[pos];
        if (c == '"') {
            parseString();
            return;
        }
        if (c == '{') {
            ++pos;
            while (true) {
                skipWhitespace();
                if (consume('}')) break;
                parseString();
                skipWhitespace();
                expect(':');
                skipValue();
                skipWhitespace();
                if (!consume(',')) {
                    skipWhitespace();
                    expect('}');
                    break;
                }
            }
            return;
        }
        if (c == '[') {
            ++pos;
            while (true) {
                skipWhitespace();
                if (consume(']')) break;
                skipValue();
                skipWhitespace();
                if (!consume(',')) {
                    skipWhitespace();
                    expect(']');
                    break;
                }
            }
            return;
        }
        if (startsWith("true")) {
            pos += 4;
            return;
        }
        if (startsWith("false")) {
            pos += 5;
            return;
        }
        if (startsWith("null")) {
            pos += 4;
            return;
        }
        parseNumber();
    }

    bool startsWith(const char* token) const {
        size_t i = 0;
        while (token[i] != '\0') {
            if (pos + i >= text.size() || text[pos + i] != token[i]) return false;
            ++i;
        }
        return true;
    }

    double parseNumber() {
        skipWhitespace();
        size_t start = pos;
        if (pos < text.size() && (text[pos] == '-' || text[pos] == '+')) ++pos;
        while (pos < text.size() && isdigit(static_cast<unsigned char>(text[pos]))) ++pos;
        if (pos < text.size() && text[pos] == '.') {
            ++pos;
            while (pos < text.size() && isdigit(static_cast<unsigned char>(text[pos]))) ++pos;
        }
        if (pos < text.size() && (text[pos] == 'e' || text[pos] == 'E')) {
            ++pos;
            if (pos < text.size() && (text[pos] == '-' || text[pos] == '+')) ++pos;
            while (pos < text.size() && isdigit(static_cast<unsigned char>(text[pos]))) ++pos;
        }
        if (start == pos) throw runtime_error("Invalid JSON number at position " + to_string(pos));
        return stod(text.substr(start, pos - start));
    }

    string parseString() {
        skipWhitespace();
        expect('"');
        string out;
        while (pos < text.size()) {
            char c = text[pos++];
            if (c == '"') return out;
            if (c == '\\') {
                if (pos >= text.size()) throw runtime_error("Invalid JSON escape");
                char e = text[pos++];
                switch (e) {
                    case '"': out.push_back('"'); break;
                    case '\\': out.push_back('\\'); break;
                    case '/': out.push_back('/'); break;
                    case 'b': out.push_back('\b'); break;
                    case 'f': out.push_back('\f'); break;
                    case 'n': out.push_back('\n'); break;
                    case 'r': out.push_back('\r'); break;
                    case 't': out.push_back('\t'); break;
                    case 'u':
                        for (int i = 0; i < 4 && pos < text.size(); ++i) ++pos;
                        out.push_back('?');
                        break;
                    default: out.push_back(e); break;
                }
            } else {
                out.push_back(c);
            }
        }
        throw runtime_error("Unterminated JSON string");
    }

    void skipWhitespace() {
        while (pos < text.size() && isspace(static_cast<unsigned char>(text[pos]))) ++pos;
    }

    bool consume(char c) {
        if (pos < text.size() && text[pos] == c) {
            ++pos;
            return true;
        }
        return false;
    }

    void expect(char c) {
        if (!consume(c)) {
            throw runtime_error(string("Expected '") + c + "' at position " + to_string(pos));
        }
    }
};

void writeBenchmarkMaster(const string& allTestsPath, const string& bedbppDir) {
    ofstream out(allTestsPath);
    if (!out.is_open()) {
        throw runtime_error("Khong the ghi file: " + allTestsPath);
    }

    out << "data/random/input_50_items.txt\n";
    out << "data/random/input_100_items.txt\n";
    out << "data/random/input_500_items.txt\n";
    out << "data/scenarios/uniform.txt\n";
    out << "data/scenarios/diverse.txt\n";
    out << "data/scenarios/weight_heavy.txt\n";
    out << "data/scenarios/voluminous.txt\n";
    out << "data/edge_cases/dirty_data.txt\n";

    vector<string> files;
    for (const auto& entry : filesystem::directory_iterator(bedbppDir)) {
        if (!entry.is_regular_file()) continue;
        string name = entry.path().filename().string();
        if (name.rfind("bedbpp_order_", 0) != 0) continue;
        if (entry.path().extension() != ".txt") continue;
        if (name.find("_rollcontainer.txt") == string::npos &&
            name.find("_euro-pallet.txt") == string::npos) {
            continue;
        }
        files.push_back("data/academic/bedbpp/" + name);
    }

    sort(files.begin(), files.end());
    for (const auto& f : files) out << f << "\n";
}

} // namespace

int main(int argc, char* argv[]) {
    cout << "===========================================================\n";
    cout << "  BED-BPP PARSER (RAW JSON -> STANDARDIZED TXT DATASETS)   \n";
    cout << "===========================================================\n";

    bool exportAll = false;
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "--all") exportAll = true;
    }

    const string rawPath = "data/raw/bedbpp.json";
    const string outDir = "data/academic/bedbpp";
    const string manifestPath = outDir + "/manifest.csv";
    const string masterPath = "data/all_tests.txt";

    try {
        filesystem::create_directories(outDir);

        ifstream in(rawPath);
        if (!in.is_open()) {
            cerr << "[Loi] Khong mo duoc raw dataset: " << rawPath << "\n";
            return 1;
        }

        string json((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        in.close();

        int writtenFiles = 0;
        int totalOrders = 0;

        JsonCursor parser(json);
        parser.parse(exportAll, outDir, manifestPath, writtenFiles, totalOrders);
        writeBenchmarkMaster(masterPath, outDir);

        cout << "[OK] Da parse " << totalOrders << " don hang BED-BPP.\n";
        cout << "[OK] Da xuat " << writtenFiles << " file dataset vao " << outDir << "\n";
        cout << "[OK] Da tao manifest: " << manifestPath << "\n";
        cout << "[OK] Da cap nhat Benchmark Master: " << masterPath << "\n";
        if (!exportAll) {
            cout << "[Ghi chu] Dang o che do mac dinh: chi xuat tap dai dien. Dung --all de xuat toan bo order.\n";
        }
        cout << "===========================================================\n";
    } catch (const exception& ex) {
        cerr << "[Loi] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}

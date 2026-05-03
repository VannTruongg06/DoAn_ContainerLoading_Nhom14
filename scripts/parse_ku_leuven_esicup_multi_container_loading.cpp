#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

using namespace std;

namespace {

struct BinSpec {
    string type;
    int lengthMm = 0;
    int widthMm = 0;
    int heightMm = 0;
    int maxWeightKg = 0;
};

struct RawItem {
    int sourceId = 0;
    int lengthMm = 0;
    int widthMm = 0;
    int heightMm = 0;
    double weightKg = 0.0;
    string packageMaterial;
    string orientationConstraint;
    string productId;
};

struct ParsedInstance {
    string setLabel;
    string instanceName;
    vector<BinSpec> bins;
    vector<RawItem> items;
    vector<string> parameterLines;
    int invalidItemRows = 0;
};

struct ExportStats {
    string setLabel;
    string instanceName;
    string binType;
    string relativeOutputPath;
    int sourceBinCount = 0;
    int rawItems = 0;
    int exportedItems = 0;
    int orientationConstrained = 0;
    int negativeWeightsClamped = 0;
    int oversizedItemsSkipped = 0;
    int invalidRowsSkipped = 0;
};

string trim(const string& s) {
    size_t first = 0;
    while (first < s.size() && isspace(static_cast<unsigned char>(s[first]))) ++first;
    if (first == s.size()) return "";

    size_t last = s.size() - 1;
    while (last > first && isspace(static_cast<unsigned char>(s[last]))) --last;
    return s.substr(first, last - first + 1);
}

string toLowerCopy(string value) {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(tolower(c));
    });
    return value;
}

vector<string> splitSemicolon(const string& line) {
    vector<string> parts;
    string current;

    for (char c : line) {
        if (c == ';') {
            parts.push_back(trim(current));
            current.clear();
        } else {
            current.push_back(c);
        }
    }

    parts.push_back(trim(current));
    return parts;
}

string sanitizeToken(const string& input) {
    string out;
    out.reserve(input.size());
    for (char c : input) {
        if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_') out.push_back(c);
        else out.push_back('_');
    }
    return out;
}

string escapeForPythonSingleQuotes(const string& text) {
    string out;
    out.reserve(text.size() + 8);
    for (char c : text) {
        if (c == '\\') out += "\\\\";
        else if (c == '\'') out += "\\'";
        else out.push_back(c);
    }
    return out;
}

bool parseIntStrict(const string& token, int& value) {
    try {
        size_t idx = 0;
        int parsed = stoi(trim(token), &idx);
        string clean = trim(token);
        if (idx != clean.size()) return false;
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool parseDoubleStrict(const string& token, double& value) {
    try {
        size_t idx = 0;
        double parsed = stod(trim(token), &idx);
        string clean = trim(token);
        if (idx != clean.size()) return false;
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

int mmToCm(int mm) {
    return max(1, (mm + 5) / 10);
}

int kgToIntWeight(double kg) {
    if (kg < 0.0) return 0;
    int weight = static_cast<int>(ceil(kg - 1e-9));
    if (weight == 0 && kg > 0.0) return 1;
    return weight;
}

int scoreValue(int width, int height, int depth, int weight, bool hasConstraint) {
    long long volume = 1LL * width * height * depth;
    long long value = weight * 10LL + volume / 40LL + (hasConstraint ? 5LL : 0LL);
    value = max(1LL, min(2000000000LL, value));
    return static_cast<int>(value);
}

bool canFitWithRotation(int cWidth, int cHeight, int cDepth, int iWidth, int iHeight, int iDepth) {
    vector<int> containerDims = {cWidth, cHeight, cDepth};
    vector<int> itemDims = {iWidth, iHeight, iDepth};
    sort(containerDims.begin(), containerDims.end());
    sort(itemDims.begin(), itemDims.end());

    return itemDims[0] <= containerDims[0] &&
           itemDims[1] <= containerDims[1] &&
           itemDims[2] <= containerDims[2];
}

fs::path findChildDirectoryIgnoreCase(const fs::path& parent, const string& expectedLowerName) {
    if (!fs::exists(parent)) return {};

    for (const auto& entry : fs::directory_iterator(parent)) {
        if (!entry.is_directory()) continue;
        if (toLowerCopy(entry.path().filename().string()) == expectedLowerName) {
            return entry.path();
        }
    }
    return {};
}

bool hasDatasetMarkers(const fs::path& root) {
    if (!fs::exists(root)) return false;
    return fs::exists(root / "InstancesA.zip") ||
           fs::exists(root / "InstancesB.zip") ||
           fs::exists(root / "InstancesX.zip") ||
           fs::exists(root / "instancesA") ||
           fs::exists(root / "instancesB") ||
           fs::exists(root / "instancesX");
}

fs::path detectDatasetRoot() {
    vector<fs::path> candidates = {
        fs::current_path() / "KU Leuven  ESICUP Multi-Container Loading",
        fs::current_path().parent_path() / "KU Leuven  ESICUP Multi-Container Loading",
        fs::current_path()
    };

    for (const auto& candidate : candidates) {
        if (hasDatasetMarkers(candidate)) return fs::absolute(candidate);
    }

    throw runtime_error(
        "Khong tim thay folder dataset KU Leuven / ESICUP Multi-Container Loading. "
        "Hay truyen duong dan folder chua InstancesA.zip, InstancesB.zip, InstancesX.zip."
    );
}

void ensureDirectory(const fs::path& dir) {
    fs::create_directories(dir);
}

void extractArchive(const fs::path& zipPath, const fs::path& destination) {
    if (fs::exists(destination)) {
        fs::remove_all(destination);
    }
    fs::create_directories(destination.parent_path());

    string command =
        "python -c \"import zipfile; "
        "zipfile.ZipFile(r'" + escapeForPythonSingleQuotes(fs::absolute(zipPath).string()) +
        "').extractall(r'" + escapeForPythonSingleQuotes(fs::absolute(destination).string()) +
        "')\"";

    int exitCode = system(command.c_str());
    if (exitCode != 0) {
        throw runtime_error("Khong the giai nen archive: " + zipPath.string());
    }
}

fs::path resolveSetRoot(const fs::path& datasetRoot,
                        const string& zipName,
                        const string& expectedDirName,
                        const fs::path& extractionRoot) {
    fs::path directDir = datasetRoot / expectedDirName;
    if (fs::exists(directDir) && fs::is_directory(directDir)) {
        return fs::absolute(directDir);
    }

    fs::path caseInsensitiveDir = findChildDirectoryIgnoreCase(datasetRoot, toLowerCopy(expectedDirName));
    if (!caseInsensitiveDir.empty()) return fs::absolute(caseInsensitiveDir);

    fs::path zipPath = datasetRoot / zipName;
    if (!fs::exists(zipPath)) {
        throw runtime_error("Khong tim thay " + zipName + " hoac folder " + expectedDirName);
    }

    fs::path extractionTarget = extractionRoot / fs::path(zipName).stem();
    extractArchive(zipPath, extractionTarget);

    fs::path extractedDir = extractionTarget / expectedDirName;
    if (fs::exists(extractedDir) && fs::is_directory(extractedDir)) {
        return fs::absolute(extractedDir);
    }

    fs::path caseInsensitiveExtracted = findChildDirectoryIgnoreCase(extractionTarget, toLowerCopy(expectedDirName));
    if (!caseInsensitiveExtracted.empty()) return fs::absolute(caseInsensitiveExtracted);

    throw runtime_error("Da giai nen " + zipName + " nhung khong tim thay thu muc " + expectedDirName);
}

vector<BinSpec> parseBinFile(const fs::path& filePath) {
    ifstream in(filePath);
    if (!in.is_open()) {
        throw runtime_error("Khong the mo file bins: " + filePath.string());
    }

    vector<BinSpec> bins;
    string line;
    bool skippedHeader = false;

    while (getline(in, line)) {
        string clean = trim(line);
        if (clean.empty()) continue;
        if (!skippedHeader) {
            skippedHeader = true;
            continue;
        }

        vector<string> parts = splitSemicolon(clean);
        if (parts.size() < 5) continue;

        BinSpec bin;
        bin.type = parts[0];
        if (!parseIntStrict(parts[1], bin.lengthMm) ||
            !parseIntStrict(parts[2], bin.widthMm) ||
            !parseIntStrict(parts[3], bin.heightMm) ||
            !parseIntStrict(parts[4], bin.maxWeightKg)) {
            continue;
        }

        if (bin.lengthMm <= 0 || bin.widthMm <= 0 || bin.heightMm <= 0 || bin.maxWeightKg <= 0) {
            continue;
        }

        bins.push_back(bin);
    }

    if (bins.empty()) {
        throw runtime_error("Khong doc duoc bin hop le tu " + filePath.string());
    }

    return bins;
}

vector<RawItem> parseItemFile(const fs::path& filePath, int& invalidRowsSkipped) {
    ifstream in(filePath);
    if (!in.is_open()) {
        throw runtime_error("Khong the mo file items: " + filePath.string());
    }

    vector<RawItem> items;
    string line;
    bool skippedHeader = false;

    while (getline(in, line)) {
        string clean = trim(line);
        if (clean.empty()) continue;
        if (!skippedHeader) {
            skippedHeader = true;
            continue;
        }

        vector<string> parts = splitSemicolon(clean);
        if (parts.size() < 8) {
            ++invalidRowsSkipped;
            continue;
        }

        RawItem item;
        if (!parseIntStrict(parts[0], item.sourceId) ||
            !parseIntStrict(parts[1], item.lengthMm) ||
            !parseIntStrict(parts[2], item.widthMm) ||
            !parseIntStrict(parts[3], item.heightMm) ||
            !parseDoubleStrict(parts[4], item.weightKg)) {
            ++invalidRowsSkipped;
            continue;
        }

        if (item.lengthMm <= 0 || item.widthMm <= 0 || item.heightMm <= 0) {
            ++invalidRowsSkipped;
            continue;
        }

        item.packageMaterial = parts[5];
        item.orientationConstraint = parts[6];
        item.productId = parts[7];
        items.push_back(item);
    }

    return items;
}

vector<string> parseParameterFile(const fs::path& filePath) {
    ifstream in(filePath);
    if (!in.is_open()) {
        throw runtime_error("Khong the mo file parameters: " + filePath.string());
    }

    vector<string> lines;
    string line;
    while (getline(in, line)) {
        string clean = trim(line);
        if (!clean.empty()) lines.push_back(clean);
    }
    return lines;
}

ParsedInstance parseInstanceDirectory(const fs::path& instanceDir, const string& setLabel) {
    ParsedInstance instance;
    instance.setLabel = setLabel;
    instance.instanceName = instanceDir.filename().string();
    instance.bins = parseBinFile(instanceDir / "input_bin.csv");
    instance.items = parseItemFile(instanceDir / "input_items.csv", instance.invalidItemRows);
    instance.parameterLines = parseParameterFile(instanceDir / "parameters.txt");
    return instance;
}

ExportStats exportStandardizedDataset(const ParsedInstance& instance,
                                      const BinSpec& bin,
                                      const fs::path& outputDir) {
    ExportStats stats;
    stats.setLabel = instance.setLabel;
    stats.instanceName = instance.instanceName;
    stats.binType = bin.type;
    stats.sourceBinCount = static_cast<int>(instance.bins.size());
    stats.rawItems = static_cast<int>(instance.items.size());
    stats.invalidRowsSkipped = instance.invalidItemRows;

    const int cWidth = mmToCm(bin.lengthMm);
    const int cHeight = mmToCm(bin.heightMm);
    const int cDepth = mmToCm(bin.widthMm);

    string filename =
        "ku_leuven_esicup_" + sanitizeToken(instance.setLabel) + "_" +
        sanitizeToken(instance.instanceName) + "_" + sanitizeToken(bin.type) + ".txt";
    fs::path fullPath = outputDir / filename;
    stats.relativeOutputPath = "data/academic/esicup/" + filename;

    ofstream out(fullPath);
    if (!out.is_open()) {
        throw runtime_error("Khong the tao file dataset: " + fullPath.string());
    }

    int runningOrientationConstraints = 0;
    int runningNegativeWeights = 0;
    int runningOversized = 0;

    vector<string> exportedRows;
    exportedRows.reserve(instance.items.size());

    for (const auto& raw : instance.items) {
        const bool hasConstraint = !trim(raw.orientationConstraint).empty();
        if (hasConstraint) ++runningOrientationConstraints;

        int width = mmToCm(raw.lengthMm);
        int height = mmToCm(raw.heightMm);
        int depth = mmToCm(raw.widthMm);

        if (!canFitWithRotation(cWidth, cHeight, cDepth, width, height, depth)) {
            ++runningOversized;
            continue;
        }

        if (raw.weightKg < 0.0) ++runningNegativeWeights;
        int weight = kgToIntWeight(raw.weightKg);
        int value = scoreValue(width, height, depth, weight, hasConstraint);

        ostringstream row;
        row << raw.sourceId << " "
            << width << " " << height << " " << depth << " "
            << weight << " " << value;
        exportedRows.push_back(row.str());
    }

    stats.exportedItems = static_cast<int>(exportedRows.size());
    stats.orientationConstrained = runningOrientationConstraints;
    stats.negativeWeightsClamped = runningNegativeWeights;
    stats.oversizedItemsSkipped = runningOversized;

    if (stats.exportedItems == 0) {
        out.close();
        fs::remove(fullPath);
        stats.relativeOutputPath.clear();
        return stats;
    }

    out << "# Source: KU Leuven / ESICUP Multi-Container Loading 2015 Challenge\n";
    out << "# Set=" << instance.setLabel
        << " Instance=" << instance.instanceName
        << " BinType=" << bin.type
        << " SourceBinCount=" << instance.bins.size() << "\n";
    out << "# Unit: dimensions converted from mm to cm, weight rounded up to integer kg\n";
    out << "# Note: source orientation constraints are counted for traceability but not enforced by current benchmark pipeline\n";
    out << "# OrientationConstrainedItems=" << stats.orientationConstrained
        << " OversizedItemsSkipped=" << stats.oversizedItemsSkipped
        << " InvalidRowsSkipped=" << stats.invalidRowsSkipped << "\n";
    if (!instance.parameterLines.empty()) {
        out << "# Parameters: ";
        for (size_t i = 0; i < instance.parameterLines.size(); ++i) {
            if (i) out << " | ";
            out << instance.parameterLines[i];
        }
        out << "\n";
    }
    out << "# Container_Width Container_Height Container_Depth Max_Weight\n";
    out << cWidth << " " << cHeight << " " << cDepth << " " << bin.maxWeightKg << "\n";
    out << "# ID Width Height Depth Weight Value\n";
    for (const auto& row : exportedRows) {
        out << row << "\n";
    }

    return stats;
}

vector<ExportStats> exportSet(const fs::path& setRoot, const string& setLabel, const fs::path& outputDir) {
    vector<ExportStats> exported;

    vector<fs::path> instanceDirs;
    for (const auto& entry : fs::directory_iterator(setRoot)) {
        if (entry.is_directory()) instanceDirs.push_back(entry.path());
    }

    sort(instanceDirs.begin(), instanceDirs.end());

    for (const auto& instanceDir : instanceDirs) {
        ParsedInstance instance = parseInstanceDirectory(instanceDir, setLabel);
        for (const auto& bin : instance.bins) {
            ExportStats stats = exportStandardizedDataset(instance, bin, outputDir);
            if (!stats.relativeOutputPath.empty()) {
                exported.push_back(stats);
            }
        }
    }

    return exported;
}

void writeManifest(const fs::path& manifestPath, const vector<ExportStats>& stats) {
    ofstream out(manifestPath);
    if (!out.is_open()) {
        throw runtime_error("Khong the tao manifest: " + manifestPath.string());
    }

    out << "set,instance,bin_type,source_bin_count,raw_items,exported_items,orientation_constraints,negative_weights_clamped,oversized_items_skipped,invalid_rows_skipped,output_file\n";
    for (const auto& row : stats) {
        out << row.setLabel << ","
            << row.instanceName << ","
            << row.binType << ","
            << row.sourceBinCount << ","
            << row.rawItems << ","
            << row.exportedItems << ","
            << row.orientationConstrained << ","
            << row.negativeWeightsClamped << ","
            << row.oversizedItemsSkipped << ","
            << row.invalidRowsSkipped << ","
            << row.relativeOutputPath << "\n";
    }
}

void writeListFile(const fs::path& path, const vector<string>& lines) {
    ofstream out(path);
    if (!out.is_open()) {
        throw runtime_error("Khong the ghi file danh sach: " + path.string());
    }

    for (const auto& line : lines) {
        out << line << "\n";
    }
}

vector<string> buildCompleteDatasetList(const vector<ExportStats>& exported) {
    vector<string> lines;
    lines.reserve(exported.size());
    for (const auto& row : exported) {
        lines.push_back(row.relativeOutputPath);
    }
    sort(lines.begin(), lines.end());
    return lines;
}

vector<string> pickRepresentativeDatasets(const vector<ExportStats>& exported, int perSet) {
    map<string, vector<ExportStats>> grouped;
    for (const auto& row : exported) {
        grouped[row.setLabel].push_back(row);
    }

    vector<string> chosen;

    for (auto& [setLabel, rows] : grouped) {
        sort(rows.begin(), rows.end(), [](const ExportStats& a, const ExportStats& b) {
            if (a.exportedItems != b.exportedItems) return a.exportedItems < b.exportedItems;
            if (a.instanceName != b.instanceName) return a.instanceName < b.instanceName;
            return a.binType < b.binType;
        });

        if (rows.empty()) continue;

        vector<size_t> candidateIndices;
        candidateIndices.push_back(0);
        candidateIndices.push_back(rows.size() / 4);
        candidateIndices.push_back(rows.size() / 2);
        candidateIndices.push_back((rows.size() * 3) / 4);
        candidateIndices.push_back(rows.size() - 1);

        set<size_t> usedIndices;
        for (size_t idx : candidateIndices) {
            if (idx >= rows.size()) continue;
            if (usedIndices.insert(idx).second) {
                chosen.push_back(rows[idx].relativeOutputPath);
            }
            if (static_cast<int>(usedIndices.size()) >= perSet) break;
        }

        for (size_t idx = 0; idx < rows.size() && static_cast<int>(usedIndices.size()) < perSet; ++idx) {
            if (usedIndices.insert(idx).second) {
                chosen.push_back(rows[idx].relativeOutputPath);
            }
        }
    }

    return chosen;
}

void mergeIntoBenchmarkMaster(const fs::path& masterPath, const vector<string>& esicupLines) {
    vector<string> merged;
    set<string> seen;

    if (fs::exists(masterPath)) {
        ifstream in(masterPath);
        if (!in.is_open()) {
            throw runtime_error("Khong the mo file benchmark master: " + masterPath.string());
        }

        string line;
        while (getline(in, line)) {
            string clean = trim(line);
            if (clean.empty() || clean[0] == '#') continue;
            if (clean.find("data/academic/esicup/") == 0) continue;
            if (seen.insert(clean).second) merged.push_back(clean);
        }
    }

    for (const auto& line : esicupLines) {
        if (seen.insert(line).second) merged.push_back(line);
    }

    writeListFile(masterPath, merged);
}

void printSummary(const vector<ExportStats>& exported, const vector<string>& benchmarkList) {
    map<string, int> countBySet;
    long long totalItems = 0;

    for (const auto& row : exported) {
        ++countBySet[row.setLabel];
        totalItems += row.exportedItems;
    }

    cout << "[OK] Tong file dataset ESICUP da xuat: " << exported.size() << "\n";
    cout << "[OK] Tong so item da chuan hoa: " << totalItems << "\n";
    for (const auto& [setLabel, count] : countBySet) {
        cout << "     - Set " << setLabel << ": " << count << " file\n";
    }
    cout << "[OK] So dataset dai dien dua vao benchmark master: " << benchmarkList.size() << "\n";
}

} // namespace

int main(int argc, char* argv[]) {
    cout << "====================================================================\n";
    cout << "  KU LEUVEN / ESICUP MULTI-CONTAINER LOADING PARSER (REAL DATA)    \n";
    cout << "====================================================================\n";

    try {
        fs::path datasetRoot = (argc >= 2) ? fs::path(argv[1]) : detectDatasetRoot();
        datasetRoot = fs::absolute(datasetRoot);

        const fs::path outputDir = fs::path("data") / "academic" / "esicup";
        const fs::path extractionRoot = fs::path("data") / "raw" / "esicup_extracted";
        const fs::path manifestPath = outputDir / "manifest.csv";
        const fs::path allExportsPath = outputDir / "all_exports.txt";
        const fs::path benchmarkSubsetPath = outputDir / "all_tests_esicup.txt";
        const fs::path benchmarkMasterPath = fs::path("data") / "all_tests.txt";

        ensureDirectory(outputDir);
        ensureDirectory(extractionRoot);

        cout << "[-] Dataset root: " << datasetRoot.string() << "\n";
        cout << "[-] Output dir  : " << fs::absolute(outputDir).string() << "\n";

        vector<ExportStats> exported;

        const vector<pair<string, string>> sets = {
            {"A", "InstancesA.zip"},
            {"B", "InstancesB.zip"},
            {"X", "InstancesX.zip"}
        };

        for (const auto& [setLabel, zipName] : sets) {
            string dirName = "instances" + toLowerCopy(setLabel);
            fs::path setRoot = resolveSetRoot(datasetRoot, zipName, dirName, extractionRoot);
            cout << "[+] Dang parse set " << setLabel << " tu " << setRoot.string() << "\n";
            vector<ExportStats> setExports = exportSet(setRoot, setLabel, outputDir);
            exported.insert(exported.end(), setExports.begin(), setExports.end());
        }

        if (exported.empty()) {
            throw runtime_error("Khong xuat duoc dataset ESICUP nao.");
        }

        sort(exported.begin(), exported.end(), [](const ExportStats& a, const ExportStats& b) {
            if (a.setLabel != b.setLabel) return a.setLabel < b.setLabel;
            if (a.instanceName != b.instanceName) return a.instanceName < b.instanceName;
            return a.binType < b.binType;
        });

        vector<string> allExports = buildCompleteDatasetList(exported);
        vector<string> benchmarkSubset = pickRepresentativeDatasets(exported, 5);

        writeManifest(manifestPath, exported);
        writeListFile(allExportsPath, allExports);
        writeListFile(benchmarkSubsetPath, benchmarkSubset);
        mergeIntoBenchmarkMaster(benchmarkMasterPath, benchmarkSubset);

        printSummary(exported, benchmarkSubset);
        cout << "[OK] Manifest        : " << manifestPath.string() << "\n";
        cout << "[OK] Tat ca dataset  : " << allExportsPath.string() << "\n";
        cout << "[OK] Benchmark subset: " << benchmarkSubsetPath.string() << "\n";
        cout << "[OK] Benchmark master: " << benchmarkMasterPath.string() << "\n";
        cout << "====================================================================\n";
    } catch (const exception& ex) {
        cerr << "[Loi] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

// C++ Program này sẽ tạo ra một đoạn mã Python động và sử dụng lệnh system() 
// để thực thi nó. Đây là cách C++ chuẩn (chuẩn chuyên nghiệp) để lợi dụng sức mạnh
// vẽ biểu đồ của Python mà không cần phải cài đặt thư viện đồ họa C++ nặng nề.

int main() {
    cout << "===============================================\n";
    cout << "  C++ PLOT BENCHMARK GENERATOR                 \n";
    cout << "===============================================\n";
    cout << "[+] Dang khoi tao script ve bieu do...\n";

    string pyFilename = "temp_plot.py";
    ofstream pyFile(pyFilename);

    if (!pyFile.is_open()) {
        cerr << "[Loi] Khong the tao file tam thoi!\n";
        return 1;
    }

    pyFile << "import os\n";
    pyFile << "import pandas as pd\n";
    pyFile << "import matplotlib.pyplot as plt\n";
    pyFile << "import seaborn as sns\n\n";
    
    pyFile << "CSV_PATH = '../results/benchmark_results.csv'\n";
    pyFile << "PLOTS_DIR = '../results/plots'\n";
    pyFile << "os.makedirs(PLOTS_DIR, exist_ok=True)\n";
    
    pyFile << "plt.rcParams['font.family'] = 'serif'\n";
    pyFile << "plt.rcParams['font.serif'] = ['Times New Roman']\n";
    pyFile << "plt.rcParams['font.size'] = 12\n\n";
    
    pyFile << "try:\n";
    pyFile << "    df = pd.read_csv(CSV_PATH)\n";
    pyFile << "    df['Algorithm'] = df['Knapsack'] + ' + ' + df['Packing']\n";
    
    pyFile << "    plt.figure(figsize=(10, 6))\n";
    pyFile << "    sns.barplot(data=df, x='Algorithm', y='FillRate(%)', hue='Dataset')\n";
    pyFile << "    plt.title('So sanh Ti le Lap day (Fill Rate)')\n";
    pyFile << "    plt.ylim(0, 105)\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '1_fill_rate_bar.png'), dpi=300)\n";
    pyFile << "    plt.close()\n";
    pyFile << "    print('  -> Da ve xong bieu do Fill Rate!')\n";

    pyFile << "except Exception as e:\n";
    pyFile << "    print('Loi:', e)\n";

    pyFile.close();

    cout << "[+] Dang goi thuc thi bieu do...\n";
    
    // Gọi Python để chạy file vừa tạo ra
    int res = system("python temp_plot.py");
    
    if (res == 0) {
        cout << "[+] Ve bieu do thanh cong!\n";
        remove(pyFilename.c_str()); // Xóa file rác
    } else {
        cout << "[!] Co loi xay ra hoac chua cai dat matplotlib/pandas.\n";
    }

    cout << "===============================================\n";
    return 0;
}

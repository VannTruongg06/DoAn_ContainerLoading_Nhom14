#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main() {
    cout << "===============================================\n";
    cout << "  C++ PLOT BENCHMARK GENERATOR (FIXED PATHS)   \n";
    cout << "===============================================\n";
    
    // Tạo thư mục kết quả nếu chưa có
    #ifdef _WIN32
        system("if not exist results\\plots mkdir results\\plots");
    #else
        system("mkdir -p results/plots");
    #endif

    string pyFilename = "temp_plot.py";
    ofstream pyFile(pyFilename);

    if (!pyFile.is_open()) {
        cerr << "[Loi] Khong the tao file script tam thoi!\n";
        return 1;
    }

    pyFile << "import os\n";
    pyFile << "import pandas as pd\n";
    pyFile << "import matplotlib.pyplot as plt\n";
    pyFile << "import seaborn as sns\n\n";
    
    // SỬA LỖI ĐƯỜNG DẪN: Chạy từ thư mục gốc dự án
    pyFile << "CSV_PATH = 'results/benchmark_results.csv'\n";
    pyFile << "PLOTS_DIR = 'results/plots'\n";
    
    pyFile << "plt.rcParams['font.family'] = 'serif'\n";
    pyFile << "plt.rcParams['font.size'] = 12\n\n";
    
    pyFile << "try:\n";
    pyFile << "    if not os.path.exists(CSV_PATH):\n";
    pyFile << "        print(f'[Loi] Khong tim thay file: {CSV_PATH}')\n";
    pyFile << "        exit(1)\n";
    
    pyFile << "    df = pd.read_csv(CSV_PATH)\n";
    pyFile << "    df['Algorithm'] = df['Knapsack'] + ' + ' + df['Packing']\n";
    
    // Biểu đồ 1: Fill Rate
    pyFile << "    plt.figure(figsize=(10, 6))\n";
    pyFile << "    sns.barplot(data=df, x='Algorithm', y='FillRate(%)', hue='Dataset')\n";
    pyFile << "    plt.title('So sanh Ti le Lap day (Fill Rate)')\n";
    pyFile << "    plt.ylim(0, 105)\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '1_fill_rate_bar.png'), dpi=300)\n";
    pyFile << "    plt.close()\n";
    pyFile << "    print('  -> Da ve xong bieu do Fill Rate!')\n";

    // Biểu đồ 2: Time
    pyFile << "    plt.figure(figsize=(10, 6))\n";
    pyFile << "    sns.lineplot(data=df, x='Algorithm', y='Time(ms)', hue='Dataset', marker='o')\n";
    pyFile << "    plt.title('So sanh Thoi gian thuc thi (ms)')\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '2_execution_time.png'), dpi=300)\n";
    pyFile << "    plt.close()\n";
    pyFile << "    print('  -> Da ve xong bieu do Thoi gian!')\n";

    pyFile << "except Exception as e:\n";
    pyFile << "    print('Loi trong qua trinh ve bieu do:', e)\n";

    pyFile.close();

    cout << "[+] Dang thuc thi Python script...\n";
    int res = system("python temp_plot.py");
    
    if (res == 0) {
        cout << "[+] Thanh cong! Anh da duoc luu tai results/plots/\n";
        remove(pyFilename.c_str()); 
    } else {
        cout << "[!] Co loi xay ra (Yeu cau: Python + pandas + seaborn + matplotlib).\n";
    }

    cout << "===============================================\n";
    return 0;
}

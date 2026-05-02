#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main() {
    cout << "===============================================\n";
    cout << "  C++ PLOT BENCHMARK GENERATOR (ENHANCED V2)   \n";
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
    pyFile << "import matplotlib\n";
    pyFile << "matplotlib.use('Agg')\n";
    pyFile << "import matplotlib.pyplot as plt\n";
    pyFile << "import seaborn as sns\n";
    pyFile << "import numpy as np\n\n";

    pyFile << "CSV_PATH = 'results/benchmark_results.csv'\n";
    pyFile << "PLOTS_DIR = 'results/plots'\n\n";

    pyFile << "plt.rcParams['font.family'] = 'serif'\n";
    pyFile << "plt.rcParams['font.size'] = 11\n";
    pyFile << "sns.set_theme(style='whitegrid', palette='Set2')\n\n";
    
    pyFile << "try:\n";
    pyFile << "    if not os.path.exists(CSV_PATH):\n";
    pyFile << "        print(f'[Loi] Khong tim thay file: {CSV_PATH}')\n";
    pyFile << "        exit(1)\n\n";

    pyFile << "    df = pd.read_csv(CSV_PATH)\n";
    pyFile << "    df['Algorithm'] = df['Knapsack'] + ' + ' + df['Packing']\n";
    pyFile << "    df['DatasetShort'] = df['Dataset'].apply(lambda x: os.path.basename(x).replace('.txt',''))\n\n";

    // Biểu đồ 1: Fill Rate so sánh theo Algorithm (trung bình)
    pyFile << "    # ===== BIEU DO 1: FILL RATE TRUNG BINH =====\n";
    pyFile << "    fig, ax = plt.subplots(figsize=(14, 7))\n";
    pyFile << "    avg_fill = df.groupby('Algorithm')['FillRate(%)'].mean().sort_values(ascending=False).reset_index()\n";
    pyFile << "    colors = sns.color_palette('viridis', len(avg_fill))\n";
    pyFile << "    bars = ax.bar(avg_fill['Algorithm'], avg_fill['FillRate(%)'], color=colors, edgecolor='black', linewidth=0.5)\n";
    pyFile << "    for bar in bars:\n";
    pyFile << "        height = bar.get_height()\n";
    pyFile << "        ax.annotate(f'{height:.1f}%', xy=(bar.get_x() + bar.get_width()/2, height),\n";
    pyFile << "                    xytext=(0, 5), textcoords='offset points', ha='center', fontsize=9, fontweight='bold')\n";
    pyFile << "    ax.set_title('So sanh Ti le Lap day Trung binh (Fill Rate %)', fontsize=14, fontweight='bold')\n";
    pyFile << "    ax.set_xlabel('Thuat toan', fontsize=12)\n";
    pyFile << "    ax.set_ylabel('Fill Rate (%)', fontsize=12)\n";
    pyFile << "    ax.set_ylim(0, max(avg_fill['FillRate(%)']) * 1.2)\n";
    pyFile << "    plt.xticks(rotation=30, ha='right')\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '1_fill_rate_bar.png'), dpi=300)\n";
    pyFile << "    plt.close()\n";
    pyFile << "    print('  -> Da ve xong bieu do Fill Rate!')\n\n";

    // Biểu đồ 2: Thời gian thực thi
    pyFile << "    # ===== BIEU DO 2: THOI GIAN THUC THI =====\n";
    pyFile << "    fig, ax = plt.subplots(figsize=(14, 7))\n";
    pyFile << "    avg_time = df.groupby('Algorithm')['Time(ms)'].mean().sort_values(ascending=True).reset_index()\n";
    pyFile << "    colors_t = sns.color_palette('rocket_r', len(avg_time))\n";
    pyFile << "    bars = ax.barh(avg_time['Algorithm'], avg_time['Time(ms)'], color=colors_t, edgecolor='black', linewidth=0.5)\n";
    pyFile << "    for bar in bars:\n";
    pyFile << "        width = bar.get_width()\n";
    pyFile << "        ax.annotate(f'{width:.2f} ms', xy=(width, bar.get_y() + bar.get_height()/2),\n";
    pyFile << "                    xytext=(5, 0), textcoords='offset points', ha='left', va='center', fontsize=9)\n";
    pyFile << "    ax.set_title('So sanh Thoi gian Thuc thi Trung binh (ms)', fontsize=14, fontweight='bold')\n";
    pyFile << "    ax.set_xlabel('Thoi gian (ms)', fontsize=12)\n";
    pyFile << "    ax.set_ylabel('Thuat toan', fontsize=12)\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '2_execution_time.png'), dpi=300)\n";
    pyFile << "    plt.close()\n";
    pyFile << "    print('  -> Da ve xong bieu do Thoi gian!')\n\n";

    // Biểu đồ 3: Tổng giá trị
    pyFile << "    # ===== BIEU DO 3: TONG GIA TRI =====\n";
    pyFile << "    fig, ax = plt.subplots(figsize=(14, 7))\n";
    pyFile << "    avg_val = df.groupby('Algorithm')['TotalValue($)'].mean().sort_values(ascending=False).reset_index()\n";
    pyFile << "    colors_v = sns.color_palette('mako', len(avg_val))\n";
    pyFile << "    bars = ax.bar(avg_val['Algorithm'], avg_val['TotalValue($)'], color=colors_v, edgecolor='black', linewidth=0.5)\n";
    pyFile << "    for bar in bars:\n";
    pyFile << "        height = bar.get_height()\n";
    pyFile << "        ax.annotate(f'${height:,.0f}', xy=(bar.get_x() + bar.get_width()/2, height),\n";
    pyFile << "                    xytext=(0, 5), textcoords='offset points', ha='center', fontsize=9, fontweight='bold')\n";
    pyFile << "    ax.set_title('So sanh Tong Gia tri Hang hoa Trung binh ($)', fontsize=14, fontweight='bold')\n";
    pyFile << "    ax.set_xlabel('Thuat toan', fontsize=12)\n";
    pyFile << "    ax.set_ylabel('Tong gia tri ($)', fontsize=12)\n";
    pyFile << "    ax.set_ylim(0, max(avg_val['TotalValue($)']) * 1.2)\n";
    pyFile << "    plt.xticks(rotation=30, ha='right')\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '3_total_value.png'), dpi=300)\n";
    pyFile << "    plt.close()\n";
    pyFile << "    print('  -> Da ve xong bieu do Tong Gia tri!')\n\n";

    // Biểu đồ 4: Heatmap Fill Rate theo Dataset x Algorithm
    pyFile << "    # ===== BIEU DO 4: HEATMAP FILL RATE =====\n";
    pyFile << "    fig, ax = plt.subplots(figsize=(16, 10))\n";
    pyFile << "    pivot = df.pivot_table(values='FillRate(%)', index='DatasetShort', columns='Algorithm', aggfunc='mean')\n";
    pyFile << "    sns.heatmap(pivot, annot=True, fmt='.1f', cmap='YlOrRd', linewidths=0.5, ax=ax, cbar_kws={'label': 'Fill Rate (%)'})\n";
    pyFile << "    ax.set_title('Heatmap: Fill Rate theo Dataset va Thuat toan', fontsize=14, fontweight='bold')\n";
    pyFile << "    ax.set_xlabel('Thuat toan', fontsize=12)\n";
    pyFile << "    ax.set_ylabel('Dataset', fontsize=12)\n";
    pyFile << "    plt.xticks(rotation=30, ha='right')\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '4_heatmap_fillrate.png'), dpi=300)\n";
    pyFile << "    plt.close()\n";
    pyFile << "    print('  -> Da ve xong Heatmap Fill Rate!')\n\n";

    // Biểu đồ 5: Fill Rate theo số lượng Items (line chart)
    pyFile << "    # ===== BIEU DO 5: FILL RATE THEO SO LUONG ITEMS =====\n";
    pyFile << "    fig, ax = plt.subplots(figsize=(12, 7))\n";
    pyFile << "    for algo in df['Algorithm'].unique():\n";
    pyFile << "        subset = df[df['Algorithm'] == algo].sort_values('Items')\n";
    pyFile << "        ax.plot(subset['Items'], subset['FillRate(%)'], marker='o', label=algo, linewidth=2, markersize=4)\n";
    pyFile << "    ax.set_title('Fill Rate theo so luong Items', fontsize=14, fontweight='bold')\n";
    pyFile << "    ax.set_xlabel('So luong Items', fontsize=12)\n";
    pyFile << "    ax.set_ylabel('Fill Rate (%)', fontsize=12)\n";
    pyFile << "    ax.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=8)\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '5_fillrate_by_items.png'), dpi=300)\n";
    pyFile << "    plt.close()\n";
    pyFile << "    print('  -> Da ve xong bieu do Fill Rate theo Items!')\n\n";

    pyFile << "    print('\\n==> HOAN THANH! Tong cong 5 bieu do da luu tai results/plots/')\n\n";

    pyFile << "except Exception as e:\n";
    pyFile << "    print('Loi trong qua trinh ve bieu do:', e)\n";
    pyFile << "    import traceback\n";
    pyFile << "    traceback.print_exc()\n";

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

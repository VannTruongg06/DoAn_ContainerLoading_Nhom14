#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main() {
    cout << "===============================================\n";
    cout << "  C++ PLOT BENCHMARK GENERATOR (V5 - FINAL)    \n";
    cout << "===============================================\n";
    
    #ifdef _WIN32
        system("if not exist results\\plots mkdir results\\plots");
    #else
        system("mkdir -p results/plots");
    #endif

    string pyFilename = "temp_plot.py";
    ofstream pyFile(pyFilename);

    pyFile << "import os, pandas as pd, matplotlib, matplotlib.pyplot as plt, seaborn as sns, numpy as np, re\n";
    pyFile << "matplotlib.use('Agg')\n";
    pyFile << "CSV_PATH = 'results/smart_benchmark.csv' if os.path.exists('results/smart_benchmark.csv') else 'results/benchmark_results.csv'\n";
    pyFile << "PLOTS_DIR = 'results/plots'\n";
    pyFile << "sns.set_theme(style='whitegrid')\n\n";
    
    pyFile << "try:\n";
    pyFile << "    df = pd.read_csv(CSV_PATH)\n";
    pyFile << "    if 'FillRate' in df.columns: df.rename(columns={'FillRate': 'FillRate(%)'}, inplace=True)\n";
    pyFile << "    if 'TimeMs' in df.columns: df.rename(columns={'TimeMs': 'Time(ms)'}, inplace=True)\n";
    pyFile << "    if 'TotalValue' in df.columns: df.rename(columns={'TotalValue': 'TotalValue($)'}, inplace=True)\n";
    pyFile << "    df['Algorithm'] = df['Knapsack'] + ' + ' + df['Packing']\n";
    
    // Ham suy luan so luong item tu ten dataset
    pyFile << "    def get_n(ds):\n";
    pyFile << "        m = re.search(r'(\\d+)', str(ds))\n";
    pyFile << "        return int(m.group(1)) if m else 500\n";
    pyFile << "    df['n'] = df['Dataset'].apply(get_n)\n\n";

    // 1. Packing Efficiency (Boxplot)
    pyFile << "    plt.figure(figsize=(10, 6))\n";
    pyFile << "    sns.boxplot(x='Packing', y='FillRate(%)', data=df, palette='Set3', hue='Packing', legend=False)\n";
    pyFile << "    plt.title('Stage 2: Packing Strategy Efficiency', fontsize=14, fontweight='bold')\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '1_packing_efficiency.png'), dpi=300)\n";
    pyFile << "    plt.close()\n\n";

    // 2. Execution Time
    pyFile << "    plt.figure(figsize=(10, 8))\n";
    pyFile << "    avg_time = df.groupby('Algorithm')['Time(ms)'].mean().sort_values()\n";
    pyFile << "    avg_time.plot(kind='barh', color=sns.color_palette('Reds_r', len(avg_time)))\n";
    pyFile << "    plt.title('Average Execution Time (ms)', fontsize=14, fontweight='bold')\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '2_execution_time.png'), dpi=300)\n";
    pyFile << "    plt.close()\n\n";

    // 3. Knapsack Optimization (STAGE 1 Analysis)
    pyFile << "    plt.figure(figsize=(10, 6))\n";
    pyFile << "    # Tinh hieu suat Knapsack thuan tuy so voi Max Value tim duoc cho tung Dataset\n";
    pyFile << "    df['K_Perf'] = df.groupby('Dataset')['SelectedValue'].transform(lambda x: (x / x.max()) * 100)\n";
    pyFile << "    ax3 = sns.barplot(x='Knapsack', y='K_Perf', data=df, palette='viridis', hue='Knapsack', legend=False, estimator=np.mean)\n";
    pyFile << "    plt.ylim(90, 105)\n";
    pyFile << "    plt.title('Stage 1 Analysis: Knapsack Selection Efficiency', fontsize=14, fontweight='bold')\n";
    pyFile << "    plt.ylabel('Selection Performance (%)')\n";
    pyFile << "    # Them nhan gia tri tren tung cot\n";
    pyFile << "    for p in ax3.patches:\n";
    pyFile << "        ax3.annotate(f'{p.get_height():.2f}%', (p.get_x() + p.get_width() / 2., p.get_height()), \n";
    pyFile << "                     ha='center', va='center', xytext=(0, 9), textcoords='offset points', fontweight='bold')\n";
    pyFile << "    plt.tight_layout()\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '3_knapsack_optimization.png'), dpi=300)\n";
    pyFile << "    plt.close()\n\n";

    // 4. Heatmap
    pyFile << "    plt.figure(figsize=(12, 8))\n";
    pyFile << "    pivot = df.pivot_table(values='FillRate(%)', index='Dataset', columns='Algorithm')\n";
    pyFile << "    sns.heatmap(pivot, annot=True, fmt='.1f', cmap='YlGnBu')\n";
    pyFile << "    plt.title('Fill Rate Heatmap (Robustness)', fontsize=14, fontweight='bold')\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '4_heatmap_fillrate.png'), dpi=300)\n";
    pyFile << "    plt.close()\n\n";

    // 5. Scalability Line Chart (HÌNH ĐƯỜNG DỄ NHÌN NHƯ Ý BẠN)
    pyFile << "    plt.figure(figsize=(12, 7))\n";
    pyFile << "    # Dung bang mau rong va cac kieu duong khac nhau\n";
    pyFile << "    algos = df['Algorithm'].unique()\n";
    pyFile << "    colors = plt.cm.tab20(np.linspace(0, 1, len(algos)))\n";
    pyFile << "    markers = ['o', 's', '^', 'D', 'x', '*', 'v', 'p', 'h', '+']\n";
    pyFile << "    for i, algo in enumerate(algos):\n";
    pyFile << "        sub = df[df['Algorithm']==algo].sort_values('n')\n";
    pyFile << "        if not sub.empty:\n";
    pyFile << "            plt.plot(sub['n'], sub['FillRate(%)'], label=algo, \n";
    pyFile << "                     color=colors[i], marker=markers[i % len(markers)], \n";
    pyFile << "                     linewidth=2, markersize=6, alpha=0.8)\n";
    pyFile << "    plt.title('Scalability: Fill Rate vs Number of Items (n)', fontsize=14, fontweight='bold')\n";
    pyFile << "    plt.xlabel('Number of Items (n)')\n";
    pyFile << "    plt.ylabel('Fill Rate (%)')\n";
    pyFile << "    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=8)\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '5_scalability_line_chart.png'), dpi=300, bbox_inches='tight')\n";
    pyFile << "    plt.close()\n\n";

    // 6. Trade-off Analysis (Scatter Plot SẠCH SẼ - KHÔNG CHỮ ĐÈ NHAU)
    pyFile << "    plt.figure(figsize=(10, 7))\n";
    pyFile << "    sns.scatterplot(data=df, x='Time(ms)', y='FillRate(%)', hue='Algorithm', style='Knapsack', s=150, palette='tab20')\n";
    pyFile << "    plt.xscale('log')\n";
    pyFile << "    plt.title('Trade-off: Speed vs Quality (Clean View)', fontsize=14, fontweight='bold')\n";
    pyFile << "    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=8)\n";
    pyFile << "    plt.savefig(os.path.join(PLOTS_DIR, '6_trade_off_scatter.png'), dpi=300, bbox_inches='tight')\n";
    pyFile << "    plt.close()\n\n";

    pyFile << "    print('Success: 6 plots generated.')\n";
    pyFile << "except Exception as e: print(f'Error: {e}')\n";
    pyFile.close();

    cout << "[+] Executing V5 plot script...\n";
    system("python temp_plot.py");
    remove("temp_plot.py");
    return 0;
}

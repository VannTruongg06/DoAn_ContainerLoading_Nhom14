@echo off
echo ===============================================
echo     BUILDING ALL C++ FILES (CONTAINER LOADING)
echo ===============================================

echo.
echo [1] Building Source Files (src/)...
g++ -std=c++17 src/*.cpp -Isrc -o main.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile main.exe
    exit /b %ERRORLEVEL%
)
echo [OK] main.exe compiled successfully.

echo.
echo [2] Building Script Files (scripts/)...

echo  - Compiling generate_data.exe...
g++ -std=c++17 scripts/generate_data.cpp -o generate_data.exe

echo  - Compiling benchmark.exe...
g++ -std=c++17 scripts/benchmark.cpp src/dataset_loader.cpp src/genetic_algorithm.cpp src/knapsack_algorithms.cpp src/meta_heuristics.cpp src/packing_algorithms.cpp -Isrc -o benchmark.exe

echo  - Compiling plot_benchmark.exe...
g++ -std=c++17 scripts/plot_benchmark.cpp -o plot_benchmark.exe

echo  - Compiling parse_bedbpp.exe...
g++ -std=c++17 scripts/parse_bedbpp.cpp -o parse_bedbpp.exe

echo  - Compiling parse_esicup.exe...
g++ -std=c++17 scripts/parse_esicup.cpp -o parse_esicup.exe

echo  - Compiling parse_ku_leuven_esicup_multi_container_loading.exe...
g++ -std=c++17 scripts/parse_ku_leuven_esicup_multi_container_loading.cpp -o parse_ku_leuven_esicup_multi_container_loading.exe

echo  - Compiling parse_orlib.exe...
g++ -std=c++17 scripts/parse_orlib.cpp -o parse_orlib.exe

echo.
echo ===============================================
echo     BUILD COMPLETE!
echo ===============================================
echo Run the generated .exe files to execute the code.
pause

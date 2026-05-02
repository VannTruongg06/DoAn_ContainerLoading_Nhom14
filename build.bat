@echo off
echo ===============================================
echo     BUILDING ALL C++ FILES (CONTAINER LOADING)
echo ===============================================

echo.
echo [1] Building Source Files (src/)...
g++ src/*.cpp -Isrc -o main.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile main.exe
    exit /b %ERRORLEVEL%
)
echo [OK] main.exe compiled successfully.

echo.
echo [2] Building Script Files (scripts/)...

echo  - Compiling generate_data.exe...
g++ scripts/generate_data.cpp -o generate_data.exe

echo  - Compiling benchmark.exe...
g++ scripts/benchmark.cpp src/*.cpp -Isrc -o benchmark.exe

echo  - Compiling plot_benchmark.exe...
g++ scripts/plot_benchmark.cpp -o plot_benchmark.exe

echo  - Compiling parse_bedbpp.exe...
g++ scripts/parse_bedbpp.cpp -o parse_bedbpp.exe

echo  - Compiling parse_esicup.exe...
g++ scripts/parse_esicup.cpp -o parse_esicup.exe

echo  - Compiling parse_orlib.exe...
g++ scripts/parse_orlib.cpp -o parse_orlib.exe

echo.
echo ===============================================
echo     BUILD COMPLETE!
echo ===============================================
echo Run the generated .exe files to execute the code.
pause

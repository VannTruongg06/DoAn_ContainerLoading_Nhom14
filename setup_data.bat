@echo off
echo ===============================================
echo      DATASET SETUP ^& GENERATION TOOL
echo ===============================================

if not exist data mkdir data
if not exist data\random mkdir data\random
if not exist data\scenarios mkdir data\scenarios
if not exist data\edge_cases mkdir data\edge_cases
if not exist data\academic mkdir data\academic
if not exist data\academic\orlib mkdir data\academic\orlib
if not exist data\academic\bedbpp mkdir data\academic\bedbpp
if not exist data\academic\esicup mkdir data\academic\esicup

echo.
echo [1/4] Dang sinh du lieu Random ^& Scenarios...
generate_data.exe

echo.
echo [2/4] Dang parse BED-BPP tu data\raw\bedbpp.json (du lieu thuc)...
parse_bedbpp.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] parse_bedbpp.exe that bai.
    exit /b %ERRORLEVEL%
)

echo.
echo [3/4] OR-Library demo parser (khong co raw local):
parse_orlib.exe

echo.
echo [4/4] ESICUP demo parser (khong co raw local):
parse_esicup.exe

echo.
echo ===============================================
echo    SETUP HOAN TAT!
echo    - Benchmark master: data\all_tests.txt
echo    - BED-BPP manifest: data\academic\bedbpp\manifest.csv
echo ===============================================
pause

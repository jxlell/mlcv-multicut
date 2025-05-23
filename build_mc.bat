@echo off
setlocal

REM === 1. MSVC-Umgebung aktivieren ===
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

REM === 2. Clang++-Build starten ===
"C:\Program Files\LLVM\bin\clang++.exe" ^
  --target=x86_64-pc-windows-msvc ^
  -fms-compatibility-version=19.00 ^
  -std=c++17 -O3 -g ^
  -o code\multicut.exe ^
  code\Image.cpp ^
  code\Util.cpp ^
  code\main.cpp ^
  code\huffman.cpp ^
  code\compress.cpp ^
  code\mc.cpp ^
  code\decompress.cpp ^
  -Izlib-win-x64-main\include ^
  -Lzlib-win-x64-main ^
  -lzdll ^
  -IC:\libs\opencv\opencv\build\include ^
  -LC:\libs\opencv\opencv\build\x64\vc16\lib ^
  -lopencv_world4110 ^
  -fcolor-diagnostics ^
  -fansi-escape-codes

REM === 3. Erfolgsmeldung ===
if exist code\multicut.exe (
    echo.
    echo ✅ Build erfolgreich: multicut.exe wurde erstellt.
) else (
    echo.
    echo ❌ Build fehlgeschlagen: multicut.exe wurde nicht erzeugt.
)

pause
endlocal

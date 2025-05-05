@echo off
setlocal

:: Clean old build
del game.exe 2>nul

set CC=compiler\bin\gcc.exe
set INCLUDE=include
set LIBS=libs

:: Compile
%CC% src\main.c -o game.exe ^
    -I%INCLUDE% ^
    -L%LIBS% ^
    -lraylib -lopengl32 -lgdi32 -lwinmm ^
    -mwindows -Wall -std=c99 -Os -s

if errorlevel 1 (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)

echo [SUCCESS] Build complete!
game.exe

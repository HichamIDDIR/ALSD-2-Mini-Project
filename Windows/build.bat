@echo off
SETLOCAL

:: Configure paths
set MINGW_PATH=mingw64\bin\gcc
set RAYLIB_INCLUDE=rayliblol\include
set RAYGUI_INCLUDE=raygui-master\src
set RAYLIB_LIB=rayliblol\lib

:: Compile
echo Compiling main.c...
"%MINGW_PATH%" main.c -o main.exe -I"%RAYLIB_INCLUDE%" -I"%RAYGUI_INCLUDE%" -L"%RAYLIB_LIB%" -lraylib -lopengl32 -lgdi32 -lwinmm -static -std=c99 -mwindows

:: Check for errors
if %errorlevel% neq 0 (
    echo.
    echo !!! COMPILATION FAILED !!!
    echo Check for errors above
    pause
    exit /b %errorlevel%
)

:: Run
echo.
echo Running program...
start "" "main.exe"

ENDLOCAL
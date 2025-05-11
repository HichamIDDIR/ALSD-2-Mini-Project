cmake_minimum_required(VERSION 3.10)
project(GameProject C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Os -s -mwindows")

include_directories(include)
link_directories(libs)

file(GLOB SOURCES src/*.c)

add_executable(game ${SOURCES})

target_link_libraries(game.exe raylib opengl32 gdi32 winmm)


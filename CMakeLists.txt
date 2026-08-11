cmake_minimum_required(VERSION 3.15)
project(Geoscape CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)

add_executable(geoscape main.cpp icosphere.cpp globe.cpp)
target_link_libraries(geoscape PRIVATE SDL2::SDL2 SDL2::SDL2main)
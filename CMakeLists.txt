cmake_minimum_required(VERSION 3.20)
project(laba5)

set(CMAKE_CXX_STANDARD 20)

find_package(Matplot++ REQUIRED)

add_executable(laba5 main.cpp split.h interpol.h format.h solve.h tridiag.h vector.h)

target_link_libraries(laba5 PUBLIC Matplot++::matplot)

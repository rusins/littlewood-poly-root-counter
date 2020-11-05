cmake_minimum_required(VERSION 3.10)

project("Littlewood Poly Root Plotter" VERSION 1.0.0)

add_executable(LPRP src/main.cpp)

find_package(GSL REQUIRED)
target_link_libraries(LPRP GSL::gsl)

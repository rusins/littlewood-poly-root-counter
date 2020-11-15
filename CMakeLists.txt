cmake_minimum_required(VERSION 3.10)

project("Littlewood Poly Root Plotter" VERSION 1.0.0)

add_executable(LPRP src/main.cpp)

# GSL and MPSolve are the 2 main libraries we use for solving polynomials

find_package(GSL REQUIRED) # GNU Scientific Library
find_package(Threads REQUIRED) # Required by MPSolve

target_link_libraries(LPRP GSL::gsl)
target_link_libraries(LPRP ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(LPRP gmp) # Required by MPSolve
target_link_libraries(LPRP mps) # MPSolve

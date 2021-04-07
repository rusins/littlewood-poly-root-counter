cmake_minimum_required(VERSION 3.10)

project("Littlewood Poly Root Plotter" VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)

# Uncomment to enable memory leak and uninitialized data access detection
add_compile_options(-fsanitize=address,undefined -Wall -Wextra -Wshadow -Wpedantic)
add_link_options(-fsanitize=address,undefined)

add_executable(LPRP src/main.cpp)
add_executable(solver src/program_solver.cpp)
add_executable(find-roots src/program_find_roots.cpp)
add_executable(count-roots src/program_count_roots.cpp)
add_executable(count-ones src/program_count_ones.cpp)
add_executable(unit-tests src/unit_tests.cpp)

# GSL and MPSolve are the 2 main libraries we use for solving polynomials

find_package(GSL REQUIRED) # GNU Scientific Library
find_package(Threads REQUIRED) # Required by MPSolve
find_package(GTest CONFIG REQUIRED) # Unit testing library

# CXXOPTS library is used for parsing command line options
add_subdirectory(submodules/cxxopts)

# Link libraries to all executables in a loop
foreach(executable LPRP solver find-roots count-roots count-ones unit-tests)
  target_link_libraries(${executable} GSL::gsl)
  target_link_libraries(${executable} ${CMAKE_THREAD_LIBS_INIT}) # Required by MPSolve
  target_link_libraries(${executable} gmp) # Required by MPSolve
  target_link_libraries(${executable} mps) # MPSolve
  target_link_libraries(${executable} cxxopts)
endforeach()
target_link_libraries(unit-tests GTest::gtest_main)

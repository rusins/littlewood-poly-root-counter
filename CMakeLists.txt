cmake_minimum_required(VERSION 3.10)

project("Littlewood Poly Root Plotter" VERSION 1.0.0)

add_compile_options(-fsanitize=address,undefined -Wall -Wextra -Wshadow -Wpedantic)
add_link_options(-fsanitize=address,undefined)

add_executable(LPRP src/main.cpp)
add_executable(solver src/solver.cpp)
add_executable(unit-tests src/unit_tests.cpp)

# GSL and MPSolve are the 2 main libraries we use for solving polynomials

find_package(GSL REQUIRED) # GNU Scientific Library
find_package(Threads REQUIRED) # Required by MPSolve
find_package(GTest CONFIG REQUIRED) # Unit testing library

# Link libraries to all executables in a loop
foreach(executable LPRP solver unit-tests)
  target_link_libraries(${executable} GSL::gsl)
  target_link_libraries(${executable} ${CMAKE_THREAD_LIBS_INIT}) # Required by MPSolve
  target_link_libraries(${executable} gmp) # Required by MPSolve
  target_link_libraries(${executable} mps) # MPSolve
endforeach()
target_link_libraries(unit-tests GTest::gtest_main)

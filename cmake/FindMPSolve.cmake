#[[
MIT License

Copyright (c) 2018 Hanqi Guo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
]]

find_path (MPSolve_INCLUDE_DIR NAMES mps/mps.h)
find_library (MPSolve_LIBRARY NAMES mps)

if(MPSolve_INCLUDE_DIR AND MPSolve_LIBRARY)
  set(MPSolve_FOUND TRUE)
endif(MPSolve_INCLUDE_DIR AND MPSolve_LIBRARY)

if(MPSolve_FOUND)
  if(NOT MPSolve_FIND_QUIETLY)
    message(STATUS "Found MPSolve: ${MPSolve_LIBRARY}")
  endif(NOT MPSolve_FIND_QUIETLY)
else(MPSolve_FOUND)
  if(MPSolve_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find MPSolve library.")
  endif(MPSolve_FIND_REQUIRED)
endif(MPSolve_FOUND)

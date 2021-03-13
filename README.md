# littlewood-poly-root-plotter
Tool to plot the roots of randomly generated Littlewood polynomials

## Building
### Dependencies
You should install the build tool CMake, as well as the libraries GSL and MPSolve. GTest (and GMock) is required for unit testing, but you can edit CMakeLists.txt to disable unit tests if you lack it.

### Build steps
Then execute the following commands from the top-level project directory:

```
mkdir build
cd build
cmake ..
make
```

## Running
After building you will have a number of executables in the `build` directory.

- `solver` is a small program to call the root finding libraries on a polynomial specified by the command line arguments.
- `LPRP` take a single parameter, the degree of the polynomial, and will randomly generate polynomials of that degree and verify the roots.
- `unit-tests` runs unit tests on the codebase.

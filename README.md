# littlewood-poly-root-counter
A tool to find and count the roots of randomly generated Littlewood polynomials

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

- `find-roots` is a program that solves randomly generated Littlewood polynomials, and saves their roots. Running it with the `-h` option will print the parameters it accepts.

    For example, running the program by executing
    `./find-roots -d 1-10 -a`
    will produce 10 output files in the current working directory, named `n-roots.txt`, where n is the degree. Each file starts with a comment stating how many polynomials the file contains, followed by a comment line with each polynomials coefficients, and then x lines with the roots of the polynomial. The `-a` option specifies that all possible polynomials of each degree should be iterated through. A specific number of iterations can be specified by using the parameter `-i`.

- `count-roots` is a program that reads roots from the root files in the current working directory, and counts how many of them are within a disk of radius 1/n^e centered at +1, and outputs the average number (count / polynomials) to standard output. The root files must be named `n-roots.txt`, where n is the degree of the polynomial.

    For example, running the program by executing
    `./count-roots -d 1-10 -a -e 0.5 -t 8`
    will read all the roots in the files for degrees 1 to 10, and count how many roots are within the disk of radius 1/sqrt(n). And it will use 8 worker threads to do so. To save this output to a file, simply pipe it:
    `./count-roots -d 1-10 -a -e -0.5 -t 8 > count.dat`

- `count-ones` is a program that counts the multiplicities of +1 as a root for random Littlewood polynomials. For each degree n, it outputs a line with n numbers: the probability of +1 being a root of that multiplicity.

    For example, running the program by executing
    `./count-ones -d 1-10 -a -t 8`
    will produce the following output:

    ```
    1 0.500000000000
    3 0.250000000000 0.125000000000 0
    5 0.312500000000 0 0 0 0
    7 0.242187500000 0.023437500000 0.007812500000 0 0 0 0
    9 0.246093750000 0 0 0 0 0 0 0 0
    ```

    Which means that for degree 1, there is a 50% chance of +1 being a root of multiplicity 1,
    for degree 3 there is a 25% chance of +1 being a root of multiplicity 1, and a 12.5% chance of it being a root of multiplicity 2, and so on.

    Just like with count-roots, pipe this output to a file in order to save it.

- `solver` is a small program to call the root finding libraries on a polynomial specified by the command line arguments.

    For example, running
    `./solver 2 0 1`
    corresonds to asking it to solve the polynomial x^2 + 2. So the first coefficient is for x^0, the second for x^1, and so on.

    
- `unit-tests` runs unit tests on the codebase.


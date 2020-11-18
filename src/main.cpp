#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>


#include "qr_algorithm.cpp"
#include "aberth_method.cpp"

using namespace std;

int main(int argc, char **argv) {
  int degree = argc - 2;
  if (degree <= 0) {
	cerr << "Enter the coefficients for the polynomial to solve!" << endl;
	return -1;
  }

  cout << "GSL -----------" << endl;
  // coefficient[y] is the coefficient for x^y
  double coefficients[degree + 1];
  for (int i = 0; i < degree + 1; ++i) {
	try {
	  coefficients[i] = atof(argv[i + 1]);
	} catch (logic_error e) {
	  cerr << "Unable to parse integer argument: " << argv[i] << endl;
	  return -1;
	}
  }

  // TODO: wrap in try block in case OOM
  qr_algorithm qr(degree);
  gsl_complex *roots = qr.solve(coefficients);
  
  for (int i = 0; i < degree; ++i) {
	cout << GSL_REAL(roots[i]) << " + " << GSL_IMAG(roots[i]) << "i" << endl;
  }


  cout << endl << "MPSolve --------" << endl;


  aberth_method ab(degree);
  roots = ab.solve(coefficients);
  
  for (int i = 0; i < degree; ++i) {
	cout << GSL_REAL(roots[i]) << " + " << GSL_IMAG(roots[i]) << "i" << endl;
  }

  return 0;
}

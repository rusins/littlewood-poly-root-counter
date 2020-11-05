#include <bits/stdc++.h>
#include <gsl/gsl_poly.h>

using namespace std;

int main (int argc, char **argv) {
    int degree = argc - 2;
  if (degree <= 0) {
	cerr << "Enter the coefficients for the polynomial to solve!" << endl;
	return -1;
  }

  // coefficient[y] is the coefficient for x^y
  double coefficients[degree + 1];
  // Every other value is the imaginary part of a root
  double roots[degree * 2];
  for (int i = 0; i < degree + 1; ++i) {
	try {
	  coefficients[i] = atof(argv[i + 1]);
	} catch (logic_error e) {
	  cerr << "Unable to parse integer argument: " << argv[i] << endl;
	  return -1;
	}
  }

  gsl_poly_complex_workspace *workspace = gsl_poly_complex_workspace_alloc(degree + 1);
  
  gsl_poly_complex_solve(coefficients, degree + 1, workspace, roots);

  for (int i = 0; i < degree; ++i) {
	cout << roots[i * 2] << " + " << roots[i * 2 + 1] << "i" << endl;
  }
  
  // Free memory
  gsl_poly_complex_workspace_free(workspace);
  return 0;
}

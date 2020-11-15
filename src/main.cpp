#include <bits/stdc++.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_complex.h>

#include <mps/mps.h>
#include <gmp.h>


using namespace std;

int main (int argc, char **argv) {
  int degree = argc - 2;
  if (degree <= 0) {
	cerr << "Enter the coefficients for the polynomial to solve!" << endl;
	return -1;
  }

  cout << "GSL -----------" << endl;
  // coefficient[y] is the coefficient for x^y
  double coefficients[degree + 1];
  // Every other value is the imaginary part of a root
  gsl_complex roots[degree];
  for (int i = 0; i < degree + 1; ++i) {
	try {
	  coefficients[i] = atof(argv[i + 1]);
	} catch (logic_error e) {
	  cerr << "Unable to parse integer argument: " << argv[i] << endl;
	  return -1;
	}
  }

  // TODO: wrap in try block in case OOM
  gsl_poly_complex_workspace *workspace = gsl_poly_complex_workspace_alloc(degree + 1);
  
  gsl_poly_complex_solve(coefficients, degree + 1, workspace, (double*) roots);

  for (int i = 0; i < degree; ++i) {
	cout << GSL_REAL(roots[i]) << " + " << GSL_IMAG(roots[i]) << "i" << endl;
  }
  
  // Free memory
  gsl_poly_complex_workspace_free(workspace);


  cout << endl << "MPSolve --------" << endl;
  mps_context *s = mps_context_new();
  mps_monomial_poly *p = mps_monomial_poly_new(s, degree);

  mps_context_select_algorithm(s, MPS_ALGORITHM_STANDARD_MPSOLVE);

  for (int i = 0; i < degree + 1; ++i)
	mps_monomial_poly_set_coefficient_d (s, p, i, coefficients[i], 0); 

  /* Set the input polynomial */
  mps_context_set_input_poly(s, MPS_POLYNOMIAL(p));

  /* Allocate space to hold the results. We check only floating point results
   * in here */
  cplx_t *results = cplx_valloc(degree);

  /* Actually solve the polynomial */
  mps_mpsolve(s);

  /* Save roots computed in the vector results */
  mps_context_get_roots_d(s, &results, NULL);

  /* Print out roots */
  for (int i = 0; i < degree; i++) {
	cout << cplx_Re(results[i]) << " + " << cplx_Im(results[i]) << "i" << endl;
	//	cplx_out_str (stdout, results[i]);
	//	printf ("\n");
  }

  // Free memory
  free(results);
  return 0;
}

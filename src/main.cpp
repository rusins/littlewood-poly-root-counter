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
  mps_monomial_poly *p;
  mps_context *s;
  
  mpq_t one, m_one, zero;

  mpq_init (one);
  mpq_init (m_one);
  mpq_init (zero);

  mpq_set_si (one, 1, 1);
  mpq_set_si (m_one, -1, 1);
  mpq_set_si (zero, 0, 1);

  int n = 5;
  s = mps_context_new ();
  p = mps_monomial_poly_new (s, n);

  mps_context_select_algorithm(s, MPS_ALGORITHM_SECULAR_GA);

  mps_monomial_poly_set_coefficient_d (s, p, 0, 1, 0); 
  mps_monomial_poly_set_coefficient_d (s, p, n, -1, 0); 

  /* Set the input polynomial */
  mps_context_set_input_poly (s, MPS_POLYNOMIAL (p));

  /* Allocate space to hold the results. We check only floating point results
   * in here */
  cplx_t *results = cplx_valloc (n);

  /* Actually solve the polynomial */
  mps_mpsolve (s);

  /* Save roots computed in the vector results */
  mps_context_get_roots_d (s, &results, NULL);

  /* Print out roots */
  for (int i = 0; i < n; i++)
    {
      cplx_out_str (stdout, results[i]);
      printf ("\n");
    }

  free (results);
  return 0;
}

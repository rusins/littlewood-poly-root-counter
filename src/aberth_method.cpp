#include <gsl/gsl_complex.h>

#include <mps/mps.h>

class aberth_method {
public:
  aberth_method(int degree);
  gsl_complex *solve(double *coefficients);
  ~aberth_method();

private:
  int degree;
  mps_context *context;
  mps_monomial_poly *poly;
  cplx_t *results;
  gsl_complex *roots;
};

aberth_method::aberth_method(int degree): degree(degree) {
  context = mps_context_new();
  poly = mps_monomial_poly_new(context, degree);
  mps_context_select_algorithm(context, MPS_ALGORITHM_STANDARD_MPSOLVE);
  results = cplx_valloc(degree);
  roots = new gsl_complex[degree];
}

aberth_method::~aberth_method() {
  mps_monomial_poly_free(context, MPS_POLYNOMIAL(poly));
  mps_context_free(context);
  free(results);
  delete roots;
}

// Uses the Aberth method to find roots of the supplied polynomial.
// coefficients array is from smallest power to largest
// The coefficient array should have size degree + 1
// Returns array of roots of size degree, which gets deleted on deconstruction
gsl_complex *aberth_method::solve(double *coefficients) {
  // Set the coefficients
  for (int i = 0; i < degree + 1; ++i)
	mps_monomial_poly_set_coefficient_d (context, poly, i, coefficients[i], 0);

  // Set the input polynomial
  mps_context_set_input_poly(context, MPS_POLYNOMIAL(poly));

  // Solve the polynomial
  mps_mpsolve(context);

  // Save roots computed in the vector results
  mps_context_get_roots_d(context, &results, NULL);

  for (int i = 0; i < degree; ++i)
	GSL_SET_COMPLEX(&roots[i], cplx_Re(results[i]), cplx_Im(results[i]));
  
  return roots;
}

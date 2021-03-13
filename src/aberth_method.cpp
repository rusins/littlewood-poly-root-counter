#pragma once

#include <vector>
#include <gsl/gsl_complex.h>
#include <mps/mps.h>
#include <math.h>

#include "poly_solver.cpp"

const int DEFAULT_PRECISION = 12;

class aberth_method : public poly_solver {
public:
  aberth_method(int degree);
  vector<gsl_complex> solve(double *coefficients);
  vector<gsl_complex> solve(double *coefficients, double precision);
  ~aberth_method();

private:
  int degree;
  mps_context *context;
  mps_monomial_poly *poly;
  cplx_t *results;
  vector<gsl_complex> continue_solve(double *coefficients);
};






aberth_method::aberth_method(int degree_param): degree(degree_param) {
}

aberth_method::~aberth_method() {
}

// Uses the Aberth method to find roots of the supplied polynomial.
// coefficients array is from smallest power to largest
// The coefficient array should have size degree + 1
// Returns array of roots of size degree, which gets deleted on deconstruction
vector<gsl_complex> aberth_method::solve(double *coefficients) {
  // MPSolve encounters address errors if we don't recreate the context every time
  return solve(coefficients, DEFAULT_PRECISION * log2(10));
}

vector<gsl_complex> aberth_method::solve(double *coefficients, double precision) {
  // MPSolve encounters address errors if we don't recreate the context every time
  context = mps_context_new();
  poly = mps_monomial_poly_new(context, degree);
  mps_context_select_algorithm(context, MPS_ALGORITHM_STANDARD_MPSOLVE);
  mps_context_set_output_prec(context, precision + 1);
  results = cplx_valloc(degree);
  
  // Set the coefficients
  for (int i = 0; i < degree + 1; ++i)
	mps_monomial_poly_set_coefficient_d(context, poly, i, coefficients[i], 0);

  // Set the input polynomial
  mps_context_set_input_poly(context, MPS_POLYNOMIAL(poly));

  // Solve the polynomial
  mps_mpsolve(context);

  // Save roots computed in the vector results
  mps_context_get_roots_d(context, &results, NULL);

  vector<gsl_complex> roots(degree);
  for (int i = 0; i < degree; ++i)
	GSL_SET_COMPLEX(&roots[i], cplx_Re(results[i]), cplx_Im(results[i]));

  mps_monomial_poly_free(context, MPS_POLYNOMIAL(poly));
  mps_context_free(context);
  cplx_vfree(results);
  
  //  return vector<gsl_complex>(roots, roots + degree);
  return roots;
}

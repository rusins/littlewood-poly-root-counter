#pragma once

#include <gsl/gsl_poly.h>
#include <gsl/gsl_complex.h>

#include "poly_solver.cpp"

using namespace std;

class qr_algorithm : public poly_solver {
public:
  qr_algorithm(int degree);
  gsl_complex *solve(double *coefficients);
  ~qr_algorithm();

private:
  int degree;
  gsl_poly_complex_workspace *workspace;
  gsl_complex *roots;
};






qr_algorithm::qr_algorithm(int degree_param): degree(degree_param) {
  workspace = gsl_poly_complex_workspace_alloc(degree + 1);
  roots = new gsl_complex[degree];
}

qr_algorithm::~qr_algorithm() {
  gsl_poly_complex_workspace_free(workspace);
  delete roots;
}

// Uses the QR algorithm to find roots of the supplied polynomial.
// coefficients array is from smallest power to largest
// The coefficient array should have size degree + 1
// Returns array of roots of size degree, which gets deleted on deconstruction
gsl_complex *qr_algorithm::solve(double *coefficients) {
  int res = gsl_poly_complex_solve(coefficients, degree + 1, workspace, (double*) roots);
  //  if (res == GSL_FAILED)
  //	cerr << "QR reduction didn't converge!" << endl;
  return roots;
}

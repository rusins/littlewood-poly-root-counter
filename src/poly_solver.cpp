#pragma once

#include <gsl/gsl_complex.h>

using namespace std;

class poly_solver {
  public:
  virtual gsl_complex *solve(double *coefficients) = 0;
};

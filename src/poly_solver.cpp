#pragma once

#include <vector>
#include <gsl/gsl_complex.h>

using namespace std;

class poly_solver {
  public:
  virtual vector<gsl_complex> solve(double *coefficients) = 0;
};

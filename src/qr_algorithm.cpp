#include <gsl/gsl_poly.h>
#include <gsl/gsl_complex.h>

class qr_algorithm {
public:
  qr_algorithm(int degree);
  gsl_complex *solve(double *coefficients);
  ~qr_algorithm();

private:
  int degree;
  gsl_poly_complex_workspace *workspace;
  gsl_complex *roots;
};

qr_algorithm::qr_algorithm(int degree): degree(degree) {
  workspace = gsl_poly_complex_workspace_alloc(degree + 1);
  roots = new gsl_complex[degree];
}

qr_algorithm::~qr_algorithm() {
  gsl_poly_complex_workspace_free(workspace);
  delete(roots);
}

// Uses the QR algorithm to find roots of the supplied polynomial.
// coefficients array is from smallest power to largest
// The coefficient array should have size degree + 1
// Returns array of roots of size degree
gsl_complex *qr_algorithm::solve(double *coefficients) {
  gsl_poly_complex_solve(coefficients, degree + 1, workspace, (double*) roots);
  return roots;
}

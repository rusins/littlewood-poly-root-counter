#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>

#include "qr_algorithm.cpp"
#include "aberth_method.cpp"
#include "utils.cpp"

using namespace std;


int main(int argc, char **argv) {
  if (argc < 2) {
	cerr << "Enter the degree for polynomials to generate!" << endl;
	return -1;
  }

  int degree;
  try {
	degree = atoi(argv[1]);
  } catch (logic_error e) {
	cerr << "Unable to parse integer argument: " << argv[1] << endl;
	return -1;
  }

  double coefficients[degree + 1];
  
  // TODO: wrap in try block in case OOM
  qr_algorithm qr(degree);
  aberth_method ab(degree);
  
  while (true) {
	// Generate random coefficients
	for (int i = 0; i < degree + 1; ++i) {
	  coefficients[i] = (rand() % 2) * 2 - 1;
	}
	gsl_complex *qr_roots = qr.solve(coefficients);
	gsl_complex *ab_roots = ab.solve(coefficients);

	graph g = build_graph(qr_roots, ab_roots, degree);
	point_pairs pairs = find_pairs(g);
	
  }

  return 0;
}

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
  } catch (logic_error &e) {
	cerr << "Unable to parse integer argument: " << argv[1] << endl;
	return -1;
  }

  double *coefficients = new double[degree + 1];
  
  // TODO: wrap in try block in case OOM
  qr_algorithm qr(degree);
  aberth_method ab(degree);

  int count = 0;
  while (true) {
	// Generate random coefficients
	for (int i = 0; i < degree + 1; ++i) {
	  coefficients[i] = (rand() % 2) * 2 - 1;
	}
	
	vector<gsl_complex> qr_roots = qr.solve(coefficients);
	vector<gsl_complex> ab_roots = ab.solve(coefficients);

	graph g = build_graph(qr_roots, ab_roots);
	cout << "Working..." << endl;
	point_pairs pairs = start_find_pairs(g);
	cout << "Succeeded! :D " << ++count << endl;
	/*
	for (auto p: pairs) {
	  cout << gsl_complex_ops::print(p.first) << " <---> " << gsl_complex_ops::print(p.second) << "\n";
	}
	cout << endl;
	*/
  }


  delete[] coefficients;
  return 0;
}

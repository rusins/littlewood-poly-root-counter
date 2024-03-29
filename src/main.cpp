#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>

#include "qr_algorithm.cpp"
#include "aberth_method.cpp"
#include "utils.cpp"
#include "cxxopts.hpp"

using namespace std;


const double PRECISION = log2(10) * 12;

bool in_area(const gsl_complex &root, int degree) {
  const gsl_complex ONE = {1.0, 0.0};
  const double PRECISION_DIST = 1.0 / pow(2, PRECISION);
  double radius = 1.0 / sqrt(degree);
  double dist = gsl_complex_ops::distance(root, ONE);
  return dist - PRECISION_DIST < radius;
}

int run(aberth_method &ab, int degree, double *coefficients, bool mirror_roots) {
  vector<gsl_complex> ab_roots = ab.solve(coefficients, PRECISION);
  int count = 0;
  for (auto root: ab_roots) {
	if (in_area(root, degree))
	  ++count;
	if (mirror_roots) {
	  gsl_complex mirror = {-GSL_REAL(root), GSL_IMAG(root)};
	  if (in_area(mirror, degree))
		++count;
	}
  }
  return count;
}

void run_random(int degree) {
  const int iterations = 5000;
  ofstream file(to_string(degree) + "-rand.txt", ios::trunc);
  long long total_count = 0;

  double *coefficients = new double[degree + 1];
  
  aberth_method ab(degree);

  time_t last_update_time = time(NULL);
  for (int iter = 0; iter < iterations; ++iter) {
	if (time(NULL) - last_update_time >= 5) {
	  cout << iter * 100 / iterations << "%" << endl;
	  last_update_time = time(NULL);
	}

	
	// Generate random coefficients
	for (int i = 0; i < degree + 1; ++i) {
	  coefficients[i] = (rand() % 2) * 2 - 1;
	}

	int count = run(ab, degree, coefficients, true);
	file << count << "\n";
	total_count += count;
  }
  
  cout << degree << " " << total_count << endl;
  file.close();
  delete[] coefficients;  
}

void run_seq(int degree) {
  long long iterations = 1L << (degree + 1);
  ofstream file(to_string(degree) + "-all.txt", ios::trunc);
  long long total_count = 0;

  double *coefficients = new double[degree + 1];
  
  aberth_method ab(degree);

  time_t last_update_time = time(NULL);
  for (long long iter = 0; iter < iterations; ++iter) {
	if (time(NULL) - last_update_time >= 5) {
	  //	  cout << iter * 100 / iterations << "%" << endl;
	  last_update_time = time(NULL);
	}

	
	// Extract coefficients
	for (int i = 0; i < degree + 1; ++i) {
	  coefficients[i] = ((iter >> i) % 2) * 2 - 1;
	}

	int count = run(ab, degree, coefficients, false);
	file << count << "\n";
	total_count += count;

  }
  
  cout << degree << " " << total_count << endl;
  file.close();
  delete[] coefficients;
}

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

  while (true) {
	//	cout << "Running random on degree " << degree << endl;
	if (degree < 16) {
	  run_seq(degree);
	}  else {
	  run_random(degree);
	}
	++degree;
  }
  
  return 0;
}

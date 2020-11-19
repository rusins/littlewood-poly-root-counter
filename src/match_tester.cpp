#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>


#include "qr_algorithm.cpp"
#include "aberth_method.cpp"

using namespace std;

const double ETA = 0.005;

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

	int min_edges = degree, max_edges = 0;
  
  while (true) {
	// Generate random coefficients
	for (int i = 0; i < degree + 1; ++i) {
	  coefficients[i] = (rand() % 2) * 2 - 1;
	}
	gsl_complex *qr_roots = qr.solve(coefficients);
	gsl_complex *ab_roots = ab.solve(coefficients);

	// Generate graph

	// even vertices are qr, odd are ab
	vector<int> edges[degree * 2];
	bool visited[degree * 2];
	for (int i = 0; i < degree * 2; ++i) {
	  visited[i] = false;
	  for (int j = 0; j < degree; ++j) {
		double dr = GSL_REAL(qr_roots[i]) - GSL_REAL(ab_roots[j]);
		double di = GSL_IMAG(qr_roots[i]) - GSL_IMAG(ab_roots[j]);
		double dist = sqrt(dr*dr + di*di);
		if (dist < ETA) {
		  edges[i * 2].push_back(j * 2 + 1);
		  edges[j * 2 + 1].push_back(i * 2);
		}
	  }
	}

	/*
	// Cut leaves
	for (int i = 0; i < degree * 2; ++i) {
	if (visisted[i])
	continue;
	if (edges[i].empty) {
	cerr << "Root had no match! " << qr_roots[i] << endl;
	break;
	}
	if (edges[i].size == 1) {
	// Cut leaves
	visisted[i] = true;
	visisted[edges[i][0]] = true;
		
	}
	}
	*/
	for (int i = 0; i < degree; ++i) {
	  if (edges[i * 2].size() > max_edges)
		max_edges = edges[i * 2].size();
	  if (edges[i * 2].size() < min_edges)
		min_edges = edges[i * 2].size();
	}
	if (min_edges == 0) {
	  cout << "0 edges!" << endl;
	  cout << "QR Roots ---------" << endl;
	  for (int i = 0; i < degree; ++i)
		cout << GSL_REAL(qr_roots[i]) << " + " << GSL_IMAG(qr_roots[i]) << "i" << endl;

	  cout << "Aberth Roots ---------" << endl;
	  for (int i = 0; i < degree; ++i)
		cout << GSL_REAL(ab_roots[i]) << " + " << GSL_IMAG(ab_roots[i]) << "i" << endl;
	  break;
	} else {
	  cout << "Min: " << min_edges << " Max: " << max_edges << endl;
	}
  }

  return 0;
}

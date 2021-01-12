#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "aberth_method.cpp"
#include "qr_algorithm.cpp"
#include "poly_solver.cpp"
#include "utils.cpp"

using namespace std;

#define APPROX_EQ(val1, val2) EXPECT_LE(abs((val1) - (val2)), ETA) << "Left value: " << (val1) << " Right value: " << (val2) << "\n"

void simple_test(poly_solver &solver) {
  double coefficients[3] = {1.0, 0.0, 1.0};
  vector<gsl_complex> roots = solver.solve(coefficients);

  // Both roots should have 0 as the real part
  APPROX_EQ(GSL_REAL(roots[0]), 0.0) << "First root's real part was not 0";
  APPROX_EQ(GSL_REAL(roots[1]), 0.0) << "Second root's real part was not 0";

  // One should be +i, the other -i
  double a = min(GSL_IMAG(roots[0]), GSL_IMAG(roots[1]));
  double b = max(GSL_IMAG(roots[0]), GSL_IMAG(roots[1]));
  APPROX_EQ(a, -1.0) << "Imaginary part was not -1";
  APPROX_EQ(b, 1.0) << "Imaginary part was not +1";
}

TEST(aberth_method, SolvesSimplePolynomials) {
  aberth_method ab(2);
  simple_test(ab);
}

TEST(qr_algorithm, SolvesSimplePolynomials) {
  qr_algorithm qr(2);
  simple_test(qr);
}

void exact_match_test(vector<gsl_complex> a, vector<gsl_complex> b) {
  int n = a.size();
  graph g = build_graph(a, b);
	
  for (int i = 0; i < 2*n; ++i) {
	gsl_complex x = g.values[i];
	ASSERT_EQ(g.edges[i].size(), 1) <<
	  "Node " << gsl_complex_ops::print(x) << " does not have an exact match!";
	gsl_complex y = g.values[g.edges[i][0]];
	EXPECT_LE(gsl_complex_ops::distance(x, y), ETA) << "Node is matched with a differently valued node! \n" <<
	  "Left node: " << gsl_complex_ops::print(x) <<
	  " Right node: " << gsl_complex_ops::print(y) << "\n";
  }
}

TEST(build_graph, CorrectlyMatchesRealRoots) {
  vector<gsl_complex> a{{1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}};
  vector<gsl_complex> b{{3.0, 0.0}, {2.0, 0.0}, {1.0, 0.0}};
  exact_match_test(a, b);
}

TEST(build_graph, CorrectlyMatchesImaginaryRoots) {
  vector<gsl_complex> a{{-1.0, 0.0}, {0.5, -0.9 - ETA / 2}, {0.0, -0.9}, {0.0, -1.0}}; 
 vector<gsl_complex> b{{-1.0, 0.0}, {0.0, -0.9 - ETA * 4 / 5}, {0.5, -0.9}, {0.0, -1.0}};
  exact_match_test(a, b);
}

TEST(build_graph, CorrectlyMatchesComplexRoots) {
  vector<gsl_complex> a{{-1.0, 0.0},
						{0.0 - ETA / 4, 0.5},
						{0.0, -0.5},
						{0.0 + ETA / 4, 0.0},
						{0.5 + ETA / 6, -0.3},
						{0.5 + ETA / 4, 0.3}};
  
  vector<gsl_complex> b{{-1.0 + ETA * 4 / 5, 0.0},
						{0.0 - ETA / 6, 0.0},
						{0.0, 0.5},
						{0.0 + ETA / 6, -0.5},
						{0.5 - ETA / 4, 0.3},
						{0.5 - ETA / 6, -0.3}};
  exact_match_test(a, b);
}

TEST(build_graph, LeavesNoNodeUnmatched) {
  const int MAX_DEGREE = 1000;
  for (int ITERATIONS = 10; ITERATIONS > 0; --ITERATIONS) {
	int degree = (rand() % MAX_DEGREE) + 1;
	double *coefficients = new double[degree + 1];
	for (int i = 0; i < degree + 1; ++i)
	  coefficients[i] = (rand() % 2) * 2 - 1;
	
	aberth_method ab(degree);
	qr_algorithm qr(degree);
	vector<gsl_complex> roots_a = ab.solve(coefficients);
	vector<gsl_complex> roots_b = qr.solve(coefficients);
	graph g = build_graph(roots_a, roots_b);
	vector<bool> visited(degree * 2);
	fill(visited.begin(), visited.end(), false);
	for (int i = 0; i < degree * 2; ++i) {
	  if (visited[i]) continue;
	  stack<int> next;
	  next.push(i);
	  int count = 0;
	  while (!next.empty()) {
		int node = next.top();
		next.pop();
		if (visited[node]) continue;
		visited[node] = true;
		count++;
		for (int adj: g.edges[node])
		  if (!visited[adj])
			next.push(adj);
	  }
	  if (count % 2 == 1) {
		cout << "\n";
		cout << "Polynomial: ";
		for (int j = 0; j < degree + 1; ++j)
		  cout << coefficients[j] << " ";
		cout << "\n" << g.print() << endl;
	  }
	  ASSERT_TRUE(count % 2 == 0) << "Found a connected subset of the graph with odd size!";
	}
	
	delete[] coefficients;
  }
}

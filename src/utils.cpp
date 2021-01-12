#pragma once

#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>

using namespace std;

#define point_pairs vector< pair<gsl_complex, gsl_complex> >

// Max distance between root pairs
const double ETA = 0.01;

struct gsl_complex_ops {
  static bool compare_real(const gsl_complex &a, const gsl_complex &b) {
	return GSL_REAL(a) < GSL_REAL(b);
  }

  static string print(const gsl_complex &val) {
	return to_string(GSL_REAL(val)) + " " + to_string(GSL_IMAG(val)) + "i";
  }

  static double distance(const gsl_complex &a, const gsl_complex &b) {
	const double dr = GSL_REAL(a) - GSL_REAL(b);
	const double di = GSL_IMAG(a) - GSL_IMAG(b);
	return sqrt(dr*dr + di*di);
  }
};

struct graph {
  vector< vector<int> > edges; // For each node a list of other nodes it is connected to
  vector<gsl_complex> values;
  int size;

  graph(int size_param): size(size_param) {
	edges.resize(size);
	values.resize(size);
  }

  string print() const {
	ostringstream builder;
	builder << "\nAll roots:\n";
	for (int j = 0; j < size; ++j)
	  builder << j << ": " << gsl_complex_ops::print(values[j]) << "\n";
	builder << "\nAll edges:\n";
	for (int j = 0; j < size; j += 2)
	  for (int z : edges[j])
		builder << j << "-" << z << "\n";
	builder << "\n";
	return builder.str();
  }
};


// Matches the roots outputted by both algorithms and builds a graph.
// @param n is the length of the passed arrays.
// @return array of vector of edges that makes up the graph, of length n*2.
// roots_a maps to nodes with even numbers, roots_b maps to nodes with odd numbers.
graph build_graph(vector<gsl_complex> roots_a, vector<gsl_complex> roots_b) {
  int n = roots_a.size();
  graph g(2*n);
  // TODO: Instead of sorting in only 1 direction, we can sort in both directions
  // to avoid congestion where the roots crowd at the vertical parts of the unit circle.
  
  // Sort roots
  sort(roots_a.begin(), roots_a.end(), gsl_complex_ops::compare_real);
  sort(roots_b.begin(), roots_b.end(), gsl_complex_ops::compare_real);

  // a roots will be x2, and b nodes x2 + 1 in the graph
  for (int i = 0; i < n; ++i) {
	g.values[i*2] = roots_a[i];
	g.values[i*2 + 1] = roots_b[i];
  }

  // 2 pointer iteration through the arrays.
  // Worst case O(n^2), but should be closer to O(n) in practice
  int a = 0;
  double max_dist = 0.0;
  for (int b = 0; a < n && b < n; ++b) {
	const double real_b = GSL_REAL(roots_b[b]);
	double min_dist = 20.0;
	
	for (int aa = a; aa < n; ++aa) {
	  const double real_a = GSL_REAL(roots_a[aa]);
	  if (real_a < real_b - ETA) {
		++a; // Move first pointer
	  } else if (real_a > real_b + ETA) {
		break; // Gone too far
	  } else {
		const double dist = gsl_complex_ops::distance(roots_a[aa], roots_b[b]);
		if (dist <= ETA) {
		  if (dist < min_dist)
			min_dist = dist;
		  // Add edge to graph
		  const int a_node = aa * 2;
		  const int b_node = b * 2 + 1;
		  g.edges[a_node].push_back(b_node);
		  g.edges[b_node].push_back(a_node);
		}
	  }
	}
	if (min_dist > max_dist)
	  max_dist = min_dist;	
  }

  if (max_dist > 0.001)
	cout << max_dist << endl;

  return g;
}

class non_matching_roots_exception : public exception {
private:
  const graph &g;
  ostringstream builder;
  string msg;

public:
  non_matching_roots_exception(const graph &g_param): g(g_param) {
	builder << "The two root finding algorithms produced roots that do not agree with one another!\n";
	builder << g.print();
	msg = builder.str();
  };
  const char *what() const throw() {
	return msg.c_str();
  }
};



// Finds a pairing between the roots from a graph.
// @throws non_matching_roots_exception if a pairing does not exist!
point_pairs find_pairs(graph &g, int n, vector<bool> visited, vector<int> edge_count) {
  point_pairs pairs;

  // Step 1 – cut all the leaves
  stack<int> leaves;
  for (int i = 0; i < n; ++i) {
	if (edge_count[i] == 1)
	  leaves.push(i);
  }

  while (!leaves.empty()) {
	int node = leaves.top();
	leaves.pop();

	if (visited[node]) continue;
	if (edge_count[node] == 0) {
	  cout << "wot" << endl;
	  throw non_matching_roots_exception(g);
	}
	// From this point onward we know edge_count[node] = 1

	int other;
	for (int o : g.edges[node]) {
	  if (!visited[o]) {
		other = o;
		break; // Found the match
	  }
	}

	if (edge_count[other] == 0){
	  cout << "fuck" << endl;
	  throw non_matching_roots_exception(g);
	}

	// Update state
	visited[node] = true;
	visited[other] = true;
	--edge_count[other];
	for (int adj : g.edges[other]) {
	  --edge_count[adj];
	  if (edge_count[adj] == 1)
		leaves.push(adj); // Found new leaf
	}
	// We don't update neighbours for node, because it was already a leaf, so its adjacent
	// nodes are already visited

	if (node % 2 == 1)
	  swap(node, other); 
	pairs.push_back(make_pair(g.values[node], g.values[other]));
  }

  // Step 2 – brute force all the remaining connected graphs.
  for (int node = 0; node < n; ++node) {
	if (visited[node])
	  continue;
	visited[node] = true;
	if (edge_count[node] == 0) {
	  cout << "ugh oh" << endl;
	  throw non_matching_roots_exception(g);
	}
	
	// have node
	// find all nodes unvisited
	for (int other: g.edges[node]) {
	  if (visited[other]) continue;
	  visited[other] = true;
	  vector<int> new_edge_count = edge_count;
	  for (int adj: g.edges[node])
		new_edge_count[adj]--;
	  for (int adj: g.edges[other])
		new_edge_count[adj]--;
	  try {
		point_pairs result = find_pairs(g, n, visited, new_edge_count);
		result.insert(result.end(), pairs.begin(), pairs.end());
		return result;
	  } catch (non_matching_roots_exception &e) {
		visited[other] = false;
		continue; // This wasn't it chief
	  }
	}
	cout << "Failed to create a graph by matching root " << node << endl;
	throw new non_matching_roots_exception(g);
  }

  // Every node visited successfully
  return pairs;
}

point_pairs start_find_pairs(graph &g) {
  int n = g.values.size();
  vector<bool> visited(n);
  fill(visited.begin(), visited.end(), false);
  vector<int> edge_count(n);
  for (int i = 0; i < n; ++i) {
	edge_count[i] = g.edges[i].size();
	if (edge_count[i] == 0) {
	  cout << "back to the drawing board" << endl;
	  throw non_matching_roots_exception(g);
	}
  }
  return find_pairs(g, n, visited, edge_count);
}

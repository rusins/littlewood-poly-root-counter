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
graph build_graph(gsl_complex *roots_a, gsl_complex *roots_b, int n) {

  graph g(2*n);
  // TODO: Instead of sorting in only 1 direction, we can sort in both directions
  // to avoid congestion where the roots crowd at the vertical parts of the unit circle.
  
  // Sort roots
  sort(roots_a, roots_a + n, gsl_complex_ops::compare_real);
  sort(roots_b, roots_b + n, gsl_complex_ops::compare_real);

  // a roots will be x2, and b nodes x2 + 1 in the graph
  for (int i = 0; i < n; ++i) {
	g.values[i*2] = roots_a[i];
	g.values[i*2 + 1] = roots_b[i];
  }

  // 2 pointer iteration through the arrays.
  // Worst case O(n^2), but should be closer to O(n) in practice
  int a = 0;
  for (int b = 0; a < n && b < n; ++b) {
	const double real_b = GSL_REAL(roots_b[b]);
	
	for (int aa = a; aa < n; ++aa) {
	  const double real_a = GSL_REAL(roots_a[aa]);
	  if (real_a < real_b - ETA) {
		++a; // Move first pointer
	  } else if (real_a > real_b + ETA) {
		break; // Gone too far
	  } else {
		const double dist = gsl_complex_ops::distance(roots_a[aa], roots_b[b]);
		if (dist <= ETA) {
		  // Add edge to graph
		  const int a_node = aa * 2;
		  const int b_node = b * 2 + 1;
		  g.edges[a_node].push_back(b_node);
		  g.edges[b_node].push_back(a_node);
		}
	  }
	}
  }

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
point_pairs find_pairs(graph &g) {
  //  cout << "\n\n";
  int n = g.values.size();
  point_pairs pairs(n / 2);
  
  bool visited[n] = {true};
  cout << "visited\n" ;
  for (int i = 0; i < n; ++i)
	cout << "v: " << visited[i] << "\n";

  // Step 1 – cut all the leaves
  int edge_count[n]; // number of active edges each node has. Used for efficient leaf cutting.
  stack<int> leaves;
  for (int i = 0; i < n; ++i) {
	//visited[i] = 0;
	edge_count[i] = g.edges[i].size();
	if (edge_count[i] == 0) {
	  cout << "back to the drawing board" << endl;
	  throw non_matching_roots_exception(g);
	}
	else if (edge_count[i] == 1)
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
	//	cout << "leaf "<< node << "\n";
	//	cout << "leaf "<< other << "\n";
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
  int maxcount = 0;
  for (int i = 0; i < n; ++i) {
	if (visited[i])
	  continue;
	if (edge_count[i] == 0) {
	  cout << "ugh oh" << endl;
	  throw non_matching_roots_exception(g);
	}

	stack<int> nodes;
	stack<int> k;
	nodes.push(i);
	k.push(i);
	int count = 0;
	while (!nodes.empty()) {
	  int node = nodes.top();
	  nodes.pop();
	  if (visited[node])
		continue;
	  visited[node] = true;

	  count++;

	  for (int next: g.edges[node]) {
		if (!visited[next]) {
		  nodes.push(next);
		  k.push(next);
		}
	  }
	}
	if (count % 2 == 1) {
	  cout << "bad: " << count << endl;
	  cout << "i: "<< i << endl;
	  while (!k.empty()) {
		int kk = k.top();
		k.pop();
		cout << GSL_REAL(g.values[kk]) << " " << GSL_IMAG(g.values[kk]) << " " << kk << "\n";
	  }
	  throw non_matching_roots_exception(g);
	}
	if (count > maxcount)
	  maxcount = count;
	// have node
	// find all nodes unvisited
	// for each node run func:
	// func picks a random edge, then finds all leaves, and pairs them up and keeps note
	// func then gathers all adjacent nodes unvisited
	// func then runs itself on one of the non leaf nodes. If fail, then revert visisted state the function did and fail upwards
	// if not fail, then keep going through unvisited adj. nodes and running the function
	//
	// this is actually similar to what we already did, so should do a clever refactor :D
  }
  if (maxcount > 0)
	cout << "count: "<< maxcount << endl;

  return pairs;
}

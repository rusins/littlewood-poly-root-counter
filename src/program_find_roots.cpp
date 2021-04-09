#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>
#include <thread>
#include <mutex>
#include <math.h>

#include "qr_algorithm.cpp"
#include "aberth_method.cpp"
#include "utils.cpp"
#include "cxxopts.hpp"

using namespace std;

string print_coefficients(double *coefficients, int degree) {
  ostringstream builder;
  for (int i = degree; i > 0; --i) {
    int c = (int) coefficients[i];
    if (c > 0)
      builder << "+";
    builder << c << "x^" << i << " ";
  }
  int c = (int) coefficients[0];
  if (c > 0)
    builder << "+1";
  else
    builder << "-1";
  return builder.str();
}


class sync_obj {
private:
  int degree;
  long count = 0;
  long total;
  int precision;
  mutex mtx; // Mutex for writing to the file and incrementing the count
  time_t last_update_time = time(NULL);
public:
  ofstream file;
  
  sync_obj(int degree_param, long total_param, string file_name, int precision_param): degree(degree_param), total(total_param), precision(precision_param) {
    file = ofstream(file_name, ios::trunc);
  }

  ~sync_obj() {
    file.close();
  }

  void add_result(double *coefficients, const vector<gsl_complex> &roots) {

    file << "# " << print_coefficients(coefficients, degree) << "\n";
    for (int i = 0; i < degree; ++i)
      file << gsl_complex_ops::print(roots[i], precision) << "\n";

    count++;
    
    if (time(NULL) - last_update_time >= 10) {
      cout << count * 100 / total << "%" << endl;
      last_update_time = time(NULL);
    }
  }

  // Called if polynomial skipped
  void inc_count() {
    count++;
  }
};

void solve_and_print(int degree, double *coefficients, aberth_method &ab, qr_algorithm &qr, sync_obj *&sync, const int precision) {
  auto roots_ab = ab.solve(coefficients, precision * log2(10));
  auto roots_qr = qr.solve(coefficients);

  bool skip_polynomial = false;

  graph g = build_graph(roots_ab, roots_qr);
  try {
    start_find_pairs(g);
  } catch (non_matching_roots_exception &e) {
    cerr << "The root matching algorithms disagreed! Skipping polynomial " << print_coefficients(coefficients, degree) << endl;
    skip_polynomial = true;
  }

  if (!skip_polynomial) {
    sync->add_result(coefficients, roots_ab);
  } else {
    sync->inc_count();
  }
}

void run_thread_rand(const int degree, const int iterations, sync_obj *sync, const int precision) {
  double *coefficients = new double[degree + 1];
  aberth_method ab(degree);
  qr_algorithm qr(degree);
  for (int iter = 0; iter < iterations; ++iter) {
    for (int i = 0; i < degree + 1; ++i) {
      coefficients[i] = (rand() % 2) * 2 - 1;
    }
    solve_and_print(degree, coefficients, ab, qr, sync, precision);
  }
  delete[] coefficients;
}

void run_thread_iter(const int degree, const long start, const long step, const long end, sync_obj *sync, const int precision) {
  double *coefficients = new double[degree + 1];
  aberth_method ab(degree);
  qr_algorithm qr(degree);

  // Extract coefficients
  for (long iter = start; iter < end; iter += step) {
    for (int i = 0; i < degree + 1; ++i) {
      coefficients[i] = ((iter >> i) % 2) * 2 - 1;
    }
    solve_and_print(degree, coefficients, ab, qr, sync, precision);
  }
  delete[] coefficients;
}

void run_program(const pair<int, int> degree_range, long iterations, const int thread_count, const int precision) {
  bool random = iterations != 0;
  
  for (int degree = degree_range.first; degree <= degree_range.second; ++degree) {
    cout << "Processing polynomials of degree " << degree << endl;
    if (!random)
      iterations = 1L << (degree + 1);
    vector<thread> threads(thread_count);
    
    sync_obj sync(degree, iterations, to_string(degree) + "-roots.txt", precision); // Mutex around file and processed poly count
    if (!random) {
      sync.file << "# Roots of all Littlewood polynomials of degree " << degree << "\n";
      for (int i = 0; i < thread_count; ++i) {
        //threads[i] = thread(run_thread_iter, degree, i, thread_count, iterations, &sync);
        run_thread_iter(degree, i, thread_count, iterations, &sync, precision);
      }
    } else {
      sync.file << "# Roots of " << iterations << " randomly generated Littlewood polynomials of degree " << degree << "\n";
      // Divide iterations as equally as possible
      int div = iterations / thread_count;
      long rem = iterations - div * thread_count;
      for (int i = 0; i < thread_count; ++i) {
        int share = div;
        if (rem-- > 0)
          share++;
        //threads[i] = thread(run_thread_rand, degree, share, &sync);
        run_thread_rand(degree, share, &sync, precision);
      }
    }

    // Join all threads
    //    for (int i = 0; i < thread_count; ++i)
    //      threads[i].join();
  }

}

void option_parse_fail(const cxxopts::Options &options, string msg) {
  cout << msg << options.help();
  exit(1);
}

// Parses command line arguments and calls run_program()
int main(int argc, char **argv) {
  
  cxxopts::Options options("find-roots", "");
  options.add_options()
    ("d,degree", "Degree for the polynomials to find roots for. A range of degrees can be given by separating the start and end with a dash, e.g. 2-12", cxxopts::value<string>())
    ("a,all", "Iterate through all possible polynomials", cxxopts::value<bool>()->default_value("false"))
    ("i,iterations", "Number of polynomials to solve per degree.", cxxopts::value<int>()->default_value("10000"))
    ("t,threads", "Number of threads to solve polynomials with.", cxxopts::value<int>()->default_value("1"))
    ("p,precision", "Number of decimal places for the floating point values.", cxxopts::value<int>()->default_value("12"))
    ("h,help", "Print usage")
    ;

  try {
    auto result = options.parse(argc, argv);
    
    if (result.count("help")) {
      cout << "Solves randomly generated Littlewood polynomials and saves their roots to a file.";
      cout << options.help() << endl;
      exit(0);
    }

    // Read degree range
    if (!result.count("degree")) {
      option_parse_fail(options, "Must specify degree!");
    }
    string degree_string = result["degree"].as<string>();
    vector<string> parsed_degree = split_string(degree_string, '-');
    if (parsed_degree.size() > 2) {
      option_parse_fail(options, "Unable to parse degree!");
    }
    pair<int, int> degree_range;
    try {
      degree_range.first = stoi(parsed_degree[0]);
      if (parsed_degree.size() == 2) {
        degree_range.second = stoi(parsed_degree[1]);
      } else {
        degree_range.second = degree_range.first;
      }
    } catch (invalid_argument &e) {
      option_parse_fail(options, "Failed to parse degree!");
    } catch (out_of_range &e) {
      option_parse_fail(options, "Degree too large!");
    }

    // Read iterations
    if (!result.count("all") && !result.count("iterations")) {
      option_parse_fail(options, "Must supply either iteration count or all flag!");
    }
    int iterations = 0; // 0 means all
    if (result.count("iterations")) {
      iterations = result["iterations"].as<int>();
    }

    // Read thread count
    int thread_count = result["threads"].as<int>();
    if (thread_count <= 0) {
      option_parse_fail(options, "Thread count must be greater than or equal to 1!");
    }
    // Read precision
    int precision = result["precision"].as<int>();

    run_program(degree_range, iterations, thread_count, precision);
    
  } catch (cxxopts::OptionParseException &e) {
    option_parse_fail(options, e.what());
  }
}

#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>
#include <optional>
#include <thread>
#include <mutex>

#include "utils.cpp"
#include "cxxopts.hpp"

using namespace std;

const int PRECISION = 12;

mutex mtx;
int first_degree;
int degree_cnt;
int result_pos = 0;
vector<double> *results;

// Mutex guarded function to save the result for a degree; and output results in order if present.
void save_result(int degree, vector<long> count, long long total) {
  cout << fixed << setprecision(PRECISION);
  lock_guard<mutex> guard(mtx);
  results[degree - first_degree].resize(degree);
  for (int i = 0; i < degree; ++i)
    results[degree - first_degree][i] = count[i] * 1.0 / total;

  // Output results, if we can
  for (; result_pos < degree_cnt && results[result_pos].size() > 0; result_pos += 2) {
    degree = first_degree + result_pos;
    if (degree % 2 == 1) {
      cout << degree;
      for (int i = 0; i < degree; ++i) {
        if (results[result_pos][i] == 0.0)
          cout << " 0";
        else
          cout << " " << results[result_pos][i];
      }
      cout << endl;
    }
  }
}


void count_ones(vector<long> &count, int pos, vector<int> &coefficients, vector<int> original) {
  int n = coefficients.size();
  if (n <= 1)
    return;
  else {
    for (int i = 1; i < n; ++i) {
      coefficients[i] += coefficients[i - 1];
    } 
    if (coefficients[n - 1] == 0) { // Remainder is zero
      coefficients.pop_back();
      if (pos > 0)
        count[pos - 1]--;
      count[pos]++;
      count_ones(count, pos + 1, coefficients, original);
    }
  }
}

void run_thread(int start_degree, int end_degree, int step, int iterations) {
  mt19937 generator;
  uniform_int_distribution<int> distribution(0, 1);
  for (int degree = start_degree; degree < end_degree; degree += step) {
    vector<long> count(degree);
    long long total; // number of polynomials checked
    if (iterations == 0) {
      total = 1LL << degree;
      for (long long i = 0; i < total; ++i) {
        vector<int> coefficients(degree + 1);
        for (int j = 0; j < degree + 1; ++j) {
          coefficients[j] = ((i >> j) % 2) * 2 - 1;
        }
        vector<int> og = coefficients;
        count_ones(count, 0, coefficients, og);
      }
    } else {
      total = iterations;
      for (int i = 0; i < iterations; ++i) {
        vector<int> coefficients(degree + 1);
        for (int j = 0; j < degree + 1; ++j) {
          coefficients[j] = (distribution(generator) % 2) * 2 - 1;
        }
        vector<int> og = coefficients;
        count_ones(count, 0, coefficients, og);
      }
    }

    save_result(degree, count, total);
  }
}

// Splits work load into threads and runs them
void run_program(pair<int, int> degree_range, int iterations, int thread_count) {
  if (degree_range.first % 2 == 0)
    degree_range.first++;
  first_degree = degree_range.first;
  degree_cnt = degree_range.second - degree_range.first + 1;
  results = new vector<double>[degree_cnt];

  vector<thread> threads(thread_count);
  for (int i = 0; i < thread_count; ++i) {
    threads[i] = thread(run_thread, first_degree + i * 2, degree_range.second + 1, thread_count * 2, iterations);
  }
  for (int i = 0; i < thread_count; ++i) {
    threads[i].join();
  }
}

void option_parse_fail(const cxxopts::Options &options, string msg) {
  cout << msg << options.help();
  exit(1);
}

// Parses command line arguments and calls run_program()
int main(int argc, char **argv) {
  cxxopts::Options options("count-roots", "");
  options.add_options()
    ("d,degree", "Degree to use for generating polynomials. A range of degrees can be given by separating the start and end with a dash, e.g. 2-12", cxxopts::value<string>())
    ("a,all", "Iterate through all possible polynomials of a degree.", cxxopts::value<bool>()->default_value("false"))
    ("i,iterations", "Number of polynomials to check for degree.", cxxopts::value<int>()->default_value("10000"))
    ("t,threads", "Number of threads to count roots with.", cxxopts::value<int>()->default_value("1"))
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

    run_program(degree_range, iterations, thread_count);
      
  } catch (cxxopts::OptionParseException &e) {
    option_parse_fail(options, e.what());
  }
}

#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>
#include <optional>
#include <thread>

#include "utils.cpp"
#include "cxxopts.hpp"

using namespace std;

//const int PRECISION = 12;

int thread_degree;

optional<gsl_complex> parse_root(const string line) {
  vector<string> parts = split_string(line, ' ');
  if (parts.size() != 2)
    return {};
  // Cut off 'i'
  if (parts[1].back() == 'i')
    parts[1].pop_back();
  
  double real, imag;
  try {
    real = stod(parts[0]);
    imag = stod(parts[1]);
  } catch (invalid_argument) {
    return {};
  }
  gsl_complex root = {real, imag};
  return root;
}

/**
 * Reads a data line from the file. Ignores blank lines and comments.
 * @return empty string if end of file, legit line otherwise
 */
string read_line(ifstream &file) {
  string s = "";
  while (s.empty() || s[0] == '#') {
    if (file.eof())
      return "";
    else
      getline(file, s);
  }
  return s;
}

/**
 * Read the next root from the file. Skips invalid lines. Returns empty if there is no next root.
 */
optional<gsl_complex> read_root(ifstream &file) {
  optional<gsl_complex> root;
  do {
    string line = read_line(file);
    if (line.empty()) // eof
      return {};
    root = parse_root(line);
    if (!root.has_value())
      cerr << "Failed to parse root from file: " << line << endl;
  } while (!root.has_value());
  
  return root;
}

void output_result(int degree, long long in_area_count, long long root_count) {
  while (thread_degree < degree)
    this_thread::sleep_for(chrono::milliseconds(100));
  //        cout << "Degree " << degree << " had " << in_area_count << " roots in area out of " << root_count << " total roots" << endl;
  cout << fixed << setprecision(12) << degree << " " << (in_area_count * degree * 1.0) / root_count << endl;
  thread_degree++;
}

void run_program(pair<int, int> degree_range, int step, int iterations, const double exponent) {
  for (int degree = degree_range.first; degree <= degree_range.second; degree += step) {
    string file_name = to_string(degree) + "-roots.txt";
    // TODO: make these values programmable via cmd line parameters
    gsl_complex center = {1.0, 0.0};
    double radius = 1.0 / pow(degree, exponent);
    try {
      ifstream file = ifstream(file_name, ios::in);
      if (file.fail()) {
        cerr << "Error: file " << file_name << " not found!" << endl;
        continue;
      }
      try {
        long long root_count = 0;
        long long in_area_count = 0;
        optional<gsl_complex> root = read_root(file);
        while (root.has_value()) {
          root_count++;
          double dist = gsl_complex_ops::distance(root.value(), center);
          // const double PRECISION_DIST = 1.0 / pow(2, PRECISION);
          if (dist < radius)
            in_area_count++;
          
          root = read_root(file);
        }
        output_result(degree, in_area_count, root_count);
        file.close(); // possibly not needed
      }
      catch (ifstream::failure &e) {
        cerr << "Something went wrong while reading the file " << file_name << endl;
      }
    } catch (ifstream::failure &e) {
      cerr << "Error: file " << file_name << " not found!" << endl;
    }
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
    ("d,degree", "Degree to use for opening the polynomial file for counting the roots of. A range of degrees can be given by separating the start and end with a dash, e.g. 2-12", cxxopts::value<string>())
    ("a,all", "Iterate through all polynomials in the file", cxxopts::value<bool>()->default_value("false"))
    ("i,iterations", "Number of polynomials (if present) to read from the file.", cxxopts::value<int>()->default_value("10000"))
    ("e,exponent", "Exponent for the degree when the disk radius is 1/d^e", cxxopts::value<double>()->default_value("1.0"))
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
    thread_degree = degree_range.first;

    // Read exponent count
    double exponent = result["exponent"].as<double>();

    if (thread_count == 1)
      run_program(degree_range, 1, iterations, exponent);
    else {
      vector<thread> threads(thread_count);
      for (int i = 0; i < thread_count; ++i) {
        threads[i] = thread(run_program, make_pair(degree_range.first + i, degree_range.second), thread_count, iterations, exponent);
      }
      for (int i = 0; i < thread_count; ++i) {
        threads[i].join();
      }
    }
    
  } catch (cxxopts::OptionParseException &e) {
    option_parse_fail(options, e.what());
  }
}

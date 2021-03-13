+#include <bits/stdc++.h>
#include <gsl/gsl_complex.h>

#include "utils.cpp"
#include "cxxopts.hpp"

using namespace std;

const int PRECISION = 12;

optional<gsl_complex> parse_root(const string line) {
  vector<string> parts = split_string(degree_string, ' ');
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
  gsl_complex root;
  GSL_SET_COMPLEX(&root, real, imag);
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
      s = getline(file, s);
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

void run_program(pair<int, int> degree_range, int iterations, gsl_complex center, double radius) {
  for (int degree = degree_range.first; degree <= degree_range.second; ++degree) {
    ifstream file;
    string file_name = to_string(degree) + "-roots.txt";
    try {
      file = ifstream(filename, ios::in);
      try {
        long long root_count = 0;
        long long in_area_count = 0;
        optional<gsl_complex> root = read_root(file);
        while (root.has_value()) {
          root_count++;
          // edit this
          double dist = gsl_complex_ops::distance(root, center);
          const double PRECISION_DIST = 1.0 / pow(2, PRECISION);
          if (dist - PRECISION_DIST < radius)
            in_area_count++;
        }
        cout << "Degree " << degree << " had " << in_area_count << " roots in area out of " << root_count << " total roots" << endl;
      }
      catch (ifstream::failure &e) {
        cerr << "Something went wrong while reading the file " << file_name << endl;
      }
      finally {
        file.close();
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
    ("c,center", "Center of the disk to count roots in")
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

    run_program(degree_range, iterations);
    
  } catch (cxxopts::OptionParseException &e) {
    option_parse_fail(options, e.what());
  }
}

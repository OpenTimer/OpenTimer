#include <iostream> 
#include <string_view>
#include <string>
#include <fstream>
#include <vector>
#include <cassert>
#include <cmath>
#include <tuple>
#include <algorithm>

// Function: tokenize
std::vector<std::string> split(const std::string& str) {

  const std::string_view dels {" "};

  // Parse the token.
  std::string token;
  std::vector<std::string> tokens;

  for(size_t i=0; i<str.size(); ++i) {
    bool is_del = (dels.find(str[i]) != std::string_view::npos);
    if(is_del || std::isspace(str[i])) {
      if(!token.empty()) {                            // Add the current token.
        tokens.push_back(std::move(token));
      }   
    } else {
      token.push_back(str[i]);  // Add the char to the current token.
    }   
  }

  if(!token.empty()) {
    tokens.push_back(std::move(token));
  }

  return tokens;
}



struct Result {
  Result() = default;
  std::string b_point;
  std::string e_point;
  double req_time {0.0};
  double arr_time {0.0};
  double slack {0.0};

  // Delay/Arrival/Edge/Pin 
  std::vector<std::tuple<double, double, bool, std::string>> nodes;

  friend std::ostream& operator << (std::ostream& os, const auto& r) {
    os << r.b_point << "/" << r.e_point << std::endl;
    os << r.req_time << "   " << r.arr_time << "   " << r.slack << std::endl;
    return os;
  }
};

// File format:
//          Endpoint: inst_83197/D
//        Beginpoint: inst_74236/CK
//    = Required Time     70.036
//     - Arrival Time    535.750
//       = Slack Time   -465.714
//        -      0.000  ^        inst_74236/CK
//   124.420    124.420  v        inst_74236/QN
//     0.000    124.420  v        inst_13721/A3
//     ...
std::vector<Result> read_file(const std::string fname) {

  std::vector<Result> results;
  std::ifstream ifs(fname);
  if(ifs.fail()) {
    std::cerr << "Unable to open file " << fname << std::endl;
    return results;
  }

  for (std::string line; std::getline(ifs, line);) {
    if(line.find("Endpoint") != std::string::npos) {
      // Read endpoint
      auto tokens = split(line);
      auto& res = results.emplace_back();
      res.e_point = tokens.back();

      // Read beginpoint
      std::getline(ifs, line); 
      tokens = split(line);
      res.b_point = tokens.back();

      // Read required time
      std::getline(ifs, line); 
      tokens = split(line);
      res.req_time = std::stod(tokens.back());

      // Read arrival time
      std::getline(ifs, line); 
      tokens = split(line);
      res.arr_time = std::stod(tokens.back());

      // Read slack
      std::getline(ifs, line); 
      tokens = split(line);
      res.slack = std::stod(tokens.back());

      // Read through nodes
      for (; std::getline(ifs, line);) {
        auto tlp = res.nodes.emplace_back();
        tokens = split(line);
        std::remove_if(tokens.begin(), tokens.end(), [](const auto &t){ return t.empty(); });
        
        // Read delay
        if(tokens[0] == "-") {
          // This is the begin point
          std::get<0>(tlp) = 0.0;
        }
        else {
          std::get<0>(tlp) = std::stod(tokens[0]);
        }

        // Read arrival
        std::get<1>(tlp) = std::stod(tokens[1]);

        // Read edge (rise/fall)
        if(tokens[2] == "^") {
          std::get<2>(tlp) = true;
        }
        else {
          std::get<2>(tlp) = false;
        }

        // Read pin
        std::get<3>(tlp) = tokens[3];
 
        if(tokens[3] == res.e_point) {
          break;
        }
      }

    }
  }
  return results;
}


// --------------------------------------------------------------------------------------
// This program can compare the output files from two tau18 contest timers
// --------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {

  if(argc != 3) {
    std::cerr << "Usage: ./a.out output_file1 output_file2" << std::endl;
    return 0;
  }

  auto results_1 = read_file(argv[1]);
  auto results_2 = read_file(argv[2]);

  // If eithe one is empty, the read_file fails
  if(results_1.empty() || results_2.empty()) {
    return 0;
  }

  assert(results_1.size() == results_2.size());

  double diff {0.0};
  double slack_diff {0.0};
  int slack_id = {-1};

  for(size_t i=0; i<results_1.size(); i++) {
    auto& r1 = results_1[i];
    auto& r2 = results_2[i];

    assert(r1.b_point == r2.b_point);
    assert(r1.e_point == r2.e_point);
    
    if(std::fabs(r1.req_time-r2.req_time) > diff) {
      diff = std::fabs(r1.req_time-r2.req_time);
    }
    if(std::fabs(r1.slack-r2.slack) > slack_diff) {
      slack_diff = std::fabs(r1.slack-r2.slack);
      slack_id = i;
    }

    //assert(std::fabs(r1.req_time-r2.req_time) < 0.01);
    //assert(std::fabs(r1.arr_time-r2.arr_time) < 0.01);
    //assert(std::fabs(r1.slack-r2.slack) < 0.01);
    
    //assert(r1.nodes == r2.nodes);
  }

  if(slack_id != -1) {
    std::cout << "----------------  Difference  -----------------\n"; 
    std::cout << "Largest diff = " << diff << std::endl;
    std::cout << "Largest slack diff = " << slack_diff << std::endl;
    std::cout << "Result 1 = " << results_1[slack_id] << std::endl;
    std::cout << "Result 2 = " << results_2[slack_id] << std::endl;
  }

}



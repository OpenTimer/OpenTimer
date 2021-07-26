#include <ot/timer/timer.hpp>

int main(int argc, char* argv[]) {

  using namespace std::literals::string_literals;

  if(argc != 5) {
    OT_LOGF("usage: ot-tau15 <file>.tau2015 <file>.timing <file>.ops output");
  }

  std::filesystem::path tau2015 = argv[1];
  std::filesystem::path timing = argv[2];
  std::filesystem::path ops = argv[3];
  std::filesystem::path output = argv[4];
  std::filesystem::path early_celllib;
  std::filesystem::path late_celllib;
  std::filesystem::path spef;
  std::filesystem::path verilog;

  if(std::ifstream ifs(tau2015); ifs) {
    ifs >> early_celllib >> late_celllib >> spef >> verilog; 
  }
  else {
    OT_LOGF("error in opening ", tau2015);
  }

  ot::Timer timer;

  // Initialize the timer.
  timer.read_celllib(early_celllib, ot::MIN)
       .read_celllib(late_celllib, ot::MAX)
       .read_verilog(verilog)
       .read_spef(spef)
       .read_timing(timing)
       .cppr(true);
  
  std::string line, token, pin;
  
  // ------------------------------------------------------
  // Read ops file.
  // ------------------------------------------------------
  std::ofstream ofs(output);
  std::ifstream ifs(ops);

  ofs << std::fixed << std::setprecision(3);

  while(std::getline(ifs, line)) {
    
    auto tokens = ot::split(line); 

    if(tokens.empty()) break;

    if(tokens[0] == "report_slew")  {
      auto el = ot::MIN;
      auto rf = ot::RISE;  
      auto pin = ""s;

      for(size_t i=1; i<tokens.size(); ++i) {
        if(tokens[i] == "-pin") {
          pin = std::move(tokens[++i]);
        }
        else if(tokens[i] == "-rise") {
          rf = ot::RISE;
        }
        else if(tokens[i] == "-fall") {
          rf = ot::FALL;
        }
        else if(tokens[i] == "-early") {
          el = ot::MIN;
        }
        else if(tokens[i] == "-late") {
          el = ot::MAX;
        }
        else {
          throw std::runtime_error("unexpected report_slew option "s + tokens[i]);
        }
      }

      auto slew = timer.report_slew(std::move(pin), el, rf);
      ofs << (slew ? *slew : std::numeric_limits<float>::quiet_NaN()) << '\n';
    }
    else if(tokens[0] == "report_at")  {
      auto el = ot::MIN;
      auto rf = ot::RISE;  
      auto pin = ""s;

      for(size_t i=1; i<tokens.size(); ++i) {
        if(tokens[i] == "-pin") {
          pin = std::move(tokens[++i]);
        }
        else if(tokens[i] == "-rise") {
          rf = ot::RISE;
        }
        else if(tokens[i] == "-fall") {
          rf = ot::FALL;
        }
        else if(tokens[i] == "-early") {
          el = ot::MIN;
        }
        else if(tokens[i] == "-late") {
          el = ot::MAX;
        }
        else {
          throw std::runtime_error("unexpected report_at option "s + tokens[i]);
        }
      }
      auto at = timer.report_at(std::move(pin), el, rf);
      ofs << (at ? *at : std::numeric_limits<float>::quiet_NaN()) << '\n';
    }
    else if(tokens[0] == "report_rat"){
      
      auto el = ot::MIN;
      auto rf = ot::RISE;  
      auto pin = ""s;

      for(size_t i=1; i<tokens.size(); ++i) {
        if(tokens[i] == "-pin") {
          pin = std::move(tokens[++i]);
        }
        else if(tokens[i] == "-rise") {
          rf = ot::RISE;
        }
        else if(tokens[i] == "-fall") {
          rf = ot::FALL;
        }
        else if(tokens[i] == "-early") {
          el = ot::MIN;
        }
        else if(tokens[i] == "-late") {
          el = ot::MAX;
        }
        else {
          throw std::runtime_error("unexpected report_rat option "s + tokens[i]);
        }
      }
      auto rat = timer.report_rat(std::move(pin), el, rf);
      ofs << (rat ? *rat : std::numeric_limits<float>::quiet_NaN()) << '\n';
    }
    else if(tokens[0] == "report_slack"){
      
      auto el = ot::MIN;
      auto rf = ot::RISE;  
      auto pin = ""s;

      for(size_t i=1; i<tokens.size(); ++i) {
        if(tokens[i] == "-pin") {
          pin = std::move(tokens[++i]);
        }
        else if(tokens[i] == "-rise") {
          rf = ot::RISE;
        }
        else if(tokens[i] == "-fall") {
          rf = ot::FALL;
        }
        else if(tokens[i] == "-early") {
          el = ot::MIN;
        }
        else if(tokens[i] == "-late") {
          el = ot::MAX;
        }
        else {
          throw std::runtime_error("unexpected report_at option "s + tokens[i]);
        }
      }
      auto slack = timer.report_slack(std::move(pin), el, rf);
      ofs << (slack ? *slack : std::numeric_limits<float>::quiet_NaN()) << '\n';
    }
    else if(tokens[0] == "insert_gate") {
      if(tokens.size() < 3) {
        throw std::runtime_error("syntax error in insert_gate");
      }
      timer.insert_gate(std::move(tokens[1]), std::move(tokens[2]));
    }
    else if(tokens[0] == "remove_gate") {
      if(tokens.size() < 2) {
        throw std::runtime_error("syntax error in remove_gate");
      }
      timer.remove_gate(std::move(tokens[1]));
    }
    else if(tokens[0] == "repower_gate") {
      if(tokens.size() < 3) {
        throw std::runtime_error("syntax error in repower_gate");
      }
      timer.repower_gate(std::move(tokens[1]), std::move(tokens[2]));
    }
    else if(tokens[0] == "insert_net") {
      if(tokens.size() < 2) {
        throw std::runtime_error("syntax error in insert_net");
      }
      timer.insert_net(std::move(tokens[1]));
    }
    else if(tokens[0] == "remove_net") {
      if(tokens.size() < 2) {
        throw std::runtime_error("syntax error in remove_net");
      }
      timer.remove_net(std::move(tokens[1]));
    }
    else if(tokens[0] == "connect_pin") {
      if(tokens.size() < 3) {
        throw std::runtime_error("syntax error in connect_pin"); 
      }
      timer.connect_pin(std::move(tokens[1]), std::move(tokens[2]));
    }
    else if(tokens[0] == "disconnect_pin") {
      if(tokens.size() < 2) {
        throw std::runtime_error("syntax error in disconnect_pin");
      }
      timer.disconnect_pin(std::move(tokens[1]));
    }
    else if(tokens[0] == "read_spef") {
      if(tokens.size() < 2) {
        throw std::runtime_error("syntax error in remove_net");
      }
      timer.read_spef(std::move(tokens[1]));
    }
    else if(tokens[0] == "report_worst_paths") {

      size_t K {0};

      for(size_t i=1; i<tokens.size(); ++i) {
        if(tokens[i] == "-numPaths") {
          if(i+1 >= tokens.size()) {
            throw std::runtime_error("syntax error in -numPaths");
          }
          K = std::stoul(tokens[i+1]);
        }
      }

      if(K == 0) {
        continue;
      }

      auto paths = timer.report_timing(K);
      
      ofs << "report_worst_paths " << paths.size() << '\n';

      for(size_t i=0; i<paths.size(); ++i) {
        ofs << "Path " << i+1 << ": ";
        if(paths[i].endpoint->primary_output()) {
          ofs << "RAT";
        }
        else {
          ofs << (paths[i].endpoint->split() == ot::MIN ? "Hold" : "Setup");
        }
        ofs << ' ' << paths[i].slack << ' ' << paths[i].size() 
                  << ' ' << (paths[i].endpoint->split() == ot::MIN ? "E" : "L") << '\n';
        
        for(auto itr = paths[i].rbegin(); itr != paths[i].rend(); ++itr) {
          ofs << itr->pin.name() << ' ' 
              << (itr->transition == ot::RISE ? 'R' : 'F') << '\n';
        }
      }
    }
    else {
      throw std::runtime_error("unexpected operation "s + line);
      break;
    }
  }
  

  return 0;
}










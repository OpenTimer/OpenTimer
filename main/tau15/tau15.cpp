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
  timer.celllib(early_celllib, ot::EARLY)
       .celllib(late_celllib, ot::LATE)
       .verilog(verilog)
       .spef(spef)
       .timing(timing)
       .cppr(true);
  
  std::string line, token, pin;
  
  // ------------------------------------------------------
  // Read ops file.
  // ------------------------------------------------------
  std::ofstream ofs(output);
  std::ifstream ifs(ops);

  while(std::getline(ifs, line)) {
    
    auto tokens = ot::split(line); 

    if(tokens.empty()) break;

    if(tokens[0] == "report_slew")  {
      auto el = ot::EARLY;
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
          el = ot::EARLY;
        }
        else if(tokens[i] == "-late") {
          el = ot::LATE;
        }
        else {
          throw std::runtime_error("unexpected report_slew option "s + tokens[i]);
        }
      }

      auto slew = timer.slew(std::move(pin), el, rf);
      ofs << (slew ? *slew : std::numeric_limits<float>::quiet_NaN()) << '\n';
    }
    else if(tokens[0] == "report_at")  {
      auto el = ot::EARLY;
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
          el = ot::EARLY;
        }
        else if(tokens[i] == "-late") {
          el = ot::LATE;
        }
        else {
          throw std::runtime_error("unexpected report_at option "s + tokens[i]);
        }
      }
      auto at = timer.at(std::move(pin), el, rf);
      ofs << (at ? *at : std::numeric_limits<float>::quiet_NaN()) << '\n';
    }
    else if(tokens[0] == "report_rat"){
      
      auto el = ot::EARLY;
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
          el = ot::EARLY;
        }
        else if(tokens[i] == "-late") {
          el = ot::LATE;
        }
        else {
          throw std::runtime_error("unexpected report_rat option "s + tokens[i]);
        }
      }
      auto rat = timer.rat(std::move(pin), el, rf);
      ofs << (rat ? *rat : std::numeric_limits<float>::quiet_NaN()) << '\n';
    }
    else if(tokens[0] == "report_slack"){
      
      auto el = ot::EARLY;
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
          el = ot::EARLY;
        }
        else if(tokens[i] == "-late") {
          el = ot::LATE;
        }
        else {
          throw std::runtime_error("unexpected report_at option "s + tokens[i]);
        }
      }
      auto slack = timer.slack(std::move(pin), el, rf);
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
      timer.spef(std::move(tokens[1]));
    }
    else if(tokens[0] == "report_worst_paths") {
      auto paths = timer.worst_paths(1);
      std::cout << paths[0];
    }
    else {
      throw std::runtime_error("unexpected operation "s + line);
      break;
    }
  }
  

  return 0;
}










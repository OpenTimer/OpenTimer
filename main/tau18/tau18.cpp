#include <ot/timer/timer.hpp>

int main(int argc, char* argv[]) {

  using namespace std::literals::string_literals;

  if(argc != 5) {
    OT_LOGF("usage: ot-tau18 <file>.tau2018 <file>.timing <file>.ops output");
  }

  std::filesystem::path tau2018 = argv[1];
  std::filesystem::path timing = argv[2];
  std::filesystem::path ops = argv[3];
  std::filesystem::path output = argv[4];
  std::filesystem::path early_celllib;
  std::filesystem::path late_celllib;
  std::filesystem::path spef;
  std::filesystem::path verilog;

  if(std::ifstream ifs(tau2018); ifs) {
    ifs >> early_celllib >> late_celllib >> spef >> verilog; 
  }
  else {
    OT_LOGF("error in opening ", tau2018);
  }

  ot::Timer timer;

  // Initialize the timer.
  timer.read_celllib(early_celllib, ot::MIN)
       .read_celllib(late_celllib, ot::MAX)
       .read_verilog(verilog)
       .read_spef(spef)
       .read_timing(timing);

       // TODO: in 2018, we consider cppr?
       //.cppr(true);
  
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

    if(tokens[0] == "report_timing")  {
      OT_LOGD("report_timing command received: ");
      for(const auto& token : tokens) {
        std::cout << token << ' '; 
      }
      std::cout << std::endl;

      // TODO: transform tokens to ot::PathGuide
      ot::PathGuide pg;
      auto paths = timer.report_timing(std::move(pg));
    }
    else {
      throw std::runtime_error("unexpected operation "s + line);
      break;
    }
  }
  

  return 0;
}










#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: update_timing
void Shell::_update_timing() {
  _timer.update_timing();
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_at
void Shell::_report_at() {

  std::string token;
  std::string pin;
  auto el = MIN;
  auto rf = RISE;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-min" || token == "-early") el = MIN;
    else if(token == "-max" || token == "-late" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  auto at = _timer.report_at(std::move(pin), el, rf);
  _os << (at ? *at : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_rat
void Shell::_report_rat() {

  std::string token;
  std::string pin;
  auto el = MIN;
  auto rf = RISE;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-min" || token == "-early") el = MIN;
    else if(token == "-max" || token == "-late" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  auto rat = _timer.report_rat(std::move(pin), el, rf);
  _os << (rat ? *rat : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_slack
void Shell::_report_slack() {

  std::string token;
  std::string pin;
  auto el = MIN;
  auto rf = RISE;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-min" || token == "-early") el = MIN;
    else if(token == "-max" || token == "-late" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  auto slack = _timer.report_slack(std::move(pin), el, rf);
  _os << (slack ? *slack : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_slew
void Shell::_report_slew() {

  std::string token;
  std::string pin;
  auto el = MIN;
  auto rf = RISE;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-min" || token == "-early") el = MIN;
    else if(token == "-max" || token == "-late" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  auto slew = _timer.report_slew(std::move(pin), el, rf);
  _os << (slew ? *slew : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_tns
void Shell::_report_tns() {
  
  std::string token;
  
  std::optional<Split> el;
  std::optional<Tran> rf;

  while(_is >> token) {
    if(token == "-min" || token == "-early") el = MIN;
    else if(token == "-max" || token == "-late" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(auto tns = _timer.report_tns(el, rf); tns) {
    _os << *tns << '\n';
  }
  else {
    _os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_wns
void Shell::_report_wns() {
  
  std::string token;
  
  std::optional<Split> el;
  std::optional<Tran> rf;

  while(_is >> token) {
    if(token == "-min" || token == "-early") el = MIN;
    else if(token == "-max" || token == "-late" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(auto wns = _timer.report_wns(el, rf); wns) {
    _os << *wns << '\n';
  }
  else {
    _os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_fep
void Shell::_report_fep() {
  
  std::string token;
  
  std::optional<Split> el;
  std::optional<Tran> rf;

  while(_is >> token) {
    if(token == "-min" || token == "-early") el = MIN;
    else if(token == "-max" || token == "-late" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(auto fep = _timer.report_fep(el, rf); fep) {
    _os << *fep << '\n';
  }
  else {
    _os << "no failing endpoint found\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_timing
void Shell::_report_timing() {
  
  std::string token;
  size_t K {1};

  std::optional<Split> split;
  std::optional<Tran> tran;
  
  // TODO: make the following format extensible
  bool tau18 {false};

  while(_is >> token) {
    if(token == "-num_paths") {
      _is >> K;
    }
    else if(token == "-min" || token == "-early") {
      split = MIN;
    }
    else if(token == "-max" || token == "-late") {
      split = MAX;
    }
    else if(token == "-rise") {
      tran = RISE;
    }
    else if(token == "-fall") {
      tran = FALL;
    }
    else if(token == "-format") {
      _is >> token;
      if(token == "tau18") {
        tau18 = true;
      }
    }
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  std::vector<Path> paths;
  
  if(split && tran) {
    paths = _timer.report_timing(K, *split, *tran);
  }
  else if(split && !tran) {
    paths = _timer.report_timing(K, *split);
  }
  else if(!split && tran) {
    paths = _timer.report_timing(K, *tran);
  }
  else {
    paths = _timer.report_timing(K);
  }

  if(paths.empty()) {
    _os << "no critical path found\n";
  }
  else {
    for(size_t i=0; i<paths.size(); ++i) {
      if(i) _os << '\n';

      if(tau18) {
        paths[i].dump_tau18(_os);
      }
      else {
        _os << paths[i];
      }
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_path
void Shell::_report_path() {
  _report_timing();
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_area
void Shell::_report_area() {
  if(auto area = _timer.report_area(); area) {
    _os << *area << '\n';
  }
  else {
    _os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_leakage_power
void Shell::_report_leakage_power() {
  if(auto lp = _timer.report_leakage_power(); lp) {
    _os << *lp << '\n';
  }
  else {
    _os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------










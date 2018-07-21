#include <ot/tau/tau15.hpp>
#include <ot/timer/timer.hpp>

namespace ot::tau15 {

// Procedure: read
void Timing::read(const std::filesystem::path& path) {
  
  std::string line, token, pin;
  
  // ------------------------------------------------------
  // Read timing file.
  // ------------------------------------------------------
  std::ifstream tfs(path);

  if(!tfs.good()) {
    OT_LOGW("can't open ", path);
    return;
  }
  
  while(std::getline(tfs, line)) {
    
    std::istringstream ss(line);

    ss >> token;

    if(token == "clock") {
      Clock clock;
      ss >> clock.pin >> clock.period;
      assertions.push_back(std::move(clock));
    }
    else if(token == "at") {
      AT at;
      ss >> at.pin 
         >> at.value[ot::EARLY][ot::RISE] >> at.value[ot::EARLY][ot::FALL] 
         >> at.value[ot::LATE][ot::RISE]  >> at.value[ot::LATE][ot::FALL];
      assertions.push_back(std::move(at));
    }
    else if(token == "slew") {
      Slew slew;
      ss >> slew.pin
         >> slew.value[ot::EARLY][ot::RISE] >> slew.value[ot::EARLY][ot::FALL]
         >> slew.value[ot::LATE][ot::RISE] >> slew.value[ot::LATE][ot::FALL];
      assertions.push_back(std::move(slew));
    }
    else if(token == "rat") {
      RAT rat;
      ss >> rat.pin
         >> rat.value[ot::EARLY][ot::RISE] >> rat.value[ot::EARLY][ot::FALL]
         >> rat.value[ot::LATE][ot::RISE] >> rat.value[ot::LATE][ot::FALL];
      assertions.push_back(std::move(rat));
    }
    else if(token == "load") {
      Load load;
      ss >> load.pin >> load.value;
      assertions.push_back(std::move(load));
    }
    else {
      OT_LOGF("unknown keyword ", token);
    }
  }
}

};  // end of namespace ot::tau15. ----------------------------------------------------------------

namespace ot {

// Function: timing
Timer& Timer::timing(std::filesystem::path path) {

  auto timing = std::make_shared<tau15::Timing>();

  std::scoped_lock lock(_mutex);
  
  // Library reader
  auto reader = _taskflow.silent_emplace([this, path=std::move(path), timing] () {
    OT_LOGI("loading timing ", path, " ...");
    timing->read(path);
  });

  // Placeholder to add_lineage
  auto modifier = _taskflow.silent_emplace([this, timing] () {
    OT_LOGI("add ", timing->assertions.size(), " timing assertions");
    _timing(*timing);
  });

  // Reader -> modifier
  reader.precede(modifier);

  // parent -> modifier
  _add_to_lineage(modifier);
  
  return *this;
}

// Procedure: _timing
void Timer::_timing(tau15::Timing& timing) {

  for(auto& ast : timing.assertions) {
    std::visit(Functors{
      [&] (tau15::Clock& clock) {
        if(auto itr = _pins.find(clock.pin); itr != _pins.end()) {
          _clock(itr->first, itr->second, clock.period);
        }
        else {
          OT_LOGW("can't create clock (pin ", itr->first, " not found");
        }
      },
      [&] (tau15::AT& a) {
        FOR_EACH_EL_RF(el, rf) {
          _at(a.pin, el, rf, a.value[el][rf]);
        }
      },
      [&] (tau15::Slew& s) {
        FOR_EACH_EL_RF(el, rf) {
          _slew(s.pin, el, rf, s.value[el][rf]);
        }
      },
      [&] (tau15::RAT& r) {
        FOR_EACH_EL_RF(el, rf) {
          _rat(r.pin, el, rf, r.value[el][rf]);
        }
      },
      [&] (tau15::Load& l) {
        FOR_EACH_EL_RF(el, rf) {
          _load(l.pin, el, rf, l.value);
        }
      }
    }, ast);
  }
}

};  // end of namespace ot ------------------------------------------------------------------------


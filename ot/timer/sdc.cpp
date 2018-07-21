#include <ot/timer/timer.hpp>

namespace ot {

// Function: sdc
Timer& Timer::sdc(std::filesystem::path path) {

  // Create a shared sdc object
  auto sdc = std::make_shared<sdc::SDC>();

  std::scoped_lock lock(_mutex);
  
  // reader
  auto reader = _taskflow.silent_emplace([this, sdc, path=std::move(path)] () {
    OT_LOGI("loading sdc ", path, " ...");
    sdc->read(path);
  });

  // modifier
  auto modifier = _taskflow.silent_emplace([this, sdc] () mutable {
    OT_LOGI("sdc [commands:", sdc->commands.size(), ']');
    _sdc(*sdc);
  });

  // Build the task dependency
  reader.precede(modifier);

  // Build the dependency with the precedent
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _sdc
void Timer::_sdc(sdc::SDC& sdc) {
  for(auto& command : sdc.commands) { 
    std::visit(Functors{
      [this] (auto&& cmd) {
        _sdc(cmd);
      }
    }, command);
  }
}

// Procedure: _sdc
// Sets input delay on pins or input ports relative to a clock signal.
void Timer::_sdc(sdc::SetInputDelay& obj) {

  OT_LOGW_RIF(!obj.value, obj.command, ": delay not defined");
  OT_LOGW_RIF(!obj.port,  obj.command, ": port not defined");

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllInputs&) {
      for(auto& kvp : _pis) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _at(kvp.second, el, rf, obj.value);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pis.find(port); itr != _pis.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _at(itr->second, el, rf, obj.value); 
          }
        }
        else {
          OT_LOGW(obj.command, ": port ", std::quoted(port), " not found");
        }
      }
    },
    [] (auto&&) {
      assert(false);
    }
  }, *obj.port);
}

// Procedure: _sdc
// Sets input transition on pins or input ports relative to a clock signal.
void Timer::_sdc(sdc::SetInputTransition& obj) {

  OT_LOGW_RIF(!obj.value, obj.command, ": transition not defined");
  OT_LOGW_RIF(!obj.port,  obj.command, ": port not defined");

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllInputs&) {
      for(auto& kvp : _pis) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _slew(kvp.second, el, rf, obj.value);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pis.find(port); itr != _pis.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _slew(itr->second, el, rf, obj.value); 
          }
        }
        else {
          OT_LOGW(obj.command, ": port ", std::quoted(port), " not found");
        }
      }
    },
    [] (auto&&) {
      assert(false);
    }
  }, *obj.port);
}

// Procedure: _sdc
// Sets output delay on pins or input ports relative to a clock signal.
void Timer::_sdc(sdc::SetOutputDelay& obj) {

  OT_LOGW_RIF(!obj.value, obj.command, ": delay not defined");
  OT_LOGW_RIF(!obj.port,  obj.command, ": port not defined");

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllOutputs&) {
      for(auto& kvp : _pos) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _rat(kvp.second, el, rf, obj.value);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pos.find(port); itr != _pos.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _rat(itr->second, el, rf, obj.value); 
          }
        }
        else {
          OT_LOGW(obj.command, ": port ", std::quoted(port), " not found");
        }
      }
    },
    [] (auto&&) {
      assert(false);
    }
  }, *obj.port);
}

// Procedure: _sdc
// Sets the load attribute to a specified value on specified ports and nets.
void Timer::_sdc(sdc::SetLoad& obj) {

  OT_LOGW_RIF(!obj.value, obj.command, ": load not defined");
  OT_LOGW_RIF(!obj.port,  obj.command, ": port not defined");
  
  auto mask = sdc::TimingMask(obj.min, obj.max, std::nullopt, std::nullopt);

  std::visit(Functors{
    [&] (sdc::AllOutputs&) {
      for(auto& kvp : _pos) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _load(kvp.second, el, rf, obj.value);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pos.find(port); itr != _pos.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _load(itr->second, el, rf, obj.value); 
          }
        }
        else {
          OT_LOGW(obj.command, ": port ", std::quoted(port), " not found");
        }
      }
    },
    [] (auto&&) {
      assert(false);
    }
  }, *obj.port);
}

// Procedure: _sdc
// create a clock object and defines its waveform in the current design.
void Timer::_sdc(sdc::CreateClock& obj) {

  OT_LOGW_RIF(obj.name.empty(), obj.command, ": name not defined");
  OT_LOGW_RIF(!obj.period, obj.command, ": period not defined");
  OT_LOGW_RIF(!obj.port,   obj.command, ": port not defined");

  std::visit(Functors{
    [&] (sdc::GetPorts& get_ports) {
      auto& ports = get_ports.ports;
      assert(ports.size() == 1);
      if(auto itr = _pins.find(ports.front()); itr != _pins.end()) {
        if(obj.waveform) {
          _clocks.emplace(obj.name, itr->second, *obj.period);
        }
        else {
          _clocks.emplace(obj.name, itr->second, *obj.period, *obj.waveform);
        }
      }
      else {
        OT_LOGW(obj.command, ": port ", std::quoted(ports.front()), " not found");
      }
    },
    [] (auto&&) {
      assert(false);
    }
  }, *obj.port);

}

};  // end of namespace ot. -----------------------------------------------------------------------






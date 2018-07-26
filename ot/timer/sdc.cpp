#include <ot/timer/timer.hpp>

namespace ot {

// Function: sdc
Timer& Timer::sdc(std::filesystem::path path) {

  // Create a shared sdc object
  auto sdc = std::make_shared<sdc::SDC>();

  std::scoped_lock lock(_mutex);
  
  // reader
  auto reader = _taskflow.silent_emplace([this, sdc, path=std::move(path)] () {
    sdc->read(path);
  });

  // modifier
  auto modifier = _taskflow.silent_emplace([this, sdc] () mutable {
    _sdc(*sdc);
    OT_LOGI("added ", sdc->commands.size(), " sdc commands");
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


  //
}

// Procedure: _sdc
// Sets input delay on pins or input ports relative to a clock signal.
void Timer::_sdc(sdc::SetInputDelay& obj) {

  assert(obj.delay_value && obj.port_pin_list);

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllInputs&) {
      for(auto& kvp : _pis) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _at(kvp.second, el, rf, obj.delay_value);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pis.find(port); itr != _pis.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _at(itr->second, el, rf, obj.delay_value); 
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
  }, *obj.port_pin_list);
}

// Procedure: _sdc
// Sets input transition on pins or input ports relative to a clock signal.
void Timer::_sdc(sdc::SetInputTransition& obj) {

  assert(obj.transition && obj.port_list);

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllInputs&) {
      for(auto& kvp : _pis) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _slew(kvp.second, el, rf, obj.transition);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pis.find(port); itr != _pis.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _slew(itr->second, el, rf, obj.transition); 
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
  }, *obj.port_list);
}

// Procedure: _sdc
// Sets output delay on pins or input ports relative to a clock signal.
void Timer::_sdc(sdc::SetOutputDelay& obj) {

  assert(obj.delay_value && obj.port_pin_list);

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllOutputs&) {
      for(auto& kvp : _pos) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _rat(kvp.second, el, rf, obj.delay_value);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pos.find(port); itr != _pos.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _rat(itr->second, el, rf, obj.delay_value); 
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
  }, *obj.port_pin_list);
}

// Procedure: _sdc
// Sets the load attribute to a specified value on specified ports and nets.
void Timer::_sdc(sdc::SetLoad& obj) {

  assert(obj.value && obj.objects);
  
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
  }, *obj.objects);
}

// Procedure: _sdc
// create a clock object and defines its waveform in the current design.
void Timer::_sdc(sdc::CreateClock& obj) {

  assert(obj.period && obj.port_pin_list && !obj.name.empty());

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
  }, *obj.port_pin_list);

}

};  // end of namespace ot. -----------------------------------------------------------------------






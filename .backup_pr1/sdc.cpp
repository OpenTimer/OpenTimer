#include <ot/timer/timer.hpp>

namespace ot {

// Function: read_sdc
Timer& Timer::read_sdc(std::filesystem::path path) {

  // Create a shared sdc object
  auto sdc = std::make_shared<sdc::SDC>();

  std::scoped_lock lock(_mutex);
  
  // parser
  auto parser = _taskflow.emplace([sdc, path=std::move(path)] () {
    sdc->read(path);
  });

  // reader
  auto reader = _taskflow.emplace([this, sdc] () mutable {
    _read_sdc(*sdc);
    OT_LOGI("added ", sdc->commands.size(), " sdc commands");
  });

  // Build the task dependency
  parser.precede(reader);

  _add_to_lineage(reader);

  return *this;
}

// Procedure: _sdc
void Timer::_read_sdc(sdc::SDC& sdc) {
  for(auto& command : sdc.commands) { 
    std::visit(Functors{
      [this] (auto&& cmd) {
        _read_sdc(cmd);
      }
    }, command);
  }
}

// Procedure: _sdc
// Sets input delay on pins or input ports relative to a clock signal.
void Timer::_read_sdc(sdc::SetInputDelay& obj) {

  assert(obj.delay_value && obj.port_pin_list);

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllInputs&) {
      for(auto& kvp : _pis) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _set_at(kvp.second, el, rf, obj.delay_value);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pis.find(port); itr != _pis.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _set_at(itr->second, el, rf, obj.delay_value); 
          }
        }
        else {
          OT_LOGE(obj.command, ": port ", std::quoted(port), " not found");
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
void Timer::_read_sdc(sdc::SetInputTransition& obj) {

  assert(obj.transition && obj.port_list);

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllInputs&) {
      for(auto& kvp : _pis) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _set_slew(kvp.second, el, rf, obj.transition);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pis.find(port); itr != _pis.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _set_slew(itr->second, el, rf, obj.transition); 
          }
        }
        else {
          OT_LOGE(obj.command, ": port ", std::quoted(port), " not found");
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
void Timer::_read_sdc(sdc::SetOutputDelay& obj) {

  assert(obj.delay_value && obj.port_pin_list);

  if(_clocks.find(obj.clock) == _clocks.end()) {
    OT_LOGE(obj.command, ": clock ", std::quoted(obj.clock), " not found");
    return;
  }

  auto& clock = _clocks.at(obj.clock);

  auto mask = sdc::TimingMask(obj.min, obj.max, obj.rise, obj.fall);

  std::visit(Functors{
    [&] (sdc::AllOutputs&) {
      for(auto& kvp : _pos) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _set_rat(
            kvp.second, 
            el, 
            rf, 
            el == MIN ? -(*obj.delay_value) : clock._period - (*obj.delay_value)
          );
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pos.find(port); itr != _pos.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _set_rat(
              itr->second, 
              el, 
              rf, 
              el == MIN ? -(*obj.delay_value) : clock._period - (*obj.delay_value)
            ); 
          }
        }
        else {
          OT_LOGE(obj.command, ": port ", std::quoted(port), " not found");
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
void Timer::_read_sdc(sdc::SetLoad& obj) {

  assert(obj.value && obj.objects);
  
  auto mask = sdc::TimingMask(obj.min, obj.max, std::nullopt, std::nullopt);

  std::visit(Functors{
    [&] (sdc::AllOutputs&) {
      for(auto& kvp : _pos) {
        FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
          _set_load(kvp.second, el, rf, obj.value);
        }
      }
    },
    [&] (sdc::GetPorts& get_ports) {
      for(auto& port : get_ports.ports) {
        if(auto itr = _pos.find(port); itr != _pos.end()) {
          FOR_EACH_EL_RF_IF(el, rf, (mask | el) && (mask | rf)) {
            _set_load(itr->second, el, rf, obj.value); 
          }
        }
        else {
          OT_LOGE(obj.command, ": port ", std::quoted(port), " not found");
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
void Timer::_read_sdc(sdc::CreateClock& obj) {

  assert(obj.period && !obj.name.empty());
  
  // create clock from given sources
  if(obj.port_pin_list) {
    std::visit(Functors{
      [&] (sdc::GetPorts& get_ports) {
        auto& ports = get_ports.ports;
        assert(ports.size() == 1);
        if(auto itr = _pins.find(ports.front()); itr != _pins.end()) {
          _create_clock(obj.name, itr->second, *obj.period);
        }
        else {
          OT_LOGE(obj.command, ": port ", std::quoted(ports.front()), " not found");
        }
      },
      [] (auto&&) {
        assert(false);
      }
    }, *obj.port_pin_list);
  }
  // create virtual clock
  else {
    _create_clock(obj.name, *obj.period);
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------






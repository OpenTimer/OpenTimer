#include <ot/timer/timer.hpp>

// TODO (twhuang)
// (1) extend taskflow's capability to enable intra-task parallelization

namespace ot {

// Function: time_unit
Timer& Timer::time_unit(second_t unit) {
  
  std::scoped_lock lock(_mutex);

  // timer task
  auto task = _taskflow.silent_emplace([this, unit=std::move(unit)] () {
    _to_time_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_time_unit
void Timer::_to_time_unit(const second_t& unit) {
  
  OT_LOGI("use time unit ", unit);

  float s = (_time_unit) ? (*_time_unit / unit).value() : 1.0f;

  if(_time_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
    return;
  }

  // scale po time
  for(auto& kvp : _pos) {
    kvp.second._scale_time(s);
  }
  
  // scale pi time
  for(auto& kvp : _pis) {
    kvp.second._scale_time(s);
  }

  // scale clock time
  for(auto& kvp : _clocks) {
    kvp.second._scale_time(s);
  }

  // library time
  FOR_EACH_EL(el) {
    _celllib[el].to_time_unit(unit);
  }
  
  // enable full timing update
  _enable_full_timing_update();
}


// Function: capacitance_unit
Timer& Timer::capacitance_unit(farad_t unit) {
  
  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.silent_emplace([this, unit=std::move(unit)] () {
    _to_capacitance_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_capacitance_unit
void Timer::_to_capacitance_unit(const farad_t& unit) {
  
  OT_LOGI("use capacitance unit ", unit);

  float s = (_capacitance_unit) ? (*_capacitance_unit / unit).value() : 1.0f;

  if(_capacitance_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
    return;
  }

  // scale po capacitance
  for(auto& kvp : _pos) {
    kvp.second._scale_capacitance(s);
  }

  // scale net capacitance
  for(auto& kvp : _nets) {
    kvp.second._scale_capacitance(s);
  }

  // library capacitance
  FOR_EACH_EL(el) {
    _celllib[el].to_capacitance_unit(unit);
  }
  
  // TODO: other filed may need to change as well

  _enable_full_timing_update();
}

// Function: resistance_unit
Timer& Timer::resistance_unit(ohm_t unit) {

  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.silent_emplace([this, unit=std::move(unit)] () {
    _to_resistance_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_resistance_unit
void Timer::_to_resistance_unit(const ohm_t& unit) {
  
  OT_LOGI("use resistance unit ", unit);

  float s = (_resistance_unit) ? (*_resistance_unit / unit).value() : 1.0f;

  if(_resistance_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
    return;
  }

  // scale net resistance
  for(auto& kvp : _nets) {
    kvp.second._scale_resistance(s);
  }
  
  // library resistance
  FOR_EACH_EL(el) {
    _celllib[el].to_resistance_unit(unit);
  }
  
  // TODO:
  _enable_full_timing_update();
} 

// Function: voltage_unit
Timer& Timer::voltage_unit(volt_t unit) {

  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.silent_emplace([this, unit=std::move(unit)] () {
    _to_voltage_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_voltage_unit
void Timer::_to_voltage_unit(const volt_t& unit) {
  
  OT_LOGI("use voltage unit ", unit);

  float s = (_voltage_unit) ? (*_voltage_unit/unit).value() : 1.0f;

  if(_voltage_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
    return;
  }

  // TODO:

  _enable_full_timing_update();
}

// Function: current_unit
Timer& Timer::current_unit(ampere_t unit) {

  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.silent_emplace([this, unit=std::move(unit)] () {
    _to_current_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_current_unit
void Timer::_to_current_unit(const ampere_t& unit) {
  
  OT_LOGI("use current unit ", unit);

  float s = (_current_unit) ? (*_current_unit / unit).value() : 1.0f;

  if(_current_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
    return;
  }

  // TODO:

  _enable_full_timing_update();
}

// Function: power_unit
Timer& Timer::power_unit(watt_t unit) {

  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.silent_emplace([this, unit=std::move(unit)] () {
    _to_power_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_power_unit
void Timer::_to_power_unit(const watt_t& unit) {
  
  OT_LOGI("use power unit ", unit);

  float s = (_power_unit) ? (*_power_unit/unit).value() : 1.0f;

  if(_power_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
    return;
  }

  // TODO:

  _enable_full_timing_update();
}

/*
    
// Procedure: _rebase_unit
void Timer::_rebase_unit(Celllib& lib) {
      
  // Convert the time unit.
  if(!_time_unit) {
    if(_time_unit = lib.time_unit) {
      OT_LOGI("use celllib time unit ", std::quoted(dump_time_unit(*_time_unit)));
    }
  }
  else {
    lib.to_time_unit(*_time_unit);
  }

  // Convert the capacitive load unit
  if(!_capacitance_unit) {
    if(_capacitance_unit = lib.capacitance_unit) {
      OT_LOGI(
        "use celllib capacitance unit ", 
        std::quoted(dump_capacitance_unit(*_capacitance_unit))
      );
    }
  }
  else {
    lib.to_capacitance_unit(*_capacitance_unit);
  }

  // Conver the current unit.
  if(!_current_unit) {
    if(_current_unit = lib.current_unit) {
      OT_LOGI(
        "use celllib current unit ", 
        std::quoted(dump_current_unit(*_current_unit))
      );
    }
  }
  else {
    lib.to_current_unit(*_current_unit);
  }
  
  // Conver the voltage unit.
  if(!_voltage_unit) {
    if(_voltage_unit = lib.voltage_unit) {
      OT_LOGI(
        "use celllib voltage unit ", 
        std::quoted(dump_voltage_unit(*_voltage_unit))
      );
    }
  }
  else {
    lib.to_voltage_unit(*_voltage_unit);
  }
  
  // Conver the resistance unit.
  if(!_resistance_unit) {
    if(_resistance_unit = lib.resistance_unit) {
      OT_LOGI(
        "use celllib resistance unit ", 
        std::quoted(dump_resistance_unit(*_resistance_unit))
      );
    }
  }
  else {
    lib.to_resistance_unit(*_resistance_unit);
  }
  
  // Conver the power unit.
  if(!_power_unit) {
    if(_power_unit = lib.power_unit) {
      OT_LOGI(
        "use celllib power unit ", 
        std::quoted(dump_power_unit(*_power_unit))
      );
    }
  }
  else {
    lib.to_power_unit(*_power_unit);
  }
}

// Procedure: _rebase_unit
void Timer::_rebase_unit(spef::Spef& spef) {
      
  // Convert the capacitive load unit
  if(!_capacitance_unit) {
    if(_capacitance_unit = spef.capacitance_unit) {
      OT_LOGI(
        "use spef capacitance unit ", 
        std::quoted(dump_capacitance_unit(*_capacitance_unit))
      );
    }
  }
  else {
    spef.to_capacitance_unit(*_capacitance_unit);
  }
  
  // Conver the resistance unit.
  if(!_resistance_unit) {
    if(_resistance_unit = spef.resistance_unit) {
      OT_LOGI(
        "use spef resistance unit ", 
        std::quoted(dump_resistance_unit(*_resistance_unit))
      );
    }
  }
  else {
    spef.to_resistance_unit(*_resistance_unit);
  }
} */

}; // end of namespace ot -------------------------------------------------------------------------





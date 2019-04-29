#include <ot/timer/timer.hpp>

// TODO (twhuang)
// (1) extend taskflow's capability to enable intra-task parallelization

namespace ot {

// Function: set_time_unit
Timer& Timer::set_time_unit(second_t unit) {
  
  std::scoped_lock lock(_mutex);

  // timer task
  auto task = _taskflow.emplace([this, unit=std::move(unit)] () {
    _to_time_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_time_unit
void Timer::_to_time_unit(const second_t& unit) {
  
  OT_LOGI("use time unit ", unit);

  float s = (_time_unit) ? (*_time_unit / unit).value() : 1.0f;

  if(_time_unit = unit; std::fabs(s - 1.0f) < 1e-2f) {
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
  FOR_EACH_EL_IF(el, _celllib[el]) {
    _celllib[el]->scale_time(s);
  }
  
  // enable full timing update
  _enable_full_timing_update();
}


// Function: set_capacitance_unit
Timer& Timer::set_capacitance_unit(farad_t unit) {
  
  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.emplace([this, unit=std::move(unit)] () {
    _to_capacitance_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_capacitance_unit
void Timer::_to_capacitance_unit(const farad_t& unit) {
  
  OT_LOGI("use capacitance unit ", unit);

  float s = (_capacitance_unit) ? (*_capacitance_unit / unit).value() : 1.0f;

  if(_capacitance_unit = unit; std::fabs(s - 1.0f) < 1e-2f) {
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
  FOR_EACH_EL_IF(el, _celllib[el]) {
    _celllib[el]->scale_capacitance(s);
  }
  
  // TODO: other filed may need to change as well

  _enable_full_timing_update();
}

// Function: set_resistance_unit
Timer& Timer::set_resistance_unit(ohm_t unit) {

  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.emplace([this, unit=std::move(unit)] () {
    _to_resistance_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_resistance_unit
void Timer::_to_resistance_unit(const ohm_t& unit) {
  
  OT_LOGI("use resistance unit ", unit);

  float s = (_resistance_unit) ? (*_resistance_unit / unit).value() : 1.0f;

  if(_resistance_unit = unit; std::fabs(s - 1.0f) < 1e-2f) {
    return;
  }

  // scale net resistance
  for(auto& kvp : _nets) {
    kvp.second._scale_resistance(s);
  }
  
  // library resistance
  FOR_EACH_EL_IF(el, _celllib[el]) {
    _celllib[el]->scale_resistance(s);
  }
  
  // TODO:
  _enable_full_timing_update();
} 

// Function: set_voltage_unit
Timer& Timer::set_voltage_unit(volt_t unit) {

  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.emplace([this, unit=std::move(unit)] () {
    _to_voltage_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_voltage_unit
void Timer::_to_voltage_unit(const volt_t& unit) {
  
  OT_LOGI("use voltage unit ", unit);

  float s = (_voltage_unit) ? (*_voltage_unit/unit).value() : 1.0f;

  if(_voltage_unit = unit; std::fabs(s - 1.0f) < 1e-2f) {
    return;
  }

  // TODO:

  _enable_full_timing_update();
}

// Function: set_current_unit
Timer& Timer::set_current_unit(ampere_t unit) {

  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.emplace([this, unit=std::move(unit)] () {
    _to_current_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_current_unit
void Timer::_to_current_unit(const ampere_t& unit) {
  
  OT_LOGI("use current unit ", unit);

  float s = (_current_unit) ? (*_current_unit / unit).value() : 1.0f;

  if(_current_unit = unit; std::fabs(s - 1.0f) < 1e-2f) {
    return;
  }

  // TODO:

  _enable_full_timing_update();
}

// Function: set_power_unit
Timer& Timer::set_power_unit(watt_t unit) {

  std::scoped_lock lock(_mutex);

  // task
  auto task = _taskflow.emplace([this, unit=std::move(unit)] () {
    _to_power_unit(unit);
  });

  _add_to_lineage(task);

  return *this;
}

// Procedure: _to_power_unit
void Timer::_to_power_unit(const watt_t& unit) {
  
  OT_LOGI("use power unit ", unit);

  float s = (_power_unit) ? (*_power_unit/unit).value() : 1.0f;

  if(_power_unit = unit; std::fabs(s - 1.0f) < 1e-2f) {
    return;
  }

  // TODO:

  _enable_full_timing_update();
}

// Procedure: _rebase_unit
void Timer::_rebase_unit(Celllib& lib) {
      
  // Convert the time unit.
  if(!_time_unit) {
    if(_time_unit = lib.time_unit; _time_unit) {
      OT_LOGI("use celllib time unit ", *_time_unit);
    }
  }
  else if(lib.time_unit){
    float s = (*lib.time_unit / *_time_unit).value(); 
    if(std::fabs(s - 1.0f) >= 1e-2f) {
      OT_LOGI("rebase celllib ", lib.name, " time to ", *_time_unit);
      lib.scale_time(s);
    }
  }

  // Convert the capacitance unit
  if(!_capacitance_unit) {
    if(_capacitance_unit = lib.capacitance_unit; _capacitance_unit) {
      OT_LOGI("use celllib capacitance unit ", *_capacitance_unit);
    }
  }
  else if(lib.capacitance_unit) {
    float s = (*lib.capacitance_unit / *_capacitance_unit).value();
    if(std::fabs(s - 1.0f) >= 1e-2f) {
      OT_LOGI("rebase celllib ", lib.name, " capacitance to ", *_capacitance_unit);
      lib.scale_capacitance(s);
    }
  }

  // Conver the current unit.
  if(!_current_unit) {
    if(_current_unit = lib.current_unit; _current_unit) {
      OT_LOGI("use celllib current unit ", *_current_unit);
    }
  }
  else if(lib.current_unit) {
    float s = (*lib.current_unit / *_current_unit).value();
    if(std::fabs(s - 1.0f) >= 1e-2f) {
      OT_LOGI("rebase celllib ", lib.name, " current to ", *_current_unit);
      lib.scale_current(s);
    }
  }
  
  // Conver the voltage unit.
  if(!_voltage_unit) {
    if(_voltage_unit = lib.voltage_unit; _voltage_unit) {
      OT_LOGI("use celllib voltage unit ", *_voltage_unit);
    }
  }
  else if(lib.voltage_unit) {
    float s = (*lib.voltage_unit / *_voltage_unit).value();
    if(std::fabs(s - 1.0f) >= 1e-2f) {
      OT_LOGI("rebase celllib ", lib.name, " voltage to ", *_voltage_unit);
      lib.scale_voltage(s);
    }
  }
  
  // Conver the resistance unit.
  if(!_resistance_unit) {
    if(_resistance_unit = lib.resistance_unit; _resistance_unit) {
      OT_LOGI("use celllib resistance unit ", *_resistance_unit);
    }
  }
  else if(lib.resistance_unit){
    float s = (*lib.resistance_unit / *_resistance_unit).value();
    if(std::fabs(s - 1.0f) >= 1e-2f) {
      OT_LOGI("rebase celllib ", lib.name, " resistance to ", *_resistance_unit);
      lib.scale_resistance(s);
    }
  }
  
  // Conver the power unit.
  if(!_power_unit) {
    if(_power_unit = lib.power_unit; _power_unit) {
      OT_LOGI("use celllib power unit ", *_power_unit);
    }
  }
  else if(lib.power_unit) {
    float s = (*lib.power_unit / *_power_unit).value();
    if(std::fabs(s - 1.0f) >= 1e-2f) {
      OT_LOGI("rebase celllib ", lib.name, " power to ", *_power_unit);
      lib.scale_power(s);
    }
  }

}

// Procedure: _rebase_unit
void Timer::_rebase_unit(spef::Spef& spef) {

  auto resu = make_resistance_unit(to_lower(spef.resistance_unit));
  auto capu = make_capacitance_unit(to_lower(spef.capacitance_unit));

  // Convert the capacitive load unit
  if(!_capacitance_unit) {
    if(_capacitance_unit = capu; _capacitance_unit) {
      OT_LOGI("use spef capacitance unit ", *_capacitance_unit);
    }
  }
  else if(capu) {
    float s = (*capu / *_capacitance_unit).value();
    if(std::fabs(s - 1.0f) >= 1e-2f) {
      OT_LOGI("rebase spef capacitance to ", *capu);
      spef.scale_capacitance(s);
    }
  }

  // conver teh resistance unit
  if(!_resistance_unit) {
    if(_resistance_unit = resu; _resistance_unit) {
      OT_LOGI("use spef resistance unit ", *_resistance_unit);
    }
  }
  else if(resu){
    float s = (*resu / *_resistance_unit).value();
    if(std::fabs(s - 1.0f) >= 1e-2f) {
      OT_LOGI("rebase spef resistance to ", *resu);
      spef.scale_resistance(s);
    }
  }
}

}; // end of namespace ot -------------------------------------------------------------------------





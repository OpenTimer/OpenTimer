#include <ot/liberty/timing.hpp>

namespace ot {

// Function: to_string
std::string to_string(TimingSense sense) {

  switch(sense) {

    case TimingSense::NEGATIVE_UNATE:
      return "negative_unate";
    break;

    case TimingSense::POSITIVE_UNATE:
      return "positive_unate";
    break;

    case TimingSense::NON_UNATE:
      return "non_unate";
    break;

    default:
      return "undefined";
    break;
  }
}

// Function: timing_type_name
std::string to_string(TimingType t) {

  switch (t) {

    case TimingType::COMBINATIONAL:
      return "combinational";
    break;

    case TimingType::COMBINATIONAL_RISE:
      return "combinational_rise";
    break;

    case TimingType::COMBINATIONAL_FALL:
      return "combinational_fall";
    break;

    case TimingType::THREE_STATE_DISABLE:
      return "three_state_disable";
    break;

    case TimingType::THREE_STATE_DISABLE_RISE:
      return "three_state_disable_rise";
    break;

    case TimingType::THREE_STATE_DISABLE_FALL:
      return "three_state_disable_fall";
    break;

    case TimingType::THREE_STATE_ENABLE:
      return "three_state_enable";
    break;

    case TimingType::THREE_STATE_ENABLE_RISE:
      return "three_state_enable_rise";
    break;

    case TimingType::THREE_STATE_ENABLE_FALL:
      return "three_state_enable_fall";
    break;

    case TimingType::RISING_EDGE:
      return "rising_edge";
    break;

    case TimingType::FALLING_EDGE:
      return "falling_edge";
    break;

    case TimingType::PRESET:
      return "preset";
    break;

    case TimingType::CLEAR:
      return "clear";
    break;

    case TimingType::HOLD_RISING:
      return "hold_rising";
    break;

    case TimingType::HOLD_FALLING:
      return "hold_falling"; 
    break;

    case TimingType::SETUP_RISING:
      return "setup_rising";
    break;

    case TimingType::SETUP_FALLING:
      return "setup_falling";
    break;

    case TimingType::RECOVERY_RISING:
      return "recovery_rising";
    break;

    case TimingType::RECOVERY_FALLING:
      return "recovery_falling";
    break;

    case TimingType::SKEW_RISING:
      return "skew_rising";
    break;

    case TimingType::SKEW_FALLING:
      return "skew_falling";
    break;

    case TimingType::REMOVAL_RISING:
      return "removal_rising";
    break;

    case TimingType::REMOVAL_FALLING:
      return "removal_falling";
    break;

    case TimingType::MIN_PULSE_WIDTH:
      return "min_pulse_width";
    break;
  
    case TimingType::MINIMUM_PERIOD:
      return "minimum_period";
    break;

    case TimingType::MAX_CLOCK_TREE_PATH:
      return "max_clock_tree_path";
    break;

    case TimingType::MIN_CLOCK_TREE_PATH:
      return "min_clock_tree_path";
    break;
  
    case TimingType::NON_SEQ_SETUP_RISING:
      return "non_seq_setup_rising";
    break;

    case TimingType::NON_SEQ_SETUP_FALLING:
      return "non_seq_setup_falling";
    break;

    case TimingType::NON_SEQ_HOLD_RISING:
      return "non_seq_hold_rising";
    break;

    case TimingType::NON_SEQ_HOLD_FALLING:
      return "non_seq_hold_falling";
    break;

    case TimingType::NOCHANGE_HIGH_HIGH:
      return "nochange_high_high";
    break;

    case TimingType::NOCHANGE_HIGH_LOW:
      return "nochange_high_low";
    break;

    case TimingType::NOCHANGE_LOW_HIGH:
      return "nochange_low_high";
    break;

    case TimingType::NOCHANGE_LOW_LOW:
      return "nochange_low_low";
    break;

    default:
      return "undefined";
    break;
  }
}

// ------------------------------------------------------------------------------------------------

// Function: isomorphic
bool Timing::isomorphic(const Timing& rhs) const {
  
  if(related_pin != rhs.related_pin) return false;
  if(sense != rhs.sense)             return false;
  if(type != rhs.type)               return false;
  if(cell_rise.has_value() != rhs.cell_rise.has_value()) return false;
  if(cell_fall.has_value() != rhs.cell_fall.has_value()) return false;
  if(rise_transition.has_value() != rhs.rise_transition.has_value()) return false;
  if(fall_transition.has_value() != rhs.fall_transition.has_value()) return false;
  if(rise_constraint.has_value() != rhs.rise_constraint.has_value()) return false;
  if(fall_constraint.has_value() != rhs.fall_constraint.has_value()) return false;

  return true;
}

// Function: is_transition_defined
bool Timing::is_transition_defined(Tran irf, Tran orf) const {

  if(!is_input_transition_defined(irf)) return false;
  
  if(sense) {
    switch(*sense) {
      case TimingSense::POSITIVE_UNATE:
        if(irf != orf) return false;
      break;

      case TimingSense::NEGATIVE_UNATE:
        if(irf == orf) return false;
      break;

      default:
      break;
    }
  }

  return true;
}

// Function: is_input_transition_defined
bool Timing::is_input_transition_defined(Tran irf) const {
  if(is_rising_edge_triggered() && irf != RISE) return false;
  if(is_falling_edge_triggered() && irf != FALL) return false;
  return true;
}

// Function: is_input_transition_defined
bool Timing::is_input_transition_defined() const {
  return (is_falling_edge_triggered() || is_rising_edge_triggered());
}

// Function: is_constraint
bool Timing::is_constraint() const {
  if(type) {
    switch(*type) {
      case TimingType::REMOVAL_RISING:
      case TimingType::REMOVAL_FALLING:
      case TimingType::RECOVERY_RISING:
      case TimingType::RECOVERY_FALLING:
      case TimingType::SETUP_RISING:
      case TimingType::SETUP_FALLING:
      case TimingType::HOLD_RISING:
      case TimingType::HOLD_FALLING:
      case TimingType::NON_SEQ_SETUP_RISING:
      case TimingType::NON_SEQ_SETUP_FALLING:
      case TimingType::NON_SEQ_HOLD_RISING:
      case TimingType::NON_SEQ_HOLD_FALLING:
        return true;
      break;

      default:
        return false;
      break;
    }
  }
  else {
    return false;
  }
}

// Function: is_min_constraint
bool Timing::is_min_constraint() const {
  if(type) {
    switch(*type) {
      case TimingType::HOLD_RISING:
      case TimingType::HOLD_FALLING:
      case TimingType::NON_SEQ_HOLD_RISING:
      case TimingType::NON_SEQ_HOLD_FALLING:
      case TimingType::REMOVAL_RISING:
      case TimingType::REMOVAL_FALLING:
        return true;
      break;

      default:
        return false;
      break;
    }
  }
  else {
    return false;
  }
}

// Function: is_max_constraint
bool Timing::is_max_constraint() const {
  if(type) {
    switch(*type) {
      case TimingType::SETUP_RISING:
      case TimingType::SETUP_FALLING:
      case TimingType::NON_SEQ_SETUP_RISING:
      case TimingType::NON_SEQ_SETUP_FALLING:
      case TimingType::RECOVERY_RISING:
      case TimingType::RECOVERY_FALLING:
        return true;
      break;

      default:
        return false;
      break;
    }
  }
  else {
    return false;
  }
}

// Function: is_falling_edge_triggered
bool Timing::is_falling_edge_triggered() const {
  if(type) {
    switch (*type) {
      case TimingType::SETUP_FALLING:
      case TimingType::HOLD_FALLING:
      case TimingType::REMOVAL_FALLING:
      case TimingType::RECOVERY_FALLING:
      case TimingType::FALLING_EDGE:
        return true;
      break;

      default:
        return false;
      break;
    };
  }
  else {
    return false;
  }
}

// Function: is_rising_edge_triggered
bool Timing::is_rising_edge_triggered() const {
  if(type) {
    switch (*type) {
      case TimingType::SETUP_RISING:
      case TimingType::HOLD_RISING:
      case TimingType::REMOVAL_RISING:
      case TimingType::RECOVERY_RISING:
      case TimingType::RISING_EDGE:
        return true;
      break;

      default:
        return false;
      break;
    };
  }
  else {
    return false;
  }
}

// Procedure: scale_time
void Timing::scale_time(float s) {

  if(cell_rise) {
    cell_rise->scale_time(s);
  }
  
  if(cell_fall) {
    cell_fall->scale_time(s);
  }

  if(rise_transition) {
    rise_transition->scale_time(s);
  }

  if(fall_transition) {
    fall_transition->scale_time(s);
  }

  if(rise_constraint) {
    rise_constraint->scale_time(s);
  }

  if(fall_constraint) {
    fall_constraint->scale_time(s);
  }
}

// Procedure: scale_capacitance
void Timing::scale_capacitance(float s) {

  if(cell_rise) {
    cell_rise->scale_capacitance(s);
  }
  
  if(cell_fall) {
    cell_fall->scale_capacitance(s);
  }

  if(rise_transition) {
    rise_transition->scale_capacitance(s);
  }

  if(fall_transition) {
    fall_transition->scale_capacitance(s);
  }

  if(rise_constraint) {
    rise_constraint->scale_capacitance(s);
  }

  if(fall_constraint) {
    fall_constraint->scale_capacitance(s);
  }
}

// Function: delay
// Query the delay which is referenced by the output transition status, input slew, and driving 
// load. The output transition status indicates the type of lut that should be used during the
// linear interpolation or linear extrapolation.
std::optional<float> Timing::delay(Tran irf, Tran orf, float slew, float load) const {

  if(!is_transition_defined(irf, orf)) {
    return std::nullopt;
  }
  
  const Lut* lut {nullptr};

  switch(orf) {
    case RISE:
      lut = cell_rise ? &(cell_rise.value()) : nullptr;
    break;

    case FALL:
      lut = cell_fall ? &(cell_fall.value()) : nullptr;
    break;

    default:
      assert(false);
    break;
  };
  
  if(lut == nullptr) {
    return std::nullopt;
  }
  
  // Case 1: scalar.
  if(lut->lut_template == nullptr) {     
    if(lut->is_scalar()) {
      return lut->table[0];
    }
    else {
      OT_LOGF("lut without template must contain a single scalar");
    }
  }

  // Case 2: non-scalar table.
  float val1 {.0f}, val2 {.0f};
  
  // - obtain the input numerics
  assert(lut->lut_template->variable1);

  switch(*(lut->lut_template->variable1)) {

    case LutVar::TOTAL_OUTPUT_NET_CAPACITANCE:
      if(lut->lut_template->variable2) {
        assert(lut->lut_template->variable2 == LutVar::INPUT_NET_TRANSITION);
      }
      val1 = load;
      val2 = slew;
    break;

    case LutVar::INPUT_NET_TRANSITION:
      if(lut->lut_template->variable2) {
        assert(lut->lut_template->variable2 == LutVar::TOTAL_OUTPUT_NET_CAPACITANCE);
      }
      val1 = slew;
      val2 = load;
    break;

    default:
      OT_LOGF("invalid delay lut template variable");
    break;
  };
  
  // - perform the linear inter/extro-polation on indices1 and indices2
  return (*lut)(val1, val2); 
}

// Function: slew
// Query the slew which is referenced by the output transition status, input slew, and driving 
// load. The output transition status indicates the type of lut that should be used during the
// linear interpolation or linear extrapolation.
std::optional<float> Timing::slew(Tran irf, Tran orf, float slew, float load) const {
  
  if(!is_transition_defined(irf, orf)) {
    return std::nullopt;
  }
  
  const Lut* lut {nullptr};

  switch(orf) {

    case RISE:
      lut = rise_transition ? &(rise_transition.value()) : nullptr;
    break;

    case FALL:
      lut = fall_transition ? &(fall_transition.value()) : nullptr;
    break;

    default:
      assert(false);
    break;
  };
  
  // No slew lut
  if(lut == nullptr) {
    return std::nullopt;
  }
  
  // Case 1: scalar.
  if(lut->lut_template == nullptr) {     
    if(lut->is_scalar()) {
      return lut->table[0];
    }
    else {
      OT_LOGF("lut without template must contain a single scalar");
    }
  }

  // Case 2: non-scalar table.
  float val1 {0.0f}, val2 {0.0f};
  
  // - obtain the input numerics
  assert(lut->lut_template->variable1);

  switch(*(lut->lut_template->variable1)) {

    case LutVar::TOTAL_OUTPUT_NET_CAPACITANCE:
      if(lut->lut_template->variable2) {
        assert(*(lut->lut_template->variable2) == LutVar::INPUT_NET_TRANSITION);
      }
      val1 = load;
      val2 = slew;
    break;

    case LutVar::INPUT_NET_TRANSITION:
      if(lut->lut_template->variable2) {
        assert(*(lut->lut_template->variable2) == LutVar::TOTAL_OUTPUT_NET_CAPACITANCE);
      }
      val1 = slew;
      val2 = load;
    break;

    default:
      OT_LOGF("invalid slew lut template variable");
    break;
  }
  
  // - perform the linear inter/extro-polation on indices1 and indices2
  return (*lut)(val1, val2); 
}

// Function: constraint
// Query the constraint which is referenced by the output transition status, input slew, and
// output slew. The output transition status indicates the type of lut that should be used 
// during the linear interpolation or linear extrapolation.
std::optional<float> Timing::constraint(
  Tran irf, 
  Tran orf, 
  float related_slew, 
  float constrained_slew
) const {
  
  if(!is_transition_defined(irf, orf)) {
    return std::nullopt;
  }
  
  const Lut* lut {nullptr};

  switch(orf) {

    case RISE:
      lut = rise_constraint ? &(rise_constraint.value()) : nullptr;
    break;

    case FALL:
      lut = fall_constraint ? &(fall_constraint.value()) : nullptr;
    break;

    default:
      assert(false);
    break;
  };
  
  if(lut == nullptr) {
    return std::nullopt;
  }
  
  // Case 1: scalar.
  if(lut->lut_template == nullptr) {     
    if(lut->is_scalar()) {
      return lut->table[0];
    }
    else {
      OT_LOGF("lut without template must contain a single scalar");
    }
  }
  
  // Case 2: non-scalar table.
  float val1 {0.0f}, val2 {0.0f};
  
  // - obtain the input numerics
  assert(lut->lut_template->variable1);

  switch(*(lut->lut_template->variable1)) {

    case LutVar::CONSTRAINED_PIN_TRANSITION:
      if(lut->lut_template->variable2) {
        assert(lut->lut_template->variable2 == LutVar::RELATED_PIN_TRANSITION);
      }
      val1 = constrained_slew;
      val2 = related_slew;
    break;

    case LutVar::RELATED_PIN_TRANSITION:
      if(lut->lut_template->variable2) {
        assert(lut->lut_template->variable2 == LutVar::CONSTRAINED_PIN_TRANSITION);
      }
      val1 = related_slew;
      val2 = constrained_slew;
    break;

    default:
      OT_LOGF("invalid constraint lut template variable");
    break;
  };
  
  // - perform the linear inter/extro-polation on indices1 and indices2
  return (*lut)(val1, val2); 
}

// operator
std::ostream& operator << (std::ostream& os, const Timing& timing) {
  
  // Write the timing.
  os << "    timing () {\n";

  // Write the related pin (from cellpin).
  os << "      related_pin : \"" << timing.related_pin << "\";\n";

  // Write the timing sense.
  if(timing.sense) {
    os << "      timing_sense : " << to_string(*timing.sense) << ";\n";
  }

  // Write the timing type.
  if(timing.type) {
    os << "      timing_type : " << to_string(*timing.type) << ";\n";
  }

  // Write cell_rise
  if(timing.cell_rise) {
    os << "      cell_rise (\"" << timing.cell_rise->name << "\") {\n"; 
    os << *(timing.cell_rise);
    os << "      }\n";
  }

  // Write cell_fall
  if(timing.cell_fall) {
    os << "      cell_fall (\"" << timing.cell_fall->name << "\") {\n"; 
    os << *(timing.cell_fall);
    os << "      }\n";
  }

  // Write rise_transition
  if(timing.rise_transition) {
    os << "      rise_transition (\"" << timing.rise_transition->name << "\") {\n"; 
    os << *(timing.rise_transition);
    os << "      }\n";
  }
  
  // Write fall_transition
  if(timing.fall_transition) {
    os << "      fall_transition (\"" << timing.fall_transition->name << "\") {\n"; 
    os << *(timing.fall_transition);
    os << "      }\n";
  }
  
  // Write rise_constraint
  if(timing.rise_constraint) {
    os << "      rise_constraint (\"" << timing.rise_constraint->name << "\") {\n"; 
    os << *(timing.rise_constraint);
    os << "      }\n";
  }
  
  // Write fall_constraint
  if(timing.fall_constraint) {
    os << "      fall_constraint (\"" << timing.fall_constraint->name << "\") {\n"; 
    os << *(timing.fall_constraint);
    os << "      }\n";
  }
  
  // Write the ending group symbol.
  os << "    }\n";

  return os;
}

};  // end of namespace ot. -----------------------------------------------------------------------

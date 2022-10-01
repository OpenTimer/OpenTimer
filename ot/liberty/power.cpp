#include <ot/liberty/power.hpp>

namespace ot {

void InternalPower::scale_time(float s) {

  if(rise_power) {
    rise_power->scale_time(s);
  }

  if(fall_power) {
    fall_power->scale_time(s);
  }
}

void InternalPower::scale_capacitance(float s) {

  if(rise_power) {
    rise_power->scale_capacitance(s);
  }

  if(fall_power) {
    fall_power->scale_capacitance(s);
  }
}

std::optional<float> InternalPower::power(Tran irf, Tran orf, float time, float load) const {

  const Lut* lut {nullptr};

  switch(orf) {
    case RISE:
      lut = rise_power ? &(rise_power.value()) : nullptr;
    break;

    case FALL:
      lut = fall_power ? &(fall_power.value()) : nullptr;
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
        assert(lut->lut_template->variable2 == LutVar::INPUT_TRANSITION_TIME);
      }
      val1 = load;
      val2 = time;
    break;

    case LutVar::INPUT_TRANSITION_TIME:
      if(lut->lut_template->variable2) {
        assert(lut->lut_template->variable2 == LutVar::TOTAL_OUTPUT_NET_CAPACITANCE);
      }
      val1 = time;
      val2 = load;
    break;

    default:
      OT_LOGF("invalid power lut template variable");
    break;
  };

  // - perform the linear inter/extro-polation on indices1 and indices2
  return (*lut)(val1, val2);
}

std::ostream& operator << (std::ostream& os, const InternalPower& power) {

  // Write the timing.
  os << "    internal_power () {\n";

  // Write the related pin (from cellpin).
  os << "      related_pin : \"" << power.related_pin << "\";\n";

  if(power.rise_power) {
    os << "      rise_power (\"" << power.rise_power->name << "\") {\n";
    os << *(power.rise_power);
    os << "      }\n";
  }

  if(power.fall_power) {
    os << "      fall_power (\"" << power.fall_power->name << "\") {\n";
    os << *(power.fall_power);
    os << "      }\n";
  }

  // Write the ending group symbol.
  os << "    }\n";

  return os;
}

};  // end of namespace ot. -----------------------------------------------------------------------



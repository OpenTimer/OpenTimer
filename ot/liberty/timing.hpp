#ifndef OT_LIBERTY_TIMING_HPP_
#define OT_LIBERTY_TIMING_HPP_

#include <ot/liberty/lut.hpp>

namespace ot {

enum class TimingSense {
  NON_UNATE = 0,
  POSITIVE_UNATE,
  NEGATIVE_UNATE
};

enum class TimingType {
  COMBINATIONAL = 0,
  COMBINATIONAL_RISE,
  COMBINATIONAL_FALL,
  THREE_STATE_DISABLE,
  THREE_STATE_DISABLE_RISE,
  THREE_STATE_DISABLE_FALL,
  THREE_STATE_ENABLE,
  THREE_STATE_ENABLE_RISE,
  THREE_STATE_ENABLE_FALL,
  RISING_EDGE,
  FALLING_EDGE,
  PRESET,
  CLEAR,
  HOLD_RISING,
  HOLD_FALLING,
  SETUP_RISING,
  SETUP_FALLING,
  RECOVERY_RISING,
  RECOVERY_FALLING,
  SKEW_RISING,
  SKEW_FALLING,
  REMOVAL_RISING,
  REMOVAL_FALLING,
  MIN_PULSE_WIDTH,
  MINIMUM_PERIOD,
  MAX_CLOCK_TREE_PATH,
  MIN_CLOCK_TREE_PATH,
  NON_SEQ_SETUP_RISING,
  NON_SEQ_SETUP_FALLING,
  NON_SEQ_HOLD_RISING,
  NON_SEQ_HOLD_FALLING,
  NOCHANGE_HIGH_HIGH,
  NOCHANGE_HIGH_LOW,
  NOCHANGE_LOW_HIGH,
  NOCHANGE_LOW_LOW,
};
  
inline const std::unordered_map<std::string_view, TimingType> timing_types = {
  {"combinational",            TimingType::COMBINATIONAL},
  {"combinational_rise",       TimingType::COMBINATIONAL_RISE},
  {"combinational_fall",       TimingType::COMBINATIONAL_FALL},
  {"three_state_disable",      TimingType::THREE_STATE_DISABLE},
  {"three_state_disable_rise", TimingType::THREE_STATE_DISABLE_RISE},
  {"three_state_disable_fall", TimingType::THREE_STATE_DISABLE_FALL},
  {"three_state_enable",       TimingType::THREE_STATE_ENABLE},
  {"three_state_enable_rise",  TimingType::THREE_STATE_ENABLE_RISE},
  {"three_state_enable_fall",  TimingType::THREE_STATE_ENABLE_FALL},
  {"rising_edge",              TimingType::RISING_EDGE},
  {"falling_edge",             TimingType::FALLING_EDGE},
  {"preset",                   TimingType::PRESET},
  {"clear",                    TimingType::CLEAR},
  {"hold_rising",              TimingType::HOLD_RISING},
  {"hold_falling",             TimingType::HOLD_FALLING},
  {"setup_rising",             TimingType::SETUP_RISING},
  {"setup_falling",            TimingType::SETUP_FALLING},
  {"recovery_rising",          TimingType::RECOVERY_RISING},
  {"recovery_falling",         TimingType::RECOVERY_FALLING},
  {"skew_rising",              TimingType::SKEW_RISING},
  {"skew_falling",             TimingType::SKEW_FALLING},
  {"removal_rising",           TimingType::REMOVAL_RISING},
  {"removal_falling",          TimingType::REMOVAL_FALLING},
  {"min_pulse_width",          TimingType::MIN_PULSE_WIDTH},
  {"minimum_period",           TimingType::MINIMUM_PERIOD},
  {"max_clock_tree_path",      TimingType::MAX_CLOCK_TREE_PATH},
  {"min_clock_tree_path",      TimingType::MIN_CLOCK_TREE_PATH},
  {"non_seq_setup_rising",     TimingType::NON_SEQ_SETUP_RISING},
  {"non_seq_setup_falling",    TimingType::NON_SEQ_SETUP_FALLING},
  {"non_seq_hold_rising",      TimingType::NON_SEQ_HOLD_RISING},
  {"non_seq_hold_falling",     TimingType::NON_SEQ_HOLD_FALLING},
  {"nochange_high_high",       TimingType::NOCHANGE_HIGH_HIGH},
  {"nochange_high_low",        TimingType::NOCHANGE_HIGH_LOW},
  {"nochange_low_high",        TimingType::NOCHANGE_LOW_HIGH},
  {"nochange_low_low",         TimingType::NOCHANGE_LOW_LOW}
};

// Function: to_string
std::string to_string(TimingSense);
std::string to_string(TimingType);

// Struct: Timing
struct Timing {
  
  std::string related_pin;

  std::optional<TimingSense> sense;         
  std::optional<TimingType> type;           
  std::optional<Lut> cell_rise;             
  std::optional<Lut> cell_fall;             
  std::optional<Lut> rise_transition;       
  std::optional<Lut> fall_transition;       
  std::optional<Lut> rise_constraint;       
  std::optional<Lut> fall_constraint;       

  bool is_combinational() const;
  bool is_constraint() const; 
  bool is_min_constraint() const;
  bool is_max_constraint() const;
  bool is_rising_edge_triggered() const;
  bool is_falling_edge_triggered() const;
  bool is_input_transition_defined() const;
  bool is_input_transition_defined(Tran) const;
  bool is_output_transition_defined(Tran) const;
  bool is_transition_defined(Tran, Tran) const;
  bool isomorphic(const Timing&) const;

  void scale_time(float);
  void scale_capacitance(float);

  std::optional<float> delay(Tran, Tran, float, float) const;
  std::optional<float> slew(Tran, Tran, float, float) const;
  std::optional<float> constraint(Tran, Tran, float, float) const;
};

std::ostream& operator << (std::ostream&, const Timing&);

// Alias
using TimingView = TimingData<const Timing*, MAX_SPLIT>;


};  // end of namespace ot. -----------------------------------------------------------------------


#endif

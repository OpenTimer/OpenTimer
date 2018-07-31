#ifndef OT_LIBERTY_LIBERTY_HPP_
#define OT_LIBERTY_LIBERTY_HPP_

#include <ot/liberty/cell.hpp>
#include <ot/unit/unit.hpp>

namespace ot {

// Enum: DelayModel
enum class DelayModel {
  GENERIC_CMOS,
  TABLE_LOOKUP,
  CMOS2,
  PIECEWISE_CMOS,
  DCM,
  POLYNOMIAL
};

inline const std::unordered_map<std::string, DelayModel> delay_models {
  {"generic_cmos",   DelayModel::GENERIC_CMOS},
  {"table_lookup",   DelayModel::TABLE_LOOKUP},
  {"cmos2",          DelayModel::CMOS2},
  {"piecewise_cmos", DelayModel::PIECEWISE_CMOS},
  {"dcm",            DelayModel::DCM},
  {"polynomial",     DelayModel::POLYNOMIAL}
};

// Function: to_string
std::string to_string(DelayModel);

// Class: Celllib
struct Celllib {
  
  using token_iterator = std::vector<std::string>::iterator;

  std::string name;
  std::optional<Split> split;
  std::optional<TimeUnit> time_unit;
  std::optional<VoltageUnit> voltage_unit;
  std::optional<CurrentUnit> current_unit;
  std::optional<ResistanceUnit> resistance_unit;
  std::optional<CapacitanceUnit> capacitance_unit;
  std::optional<PowerUnit> power_unit;
  std::optional<DelayModel> delay_model;

  std::unordered_map<std::string, LutTemplate> lut_templates;
  std::unordered_map<std::string, Cell> cells;

  void read(const std::filesystem::path&, Split);
  void to_time_unit(const TimeUnit&);
  void to_voltage_unit(const VoltageUnit&);
  void to_current_unit(const CurrentUnit&);
  void to_resistance_unit(const ResistanceUnit&);
  void to_capacitance_unit(const CapacitanceUnit&);
  void to_power_unit(const PowerUnit&);

  const LutTemplate* lut_template(const std::string&) const;
  const Cell* cell(const std::string&) const;
  
  LutTemplate* lut_template(const std::string&);
  Cell* cell(const std::string&);

  private:

  LutTemplate _extract_lut_template(token_iterator&, const token_iterator);
  TimingLut   _extract_lut         (token_iterator&, const token_iterator);
  Cell        _extract_cell        (token_iterator&, const token_iterator);
  Cellpin     _extract_cellpin     (token_iterator&, const token_iterator);
  Timing      _extract_timing      (token_iterator&, const token_iterator);
};

// Operator <<
std::ostream& operator << (std::ostream&, const Celllib&);


};  // end of namespace ot. -----------------------------------------------------------------------


#endif











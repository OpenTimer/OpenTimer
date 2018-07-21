#ifndef OT_SPEF_SPEF_HPP_
#define OT_SPEF_SPEF_HPP_

#include <ot/headerdef.hpp>
#include <ot/static/logger.hpp>
#include <ot/utility/utility.hpp>
#include <ot/unit/unit.hpp>

namespace ot::spef {

// Struct: Net
struct Net {
  std::string name;
  float lcap;
  std::vector<std::string> pins;
  std::vector<std::tuple<std::string, float>> caps;
  std::vector<std::tuple<std::string, std::string, float>> ress;

  void scale_capacitance(float);
  void scale_resistance(float);
};

// Operator: <<
std::ostream& operator << (std::ostream&, const Net&);

// ------------------------------------------------------------------------------------------------

// Struct: Spef
struct Spef {

  std::optional<TimeUnit> time_unit;
  std::optional<CapacitanceUnit> capacitance_unit;
  std::optional<ResistanceUnit> resistance_unit;

  std::vector<Net> nets;

  void read(const std::filesystem::path&);
  void to_capacitance_unit(const CapacitanceUnit&);
  void to_resistance_unit(const ResistanceUnit&);
};

// Operator: <<
std::ostream& operator << (std::ostream&, const Spef&);


};  // end of namespace ot. -----------------------------------------------------------------------


#endif

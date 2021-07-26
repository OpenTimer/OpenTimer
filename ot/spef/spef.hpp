#ifndef OT_SPEF_SPEF_HPP_
#define OT_SPEF_SPEF_HPP_

#include <ot/headerdef.hpp>
#include <ot/static/logger.hpp>
#include <ot/utility/utility.hpp>
#include <ot/unit/unit.hpp>

/*namespace ot::spef {

// keywords
inline const std::unordered_set<std::string> keywords = {
  // header
  "*SPEF",
  "*DESIGN",
  "*DATE",
  "*VENDOR",
  "*PROGRAM",
  "*VERSION",
  "*DESIGN_FLOW",
  "*DIVIDER",
  "*DELIMITER",
  "*BUS_DELIMITER",
  "*T_UNIT",
  "*C_UNIT",
  "*R_UNIT",
  "*L_UNIT",
  // body
  "*NAME_MAP",
  "*PORTS",
  "*D_NET",
  "*CONN",
  "*CAP",
  "*RES",
  "*END"
};

// Function: is_keyword
bool is_keyword(const std::string&);

// Function: resolve
void resolve(const std::unordered_map<std::string, std::string>&, std::string&);

// ------------------------------------------------------------------------------------------------

enum class ConnectionType {
  INTERNAL,
  EXTERNAL
};

std::string to_string(ConnectionType);

enum class ConnectionDirection {
  INPUT,
  OUTPUT,
  INOUT
};

std::string to_string(ConnectionDirection);

// Struct: Connection
struct Connection {

  std::string name;
  ConnectionType type;
  ConnectionDirection direction;

  Connection(const std::string&, ConnectionType, ConnectionDirection);

  Connection() = default;
  Connection(Connection&&) = default;

  Connection& operator = (Connection&&) = default;
};

// ------------------------------------------------------------------------------------------------

// Struct: Net
struct Net {

  std::string name;
  float lcap;
  std::vector<Connection> connections;
  std::vector<std::tuple<std::string, float>> caps;
  std::vector<std::tuple<std::string, std::string, float>> ress;

  void scale_capacitance(float);
  void scale_resistance(float);

  Net() = default;
  Net(Net&&) = default;

  Net& operator = (Net&&) = default;
};

// Operator: <<
std::ostream& operator << (std::ostream&, const Net&);

// ------------------------------------------------------------------------------------------------

// Struct: Spef
struct Spef {
  
  std::optional<char> divider;
  std::optional<char> delimiter;
  std::optional<TimeUnit> time_unit;
  std::optional<CapacitanceUnit> capacitance_unit;
  std::optional<ResistanceUnit> resistance_unit;

  std::unordered_map<std::string, std::string> name_map;

  std::vector<Net> nets;

  void read(const std::filesystem::path&);
  void to_capacitance_unit(const CapacitanceUnit&);
  void to_resistance_unit(const ResistanceUnit&);

  Spef() = default;
  Spef(Spef&&) = default;

  Spef& operator = (Spef&&) = default;
};

// Operator: <<
std::ostream& operator << (std::ostream&, const Spef&);

};  // end of namespace ot. -----------------------------------------------------------------------
*/

#endif

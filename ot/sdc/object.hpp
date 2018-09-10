#ifndef OT_SDC_OBJECT_HPP
#define OT_SDC_OBJECT_HPP

#include <ot/sdc/tokenize.hpp>

namespace ot::sdc {

struct CurrentDesign {
};

struct AllClocks{
};

struct GetClocks {
  std::vector<std::string> clocks;
};

struct AllInputs {
};

struct AllOutputs {
};

struct GetPorts {
  std::vector<std::string> ports;
};

struct GetCells {
  std::vector<std::string> cells;
};

struct GetPins {
  std::vector<std::string> pins;
};

struct GetNets {
  std::vector<std::string> nets;
};

struct GetLibs {
  std::vector<std::string> libs;
};

struct GetLibCells {

};

struct GetLibPins {
};

struct AllRegisters{
};

using Object = std::variant<
  CurrentDesign,
  GetClocks,
  AllClocks,
  GetPorts, 
  AllInputs, 
  AllOutputs,
  GetPins,
  GetCells,
  GetNets,
  GetLibs,
  GetLibCells,
  GetLibPins,
  AllRegisters
>;

// ------------------------------------------------------------------------------------------------

Object parse_port(const std::string&);

};  // end of namespace ot::sdc. ------------------------------------------------------------------



#endif

#include <ot/sdc/tokenize.hpp>

namespace ot::sdc {

struct CurrentDesign {
};

using Design = CurrentDesign;

// ------------------------------------------------------------------------------------------------

struct AllClocks{
};

struct GetClocks {
  std::vector<std::string> clocks;
};

using Clock = std::variant<GetClocks, AllClocks>;

// ------------------------------------------------------------------------------------------------

struct AllInputs{
};

struct AllOutputs{
};

struct GetPorts {
  std::vector<std::string> ports;
};

using Port = std::variant<GetPorts, AllInputs, AllOutputs>;

// ------------------------------------------------------------------------------------------------

struct GetCells {
  std::vector<std::string> cells;
};

using Cell = GetCells;

// ------------------------------------------------------------------------------------------------

struct GetPins {
  std::vector<std::string> pins;
};

using Pin = GetPins;

// ------------------------------------------------------------------------------------------------

struct GetNets {
  std::vector<std::string> nets;
};

using Net = GetNets;

// ------------------------------------------------------------------------------------------------

struct GetLibs {
  std::vector<std::string> libs;
};

using Library = GetLibs;

// ------------------------------------------------------------------------------------------------

struct GetLibCells {

};

using LibCell = GetLibCells;

// ------------------------------------------------------------------------------------------------

struct GetLibPins {
};

using LibPin = GetLibPins;

// ------------------------------------------------------------------------------------------------

struct AllRegisters{
};

using Register = AllRegisters;

// ------------------------------------------------------------------------------------------------
  
Port  extract_port (const std::string&);
Port  extract_port (token_iter_t&, token_iter_t);
Clock extract_clock(const std::string&);
Clock extract_clock(token_iter_t&, token_iter_t);

// ------------------------------------------------------------------------------------------------

std::ostream& operator << (std::ostream&, const Port&);
std::ostream& operator << (std::ostream&, const Clock&);


};  // end of namespace ot::sdc. ------------------------------------------------------------------





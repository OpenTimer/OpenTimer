#ifndef OT_LIBERTY_CELL_HPP_
#define OT_LIBERTY_CELL_HPP_

#include <ot/liberty/cellpin.hpp>

namespace ot {

// Class SequentialInfo
struct SequentialInfo {
  std::string clocked_on;
  std::string next_state;
  std::string data_in;
  std::string enable;
};


// Class: Cell
struct Cell {

  std::string name; 
  std::string cell_footprint;

  std::optional<float> leakage_power;
  std::optional<float> area;
  std::optional<bool> is_sequential;
  std::unordered_map<std::string, Cellpin> cellpins;

  // Optional: Sequential information
  std::optional<bool> is_ff;
  std::optional<SequentialInfo> sequential_info;

  void scale_time(float s);
  void scale_capacitance(float s);

  const Cellpin* cellpin(const std::string&) const;
};

std::ostream& operator << (std::ostream&, const Cell&);

// Alias
using CellView = std::array<const Cell*, MAX_SPLIT>;

};  // end of namespace ot. -----------------------------------------------------------------------

#endif



#ifndef OT_LIBERTY_CELL_HPP_
#define OT_LIBERTY_CELL_HPP_

#include <ot/liberty/cellpin.hpp>

namespace ot {

// Class: Cell
struct Cell {

  std::string name; 
  std::string cell_footprint;

  std::optional<float> leakage_power;
  std::optional<float> area;

  std::unordered_map<std::string, Cellpin> cellpins;

  void scale_time(float s);
  void scale_capacitance(float s);

  const Cellpin* cellpin(const std::string&) const;
};

std::ostream& operator << (std::ostream&, const Cell&);

// Alias
using CellView = std::array<const Cell*, MAX_SPLIT>;

};  // end of namespace ot. -----------------------------------------------------------------------

#endif



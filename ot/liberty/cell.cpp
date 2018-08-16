#include <ot/liberty/cell.hpp>

namespace ot {

// Procedure: scale_time
void Cell::scale_time(float s) {
  for(auto& kvp : cellpins) {
    kvp.second.scale_time(s);
  }
}

// Procedure: scale_capacitance
void Cell::scale_capacitance(float s) {
  for(auto& kvp : cellpins) {
    kvp.second.scale_capacitance(s);
  }
}

// Function: cellpin
const Cellpin* Cell::cellpin(const std::string& name) const {
  if(auto itr = cellpins.find(name); itr == cellpins.end()) {
    return nullptr;
  }
  else return &(itr->second);
}

// Operator:: <<
std::ostream& operator << (std::ostream& os, const Cell& c) {

  // Write the cell name.
  os << "cell (\"" << c.name << "\") {\n";

  if(!c.cell_footprint.empty()) {
    os << "  cell_footprint : " << c.cell_footprint << ";\n";
  }

  if(c.leakage_power) {
    os << "  cell_leakage_power : " << *c.leakage_power << ";\n";
  }

  if(c.area) {
    os << "  area : " << *c.area << ";\n";
  }

  // Write the cellpins.
  for(const auto& kvp : c.cellpins) {
    os << kvp.second;
  }
  
  // Write the ending group symbol.
  os << "}\n";

  return os;
}

// ------------------------------------------------------------------------------------------------

};  // end of namespace ot. -----------------------------------------------------------------------

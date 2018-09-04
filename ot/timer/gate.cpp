#include <ot/timer/gate.hpp>

namespace ot {

// Constructor
Gate::Gate(const std::string& name, CellView cell) : 
  _name {name},
  _cell {cell} {
}

// Function: cell_name
const std::string& Gate::cell_name() const {
  return _cell[MIN]->name;
}

/*// Procedure: _repower
void Gate::_repower(CellView cell) {

  assert(cell[MIN] && cell[MAX]);

  // Remap the pin
  for(auto pin : _pins) {
    FOR_EACH_EL(el) {
      assert(pin->cellpin(el));
      if(const auto cpin = cell[el]->cellpin(pin->cellpin(el)->name)) {
        pin->_remap_cellpin(el, *cpin);
      }
      else {
        OT_LOGE(
          "repower ", _name, " with ", cell[el]->name, " failed (cellpin mismatched)"
        );  
      }
    }
  }

  // Remap the arc
  for(auto arc : _arcs) {

    auto ptv = arc->timing_view();
    
    FOR_EACH_EL_IF(el, ptv[el]) {

      if(auto tcp = cell[el]->cellpin(arc->_to.cellpin(el)->name); tcp) {
        if(auto ntm = tcp->isomorphic_timing(*ptv[el]); ntm) {
          arc->_remap_timing(el, *ntm);
        }
        else {
          OT_LOGE(
            "repower ", _name, " with ", cell[el]->name, " failed (timing mismatched)"
          );
        }
      }
      else {
        OT_LOGE(
          "repower ", _name, " with ", cell[el]->name, " failed (cellpin mismatched)"
        );  
      }
    }
  }
  
  // remap to the new cell.
  _cell = cell;
}*/



};  // end of namespace ot. -----------------------------------------------------------------------







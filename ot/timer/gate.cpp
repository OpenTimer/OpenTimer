#include <ot/timer/gate.hpp>
#include <ot/timer/pin.hpp>
#include <ot/timer/arc.hpp>
#include <ot/timer/net.hpp>
#include <ot/timer/test.hpp>

namespace ot {

// Constructor
Gate::Gate(const std::string& name, SplitView<Cell> cell) : 
  _name {name},
  _cell {cell} {
}

// Procedure: _repower
void Gate::_repower(SplitView<Cell> cell) {

  // Remap the pin
  for(auto pin : _pins) {
    FOR_EACH_EL(el) {
      assert(pin->cellpin(el));
      if(const auto cpin = cell.get(el).cellpin(pin->cellpin(el)->name)) {
        pin->_remap_cellpin(el, *cpin);
      }
      else {
        OT_LOGE(
          "repower ", _name, " with ", cell[el].name, " failed (cellpin mismatched)"
        );  
      }
    }
  }

  // Remap the arc
  for(auto arc : _arcs) {

    auto oldtm = arc->_timing();
    
    FOR_EACH_EL(el) {
      if(auto tcp = cell.get(el).cellpin(arc->_to.cellpin(el)->name); tcp) {
        if(auto newtm = tcp->isomorphic_timing(oldtm.get(el)); newtm) {
          arc->_remap_timing(el, *newtm);
        }
        else {
          OT_LOGE(
            "repower ", _name, " with ", cell[el].name, " failed (timing mismatched)"
          );
        }
      }
      else {
        OT_LOGE(
          "repower ", _name, " with ", cell[el].name, " failed (cellpin mismatched)"
        );  
      }
    }
  }
  
  // remap to the new cell.
  _cell = cell;
}



};  // end of namespace ot. -----------------------------------------------------------------------







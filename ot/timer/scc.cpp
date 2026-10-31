#include <algorithm>
#include <tuple>
#include <vector>
#include <ot/timer/pin.hpp>
#include <ot/timer/arc.hpp>
#include <ot/timer/scc.hpp>

namespace ot {

// Constructor
SCC::SCC(std::vector<Pin*>&& pins) : _pins {std::move(pins)} {

  for(auto pin : _pins) {
    pin->_scc = this;
  }
}

// Function: _is_entry
bool SCC::_is_entry(const Pin& to) const {

  assert(to._scc == this);

  for(auto arc : to._fanin) {
    if(auto& from = arc->_from; from._scc != this) {
      return true;
    }
  }

  return false;
}

// Function: _is_exit
bool SCC::_is_exit(const Pin& from) const {

  assert(from._scc == this);

  for(auto arc : from._fanout) {
    if(auto& to = arc->_to; to._scc != this) {
      return true;
    }
  }

  return false;

}

// Function: _dump
std::string SCC::_dump() const {
  
  std::ostringstream os;

  os << "digraph SCC {\n";
  for(auto from : _pins) {
    os << "  \"" << from->name() << "\";\n";

    for(auto& arc : from->_fanout) {
      if(from->_scc == arc->_to._scc) {
        os << "  \"" 
           << from->name() << "\" -> \"" << arc->_to.name()
           << "\";\n";  
      }
    }
  }

  os << "}\n";

  return os.str();
}

// Procedure: _clear
void SCC::_clear() {

  // unmark the scc pins and arcs
  for(auto pin : _pins) {
    
    for(auto arc : pin->_fanin) {
      if(arc->_from._scc == this) {
        arc->_remove_state(Arc::LOOP_BREAKER);
      }
    }

    for(auto arc : pin->_fanout) {
      if(arc->_to._scc == this) {
        arc->_remove_state(Arc::LOOP_BREAKER);
      }
    }

    pin->_scc = nullptr;
  }

  _pins.clear();
}

// Procedure: _unloop
void SCC::_unloop(Pin& from) {
 
  assert(!from._has_state(Pin::UNLOOP_CAND) && !from._has_state(Pin::IN_UNLOOP_STACK));

  from._insert_state(Pin::UNLOOP_CAND | Pin::IN_UNLOOP_STACK);

  // Visit fanouts in pin-index order: the loop breakers must not depend on
  // arc insertion order (repowered gates re-insert their arcs), so a full and
  // an incremental update break the same loop at the same arcs.
  std::vector<Arc*> fanout(from._fanout.begin(), from._fanout.end());
  std::sort(fanout.begin(), fanout.end(), [] (const Arc* a, const Arc* b) {
    const auto ka = std::pair{a->to().idx(), a->idx()}, kb = std::pair{b->to().idx(), b->idx()};
    return ka < kb;
  });

  for(auto arc : fanout) {

    if(auto& to = arc->_to; to._scc != this) {
      continue;
    }
    else if(!to._has_state(Pin::UNLOOP_CAND)) {
      _unloop(to);
    }
    else if(to._has_state(Pin::IN_UNLOOP_STACK)) {
      //OT_LOGD("mark arc ", arc->name(), " as loop breaker");
      arc->_insert_state(Arc::LOOP_BREAKER);
    }
  }
  
  from._remove_state(Pin::IN_UNLOOP_STACK);
}

// Procedure: _unloop
// Find the starting pin to break the loop.
void SCC::_unloop() {

  assert(!_pins.empty());

  // Deterministic entry, independent of the discovery order of the SCC: the
  // lowest-index entry input pin, else the lowest-index entry, else the
  // lowest-index pin.
  Pin* entry {nullptr};
  const auto better = [] (const Pin* a, const Pin* b) {
    if(!b) return true;
    if(a->is_input() != b->is_input()) return a->is_input();
    return a->_idx < b->_idx;
  };

  for(auto pin : _pins) {
    if(_is_entry(*pin) && better(pin, entry)) {
      entry = pin;
    }
  }

  if(entry == nullptr) {
    entry = *std::min_element(_pins.begin(), _pins.end(), [] (const Pin* a, const Pin* b) { return a->_idx < b->_idx; });
  }
  _unloop(*entry);
}


};  // end of namespace ot. -----------------------------------------------------------------------




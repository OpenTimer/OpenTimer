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
    os << "  \"" << from->_name << "\";\n";

    for(auto& arc : from->_fanout) {
      if(from->_scc == arc->_to._scc) {
        os << "  \"" 
           << from->_name << "\" -> \"" << arc->_to._name 
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

  for(auto arc : from._fanout) {

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

  Pin* entry {nullptr};
  
  for(auto pin : _pins) {
    if(!_is_entry(*pin)) continue;
    if(!entry || pin->is_input()) { 
      entry = pin;
    }
  }

  if(entry == nullptr) {
    entry = _pins[0];
  }
  _unloop(*entry);
}


};  // end of namespace ot. -----------------------------------------------------------------------




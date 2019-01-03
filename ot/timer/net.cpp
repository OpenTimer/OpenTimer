#include <ot/timer/net.hpp>

namespace ot {

// Constructor
RctNode::RctNode(const std::string& name) : _name {name} {
}

// Procedure: _scale_capacitance
void RctNode::_scale_capacitance(float s) {
  FOR_EACH_EL_RF(el, rf) {
    _ncap[el][rf] *= s; 
  }
}

// Function: load
float RctNode::load(Split el, Tran rf) const {
  return _load[el][rf];
}

// Function: cap
float RctNode::cap(Split el, Tran rf) const {
  return _pin ? _pin->cap(el, rf) + _ncap[el][rf] : _ncap[el][rf];
}
  
// Function: slew
float RctNode::slew(Split m, Tran t, float si) const {  
  return si < 0.0f ? -std::sqrt(si*si + _impulse[m][t]) : std::sqrt(si*si + _impulse[m][t]);
}

// Function: delay
float RctNode::delay(Split m, Tran t) const {
  return _delay[m][t];
}

// ------------------------------------------------------------------------------------------------

// Constructor
RctEdge::RctEdge(RctNode& from, RctNode& to, float res) : 
  _from {from},
  _to   {to},
  _res  {res} {
}

// Procedure: _scale_resistance
void RctEdge::_scale_resistance(float s) {
  _res *= s;
}

// ------------------------------------------------------------------------------------------------

// Function: _node
RctNode* Rct::_node(const std::string& name) {
  if(auto itr = _nodes.find(name); itr != _nodes.end()) {
    return &(itr->second);
  }
  else return nullptr;
}

// Function: node
const RctNode* Rct::node(const std::string& name) const {
  if(const auto itr = _nodes.find(name); itr != _nodes.end()) {
    return &(itr->second);
  }
  else return nullptr;
}

// Procedure: insert_node
void Rct::insert_node(const std::string& name, float cap) {

  auto& node = _nodes[name];

  node._name = name;

  FOR_EACH_EL_RF(el, rf) {
    node._ncap[el][rf] = cap;
  }
}

// Procedure: insert_edge
void Rct::insert_edge(const std::string& from, const std::string& to, float res) {
  
  auto& tail = _nodes[from];
  auto& head = _nodes[to];
  auto& edge = _edges.emplace_back(tail, head, res);

  tail._fanout.push_back(&edge);
  head._fanin.push_back(&edge);
}
 
// Function: insert_segment
void Rct::insert_segment(const std::string& name1, const std::string& name2, float res) {
  insert_edge(name1, name2, res);
  insert_edge(name2, name1, res);
}

// Procedure: update_rc_timing
void Rct::update_rc_timing() {

  if(!_root) {
    OT_THROW(Error::RCT, "rctree root not found");
  }

  for(auto& kvp : _nodes) {
    FOR_EACH_EL_RF(el, rf) {
      kvp.second._ures[el][rf]    = 0.0f;
      kvp.second._beta[el][rf]    = 0.0f;
      kvp.second._load[el][rf]    = 0.0f;
      kvp.second._delay[el][rf]   = 0.0f;
      kvp.second._ldelay[el][rf]  = 0.0f;
      kvp.second._impulse[el][rf] = 0.0f;
    }
  }
  
  _update_load(nullptr, _root);
  _update_delay(nullptr, _root);   
  _update_ldelay(nullptr, _root);  
  _update_response(nullptr, _root);

}

// Procedure: _update_load
// Compute the load capacitance of each rctree node along the downstream traversal of the rctree.
void Rct::_update_load(RctNode* parent, RctNode* from) {
  // Add downstream capacitances.
  for(auto e : from->_fanout) {
    if(auto& to = e->_to; &to != parent) {
      _update_load(from, &to);
      FOR_EACH_EL_RF(el, rf) {
        from->_load[el][rf] += to._load[el][rf];
      }
    }
  }
  FOR_EACH_EL_RF(el, rf) {
    from->_load[el][rf] += from->cap(el, rf);
  }
}

// Procedure: _update_delay
// Compute the delay of each rctree node using the Elmore delay model.
void Rct::_update_delay(RctNode* parent, RctNode* from) {
  
  for(auto e : from->_fanout) {
    if(auto& to = e->_to; &to != parent) {
      FOR_EACH_EL_RF(el, rf) {
        // Update the delay.
        to._delay[el][rf] = from->_delay[el][rf] + e->_res * to._load[el][rf];
        // Update the upstream resistance.
        to._ures[el][rf] = from->_ures[el][rf] + e->_res;
      }
      _update_delay(from, &to);
    }
  }
}

// Procedure: _update_ldelay
// Compute the load delay of each rctree node along the downstream traversal of the rctree.
void Rct::_update_ldelay(RctNode* parent, RctNode* from) {

  for(auto e : from->_fanout) {
    if(auto& to = e->_to; &to != parent) {
      _update_ldelay(from, &to);
      FOR_EACH_EL_RF(el, rf) {
        from->_ldelay[el][rf] += to._ldelay[el][rf];
      }
    }
  }

  FOR_EACH_EL_RF(el, rf) {
    from->_ldelay[el][rf] += from->cap(el, rf) * from->_delay[el][rf];
  }
}

// Procedure: _update_response
// Compute the impulse and second moment of the input response for each rctree node. 
void Rct::_update_response(RctNode* parent, RctNode* from) {

  for(auto e : from->_fanout) {
    if(auto& to = e->_to; &to != parent) {
      FOR_EACH_EL_RF(el, rf) {
        to._beta[el][rf] = from->_beta[el][rf] + e->_res * to._ldelay[el][rf];
      }
      _update_response(from, &to);
    }
  }

  FOR_EACH_EL_RF(el, rf) {
    from->_impulse[el][rf] = 2.0f * from->_beta[el][rf] - std::pow(from->_delay[el][rf], 2);
  }
}

// Procedure: _scale_capacitance
void Rct::_scale_capacitance(float s) {
  for(auto& kvp : _nodes) {
    kvp.second._scale_capacitance(s);
  }
}

// Procedure: _scale_resistance
void Rct::_scale_resistance(float s) {
  for(auto& edge : _edges) {
    edge._scale_resistance(s);
  }
}

// Function: slew
float Rct::slew(const std::string& name, Split m, Tran t, float si) const {
  auto itr = _nodes.find(name);
  if(itr == _nodes.end()) {
    OT_THROW(Error::RCT, "failed to get slew (rct-node ", name, " not found)");
  }
  return itr->second.slew(m, t, si);
}

// Function: delay
float Rct::delay(const std::string& name, Split m, Tran t) const {
  auto itr = _nodes.find(name);
  if(itr == _nodes.end()) {
    OT_THROW(Error::RCT, "failed to get delay (rct-node ", name, " not found)");
  }
  return itr->second.delay(m, t);
}

// Function: total_ncap
float Rct::total_ncap() const {
  return std::accumulate(_nodes.begin(), _nodes.end(), 0.0f,
    [] (float v, const auto& pair) {
      return v + pair.second._ncap[MIN][RISE];
    }
  );
}

// ------------------------------------------------------------------------------------------------

// Constructor
Net::Net(const std::string& name) : 
  _name {name} {
}

// Procedure: _attach
void Net::_attach(spef::Net&& spef_net) {
  assert(spef_net.name == _name && _root);
  _spef_net = std::move(spef_net);
  _rc_timing_updated = false;
}

// Procedure: _make_rct
void Net::_make_rct() {
  
  if(!_spef_net) return;

  // Step 1: create a new rctree object
  auto& rct = _rct.emplace<Rct>();

  // Step 2: insert the node and capacitance (*CAP section).
  for(const auto& [node1, node2, cap] : _spef_net->caps) {
    
    // ground capacitance
    if(node2.empty()) {
      rct.insert_node(node1, cap);
    }
    // TODO: coupling capacitance
  }

  // Step 3: insert the segment (*RES section).
  for(const auto& [node1, node2, res] : _spef_net->ress) {
    rct.insert_segment(node1, node2, res);
  }
  
  _spef_net.reset();
  
  _rc_timing_updated = false;
}

// Procedure: _scale_capacitance
void Net::_scale_capacitance(float s) {

  std::visit(Functors{
    // Leave this to the next update timing
    [&] (EmptyRct& rct) {
    },
    [&] (Rct& rct) {
      rct._scale_capacitance(s);
    }
  }, _rct);
  
  _rc_timing_updated = false;
}

// Procedure: _scale_resistance
void Net::_scale_resistance(float s) {

  std::visit(Functors{
    // Leave this to the next update timing
    [&] (EmptyRct& rct) {
    },
    [&] (Rct& rct) {
      rct._scale_resistance(s);
    }
  }, _rct);
  
  _rc_timing_updated = false;
}

// Procedure: _update_rc_timing
void Net::_update_rc_timing() {

  if(_rc_timing_updated) {
    return;
  }

  // Apply the spefnet if any
  _make_rct();
  
  // update the corresponding handle
  std::visit(Functors{
    [&] (EmptyRct& rct) {
      FOR_EACH_EL_RF(el, rf) {
        rct.load[el][rf] = std::accumulate(_pins.begin(), _pins.end(), 0.0f, 
          [this, el=el, rf=rf] (float v, Pin* pin) {
            return pin == _root ? v : v + pin->cap(el, rf);
          }
        );
      }
    },
    [&] (Rct& rct) {
      for(auto pin : _pins) {
        if(auto node = rct._node(pin->name()); node == nullptr) {
          OT_LOGE("pin ", pin->name(), " not found in rctree ", _name);
        }
        else {
          if(pin == _root) {
            rct._root = node;
          }
          else {
            node->_pin = pin;
          }
        }
      }
      rct.update_rc_timing();
    }
  }, _rct);

  _rc_timing_updated = true;
}

// Procedure: _remove_pin
// Remove a pin pointer from the net.
void Net::_remove_pin(Pin& pin) {

  assert(pin._net == this);

  // Reset the root pin
  if(_root == &pin) {
    _root = nullptr;
  }

  // Remove the pin from the pins
  _pins.erase(*(pin._net_satellite));
  pin._net_satellite.reset();
  pin._net = nullptr;
  
  // Enable the timing update.
  _rc_timing_updated = false;
}

// Procedure: _insert_pin
// Insert a pin pointer into the net.
void Net::_insert_pin(Pin& pin) {
  
  if(pin._net == this) {
    return;
  }

  assert(pin._net == nullptr && !pin._net_satellite);
  
  pin._net_satellite = _pins.insert(_pins.end(), &pin);
  pin._net = this;

  // NEW
  if(pin.is_rct_root()) {
    _root = &pin;
  }
  
  // Enable the timing update
  _rc_timing_updated = false;  
}

// Function: _load
// The total capacitive load is defined as the sum of the input capacitance 
// of all the other devices sharing the trace.
// Note that the capacitance of the device driving the trace is not included.
float Net::_load(Split m, Tran t) const {

  // TODO: outdated?
  assert(_rc_timing_updated);

  return std::visit(Functors{
    [&] (const EmptyRct& rct) {
      return rct.load[m][t];
    },
    [&] (const Rct& rct) {
      return rct._root->_load[m][t];
    }
  }, _rct);
}

// Function: _slew
// Query the slew at the give pin through this net
std::optional<float> Net::_slew(Split m, Tran t, float si, Pin& to) const {

  assert(_rc_timing_updated && to._net == this);

  return std::visit(Functors{
    [&] (const EmptyRct&) -> std::optional<float> {
      return si;
    },
    [&] (const Rct& rct) -> std::optional<float> {
      if(auto node = rct.node(to._name); node) {
        return node->slew(m, t, si);
      }
      else return std::nullopt;
    }
  }, _rct);
}

// Function: _delay
// Query the slew at the given pin through this net.
std::optional<float> Net::_delay(Split m, Tran t, Pin& to) const {
  
  assert(_rc_timing_updated && to._net == this);

  return std::visit(Functors{
    [&] (const EmptyRct&) -> std::optional<float> {
      return 0.0f;
    },
    [&] (const Rct& rct) -> std::optional<float> {
      if(auto node = rct.node(to._name); node) {
        return node->delay(m, t);
      }
      else return std::nullopt;
    }
  }, _rct);
}


};  // end of namespace ot. -----------------------------------------------------------------------






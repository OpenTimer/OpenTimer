#include <ot/timer/net.hpp>
#include <unordered_set>

namespace ot {

// Constructor
// Names belong to Rct::_nodes. Retain the named constructor for source
// compatibility; node timing never needs a second copy of the map key.
RctNode::RctNode(const std::string&) {
}

// Procedure: _scale_capacitance
void RctNode::_scale_capacitance(float s) { _ncap *= s; }

// Function: load
float RctNode::load(Split el, Tran rf) const {
  return _load[el][rf];
}

// Function: cap
float RctNode::cap(Split el, Tran rf) const {
  return _pin ? _pin->cap(el, rf) + _ncap : _ncap;
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
  return const_cast<RctNode *>(std::as_const(*this).node(name));
}

const RctNode* Rct::node(const std::string& name) const {
  if (const auto *nodes = std::get_if<NamedNodes>(&_nodes)) {
    const auto it = nodes->find(name);
    return it == nodes->end() ? nullptr : &it->second;
  }
  // Named queries on dense trees are diagnostic only. Propagation uses the
  // pin's numeric RC index, so no fanout scan occurs on the timing path.
  const auto &bound = std::get<BoundNodes>(_nodes);
  if (bound.driver->name() == name)
    return _root;
  for (const auto &node : bound)
    if (node._pin && node._pin->name() == name)
      return &node;
  return nullptr;
}

const RctNode *Rct::indexed_node(size_t index) const {
  const auto *nodes = std::get_if<BoundNodes>(&_nodes);
  return nodes && index < nodes->size() ? &(*nodes)[index] : nullptr;
}

RctNode &Rct::_insert_node(const std::string &name) {
  auto *nodes = std::get_if<NamedNodes>(&_nodes);
  if (!nodes)
    OT_THROW(Error::RCT, "cannot insert named nodes into bound RC");
  if (nodes->size() >= std::numeric_limits<uint32_t>::max() &&
      !nodes->contains(name))
    OT_THROW(Error::RCT, "too many RC nodes");
  auto [it, inserted] = nodes->try_emplace(name);
  if (inserted)
    it->second._index = static_cast<uint32_t>(nodes->size() - 1);
  return it->second;
}

std::vector<std::pair<const RctNode *, std::string>>
Rct::_node_names(const std::string &net) const {
  std::vector<std::pair<const RctNode *, std::string>> names;
  names.reserve(num_nodes());
  if (const auto *nodes = std::get_if<NamedNodes>(&_nodes)) {
    for (const auto &[name, node] : *nodes)
      names.emplace_back(&node, name);
  } else {
    const auto &bound = std::get<BoundNodes>(_nodes);
    std::unordered_set<std::string> used{bound.driver->name()};
    for (const auto &node : std::get<BoundNodes>(_nodes))
      if (node._pin)
        used.insert(node._pin->name());
    for (const auto &node : std::get<BoundNodes>(_nodes)) {
      auto name = &node == _root ? bound.driver->name()
                  : node._pin ? node._pin->name()
                              : net + ":__ot_rc_" + std::to_string(node._index);
      if (!node._pin && &node != _root) {
        while (!used.insert(name).second)
          name += "_";
      }
      names.emplace_back(&node, std::move(name));
    }
  }
  return names;
}

// Procedure: insert_node
void Rct::insert_node(const std::string& name, float cap) {

  _insert_node(name)._ncap = cap;
}

// Procedure: insert_edge
void Rct::insert_edge(const std::string& from, const std::string& to, float res) {

  auto &tail = _insert_node(from);
  auto &head = _insert_node(to);
  _insert_edge(tail, head, res);
}

void Rct::_insert_edge(RctNode &tail, RctNode &head, float res) {

  if (_edges.size() >= RctNode::no_edge) {
    OT_THROW(Error::RCT, "too many RC edges");
  }
  const auto index = static_cast<uint32_t>(_edges.size());
  _edges.emplace_back(tail, head, res);
  if (tail._last_fanout == RctNode::no_edge) {
    tail._fanout = index;
  } else {
    _edges[tail._last_fanout]._next = index;
  }
  tail._last_fanout = index;
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

  std::vector<Moments> moments(num_nodes());
  _each_node([&](auto &node) {
    FOR_EACH_EL_RF(el, rf) {
      node._load[el][rf] = 0.0f;
      node._delay[el][rf] = 0.0f;
      node._impulse[el][rf] = 0.0f;
    }
  });

  _update_load(nullptr, _root);
  _update_delay(nullptr, _root);
  _update_ldelay(nullptr, _root, moments);
  _update_response(nullptr, _root, moments);
  FOR_EACH_EL_RF(el, rf) {
    const double total = _root->_load[el][rf];
    double second = 0, third = 0;
    _each_node([&](const auto &node) {
      second += double(node.cap(el, rf)) * node._delay[el][rf];
      third += double(node.cap(el, rf)) * moments[node._index].beta[el][rf];
    });
    auto& pi = _pi[el][rf];
    pi = {total, 0, 0};
    if (second > 0 && third > 0 && std::isfinite(third)) {
      const double far = second * second / third;
      const double resistance = third * third / (second * second * second);
      if (far > 0 && far <= total * 1.0001 && std::isfinite(resistance)) {
        pi.far_cap = std::min(total, far);
        pi.near_cap = total - pi.far_cap;
        pi.resistance = resistance;
      }
    }
  }

}

// Procedure: _update_load
// Compute the load capacitance of each rctree node along the downstream traversal of the rctree.
void Rct::_update_load(RctNode* parent, RctNode* from) {
  // Add downstream capacitances.
  for (auto index = from->_fanout; index != RctNode::no_edge;
       index = _edges[index]._next) {
    const auto& edge = _edges[index];
    if(auto& to = edge._to; &to != parent) {
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

  for (auto index = from->_fanout; index != RctNode::no_edge;
       index = _edges[index]._next) {
    const auto& edge = _edges[index];
    if(auto& to = edge._to; &to != parent) {
      FOR_EACH_EL_RF(el, rf) {
        // Update the delay.
        to._delay[el][rf] = from->_delay[el][rf] + edge._res * to._load[el][rf];
      }
      _update_delay(from, &to);
    }
  }
}

// Procedure: _update_ldelay
// Compute the load delay of each rctree node along the downstream traversal of the rctree.
void Rct::_update_ldelay(RctNode *parent, RctNode *from,
                         std::vector<Moments> &moments) {

  for (auto index = from->_fanout; index != RctNode::no_edge;
       index = _edges[index]._next) {
    const auto& edge = _edges[index];
    if(auto& to = edge._to; &to != parent) {
      _update_ldelay(from, &to, moments);
      FOR_EACH_EL_RF(el, rf) {
        moments[from->_index].ldelay[el][rf] +=
            moments[to._index].ldelay[el][rf];
      }
    }
  }

  FOR_EACH_EL_RF(el, rf) {
    moments[from->_index].ldelay[el][rf] +=
        from->cap(el, rf) * from->_delay[el][rf];
  }
}

// Procedure: _update_response
// Compute the impulse and second moment of the input response for each rctree
// node.
void Rct::_update_response(RctNode *parent, RctNode *from,
                           std::vector<Moments> &moments) {

  for (auto index = from->_fanout; index != RctNode::no_edge;
       index = _edges[index]._next) {
    const auto& edge = _edges[index];
    if(auto& to = edge._to; &to != parent) {
      FOR_EACH_EL_RF(el, rf) {
        moments[to._index].beta[el][rf] =
            moments[from->_index].beta[el][rf] +
            edge._res * moments[to._index].ldelay[el][rf];
      }
      _update_response(from, &to, moments);
    }
  }

  FOR_EACH_EL_RF(el, rf) {
    from->_impulse[el][rf] = 2.0f * moments[from->_index].beta[el][rf] -
                             std::pow(from->_delay[el][rf], 2);
  }
}

// Procedure: _scale_capacitance
void Rct::_scale_capacitance(float s) {
  _each_node([&](auto &node) { node._scale_capacitance(s); });
}

// Procedure: _scale_resistance
void Rct::_scale_resistance(float s) {
  for(auto& edge : _edges) {
    edge._scale_resistance(s);
  }
}

float Rct::slew(const std::string& name, Split m, Tran t, float si) const {
  const auto *n = node(name);
  if (!n)
    OT_THROW(Error::RCT, "failed to get slew (rct-node ", name, " not found)");
  return n->slew(m, t, si);
}

float Rct::delay(const std::string& name, Split m, Tran t) const {
  const auto *n = node(name);
  if (!n)
    OT_THROW(Error::RCT, "failed to get delay (rct-node ", name, " not found)");
  return n->delay(m, t);
}

float Rct::total_ncap() const {
  float total = 0;
  _each_node([&](const auto &node) { total += node._ncap; });
  return total;
}

// ------------------------------------------------------------------------------------------------

// Constructor
Net::Net(const std::string& name) : 
  _name {name} {
}

// Procedure: _attach
void Net::_attach(spef::Net&& spef_net) {
  assert(spef_net.name == _name && _root);
  _spef_net = std::make_shared<const spef::Net>(std::move(spef_net));
  _rc_timing_updated = false;
}

void Net::_attach(IndexedRc &&network) {
  assert(network.name == _name && _root);
  _spef_net.reset();
  auto &rct = _rct.emplace<Rct>();
  rct._edges.reserve(2 * network.edges.size());
  std::vector<RctNode *> nodes;
  nodes.reserve(network.nodes.size());
  // Keep the same node insertion and bidirectional edge order as SPEF. This
  // preserves traversal and floating-point reduction order exactly.
  for (const auto &node : network.nodes) {
    auto &stored = rct._insert_node(node.name);
    stored._ncap = node.capacitance;
    nodes.push_back(&stored);
  }
  for (const auto &edge : network.edges) {
    rct._insert_edge(*nodes[edge.from], *nodes[edge.to], edge.resistance);
    rct._insert_edge(*nodes[edge.to], *nodes[edge.from], edge.resistance);
  }
  _rc_timing_updated = false;
}

void Net::_attach(const BoundRc &network, const std::vector<Pin *> &pins) {
  _spef_net.reset();
  auto &rct = _rct.emplace<Rct>();
  auto &nodes =
      rct._nodes.emplace<Rct::BoundNodes>(network.nodes.size(), _root);
  for (size_t i = 0; i < nodes.size(); ++i) {
    auto &node = nodes[i];
    node._index = static_cast<uint32_t>(i);
    node._ncap = network.nodes[i].capacitance;
    if (network.nodes[i].pin != BoundRc::no_pin) {
      node._pin = pins[network.nodes[i].pin];
      node._pin->_rc_index = node._index;
      if (node._pin == _root) {
        rct._root = &node;
        node._pin = nullptr;
      }
    }
  }
  rct._edges.reserve(2 * network.edges.size());
  for (const auto &edge : network.edges) {
    rct._insert_edge(nodes[edge.from], nodes[edge.to], edge.resistance);
    rct._insert_edge(nodes[edge.to], nodes[edge.from], edge.resistance);
  }
  _rc_timing_updated = false;
}

// Procedure: _make_rct
void Net::_make_rct() {
  
  if(!_spef_net) return;

  // Step 1: create a new rctree object
  auto& rct = _rct.emplace<Rct>();
  rct._edges.reserve(2 * _spef_net->ress.size());

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
  std::visit(Functors{[&](EmptyRct &rct) {
                        FOR_EACH_EL_RF(el, rf) {
                          rct.load[el][rf] = std::accumulate(
                              _pins.begin(), _pins.end(), 0.0f,
                              [this, el = el, rf = rf](float v, Pin *pin) {
                                return pin == _root ? v : v + pin->cap(el, rf);
                              });
                        }
                      },
                      [&](Rct &rct) {
                        if (std::holds_alternative<Rct::NamedNodes>(rct._nodes))
                          for (auto pin : _pins) {
                            if (auto node = rct._node(pin->name());
                                node == nullptr) {
                              OT_LOGE("pin ", pin->name(),
                                      " not found in rctree ", _name);
                            } else {
                              if (pin == _root) {
                                rct._root = node;
                              } else {
                                node->_pin = pin;
                              }
                            }
                          }
                        rct.update_rc_timing();
                      }},
             _rct);

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

  _rct = EmptyRct{};
  _spef_net.reset();
  _driver = {};
  _effective_cap = {};
  _elmore_fallback = {};

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
  
  _rct = EmptyRct{};
  _spef_net.reset();
  _driver = {};
  _effective_cap = {};
  _elmore_fallback = {};

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

std::optional<RcGateResult> Net::_gate_response(const Timing& timing, Split split,
                                                Tran input, Tran output, float slew, const SlewThresholds* thresholds) const {
  if (_rc_model == RcModel::elmore)
    return {};
  const auto* tree = std::get_if<Rct>(&_rct);
  if (!tree || !_root) return {};
  const auto table = [&](double cap) -> std::optional<std::pair<double,double>> {
    const auto delay = timing.delay(input, output, slew, cap);
    const auto transition = timing.slew(input, output, slew, cap);
    if (!delay || !transition || !std::isfinite(*delay) || !std::isfinite(*transition)) return {};
    return std::pair<double,double>{*delay,*transition};
  };
  return rc_effective_gate(tree->_pi[split][output], thresholds ? *thresholds : _root->thresholds(split), output, table);
}

// Threshold-aware one-pole response of this net, in the sink library's units.
std::optional<RcResponse> Net::_response(Split m, Tran t, float si, Pin& to) const {
  const auto tau = _delay(m, t, to);
  if (!tau || !_root) return std::nullopt;
  if (_rc_model == RcModel::elmore) {
    // A closed-form ranking model: characterized gate delay at total load,
    // Elmore wire delay and quadrature slew. No effective-C or crossing solve.
    const auto &from = _root->thresholds(m);
    const auto &sink = to.thresholds(m);
    const double duration = si * from.derate / from.span(t);
    const double shift = duration * (sink.input[t] - from.output[t]);
    const double lo = t == RISE ? sink.lower[t] : 1 - sink.upper[t];
    const double hi = t == RISE ? sink.upper[t] : 1 - sink.lower[t];
    const double wire_slew = *tau * std::log((1 - lo) / (1 - hi));
    return RcResponse{
        float(*tau + (t == RISE ? shift : -shift)),
        float(std::hypot(duration * sink.span(t), wire_slew) / sink.derate)};
  }
  if (_elmore_fallback[m][t]) {
    // A load-insensitive or otherwise unfit Liberty driver does not justify
    // inventing an output waveform. Retain its characterized transition and
    // use the Elmore upper-bound approximation for this annotated net.
    const auto& from = _root->thresholds(m);
    const auto& sink = to.thresholds(m);
    const double duration = si * from.derate / from.span(t);
    const double shift = duration * (sink.input[t] - from.output[t]);
    return RcResponse{float(*tau + (t == RISE ? shift : -shift)),
                      float(duration * sink.span(t) / sink.derate)};
  }
  if (const auto& driver = _driver[m][t]) {
    const auto& sink = to.thresholds(m);
    const auto progress = [t](double v) { return t == RISE ? v : 1-v; };
    const double low = progress(sink.lower[t]), high = progress(sink.upper[t]);
    const auto crossings = driver->crossings<3>({progress(sink.input[t]), high, low}, *tau);
    const double delay = crossings[0] - driver->logic_time;
    const double slew = std::abs(crossings[1] - crossings[2]) / sink.derate;
    return RcResponse{float(delay), float(slew)};
  }
  return rc_wire_response(*tau, si, _root->thresholds(m), to.thresholds(m), t);
}

// Function: _slew
// Query the slew at the give pin through this net
std::optional<float> Net::_slew(Split m, Tran t, float si, Pin& to) const {

  assert(_rc_timing_updated && to._net == this);

  return std::visit(
      Functors{[&](const EmptyRct &) -> std::optional<float> { return si; },
               [&](const Rct &rct) -> std::optional<float> {
                 const auto *node =
                     std::holds_alternative<Rct::BoundNodes>(rct._nodes)
                         ? rct.indexed_node(to._rc_index)
                         : rct.node(to.name());
                 if (node &&
                     (std::holds_alternative<Rct::NamedNodes>(rct._nodes) ||
                      node->_pin == &to ||
                      (&to == _root && node == rct._root))) {
                   return node->slew(m, t, si);
                 } else
                   return std::nullopt;
               }},
      _rct);
}

// Function: _delay
// Query the slew at the given pin through this net.
std::optional<float> Net::_delay(Split m, Tran t, Pin& to) const {
  
  assert(_rc_timing_updated && to._net == this);

  return std::visit(
      Functors{[&](const EmptyRct &) -> std::optional<float> { return 0.0f; },
               [&](const Rct &rct) -> std::optional<float> {
                 const auto *node =
                     std::holds_alternative<Rct::BoundNodes>(rct._nodes)
                         ? rct.indexed_node(to._rc_index)
                         : rct.node(to.name());
                 if (node &&
                     (std::holds_alternative<Rct::NamedNodes>(rct._nodes) ||
                      node->_pin == &to ||
                      (&to == _root && node == rct._root))) {
                   return node->delay(m, t);
                 } else
                   return std::nullopt;
               }},
      _rct);
}


};  // end of namespace ot. -----------------------------------------------------------------------






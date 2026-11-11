#include <ot/timer/timer.hpp>

namespace ot {

// Function: read_spef
Timer& Timer::read_spef(std::filesystem::path path) {

  // Create a spefnet shared pointer
  auto spef = std::make_shared<spef::Spef>(); 
  
  std::scoped_lock lock(_mutex);

  // Reader task
  auto parser = _taskflow.emplace([path=std::move(path), spef] () {
    OT_LOGI("loading spef ", path);
    if(spef->read(path); spef->error) {
      OT_LOGE("Parser-SPEF error:\n", *spef->error);
    }
    spef->expand_name();
  });
  
  // Spef update task (this has to be after parser)
  auto reader = _taskflow.emplace([this, spef] () {
    if(!(spef->error)) {
      _rebase_unit(*spef);
      _read_spef(*spef);
      OT_LOGI("added ", spef->nets.size(), " spef nets");
    }
  });
  
  // Build the task dependency.
  parser.precede(reader);
  
  _add_to_lineage(reader);

  return *this;
}

void Timer::update_parasitics(spef::Spef value) {
  std::scoped_lock lock(_mutex);
  _update_netlist();
  _rebase_unit(value);
  _read_spef(value, true);
  _remove_state(EPTS_UPDATED | AREA_UPDATED | POWER_UPDATED);
}

void Timer::update_parasitics(std::vector<IndexedRc> networks,
                              farad_t capacitance_unit, ohm_t resistance_unit) {
  if (!std::isfinite(capacitance_unit.value()) ||
      capacitance_unit.value() <= 0 ||
      !std::isfinite(resistance_unit.value()) || resistance_unit.value() <= 0) {
    OT_THROW(Error::RCT, "indexed RC units must be finite and positive");
  }
  // Reject malformed indices before changing any installed RC or units.
  for (const auto &net : networks) {
    for (const auto &edge : net.edges) {
      if (edge.from >= net.nodes.size() || edge.to >= net.nodes.size()) {
        OT_THROW(Error::RCT,
                 "indexed RC endpoint outside node array: ", net.name);
      }
    }
  }
  std::scoped_lock lock(_mutex);
  _update_netlist();
  if (!_capacitance_unit)
    _capacitance_unit = capacitance_unit;
  if (!_resistance_unit)
    _resistance_unit = resistance_unit;
  const float cap_scale = (capacitance_unit / *_capacitance_unit).value();
  const float res_scale = (resistance_unit / *_resistance_unit).value();
  for (auto &network : networks) {
    const auto found = _nets.find(network.name);
    if (found == _nets.end()) {
      OT_LOGW("indexed RC net ", network.name, " not found");
      continue;
    }
    // Match _rebase_unit(Spef&) including its unit-equivalence tolerance.
    if (std::fabs(cap_scale - 1.0f) >= 1e-2f) {
      for (auto &node : network.nodes)
        node.capacitance *= cap_scale;
    }
    if (std::fabs(res_scale - 1.0f) >= 1e-2f) {
      for (auto &edge : network.edges)
        edge.resistance *= res_scale;
    }
    found->second._attach(std::move(network));
    _insert_frontier(*found->second._root);
  }
  _remove_state(EPTS_UPDATED | AREA_UPDATED | POWER_UPDATED);
}

void Timer::update_bound_parasitics(std::vector<BoundRc> networks,
                                    farad_t capacitance_unit,
                                    ohm_t resistance_unit) {
  if (!std::isfinite(capacitance_unit.value()) ||
      capacitance_unit.value() <= 0 ||
      !std::isfinite(resistance_unit.value()) || resistance_unit.value() <= 0)
    OT_THROW(Error::RCT, "bound RC units must be finite and positive");
  std::scoped_lock lock(_mutex);
  _update_netlist();
  // Validate the entire batch before replacing any installed tree or units.
  for (const auto &net : networks) {
    if (net.driver >= _idx2pin.size() || !_idx2pin[net.driver] ||
        !_idx2pin[net.driver]->_net ||
        _idx2pin[net.driver]->_net->_root != _idx2pin[net.driver])
      OT_THROW(Error::RCT, "invalid bound RC driver");
    if (net.nodes.empty() ||
        net.nodes.size() >= std::numeric_limits<uint32_t>::max() ||
        net.edges.size() != net.nodes.size() - 1 ||
        net.edges.size() >= RctNode::no_edge / 2)
      OT_THROW(Error::RCT, "invalid bound RC tree size");
    std::unordered_set<size_t> pins;
    std::vector<size_t> parents(net.nodes.size());
    std::iota(parents.begin(), parents.end(), 0);
    const auto root = [&](size_t index) {
      while (parents[index] != index) {
        parents[index] = parents[parents[index]];
        index = parents[index];
      }
      return index;
    };
    for (const auto &node : net.nodes) {
      if (!std::isfinite(node.capacitance) || node.capacitance < 0)
        OT_THROW(Error::RCT, "invalid bound RC capacitance");
      if (node.pin != BoundRc::no_pin &&
          (node.pin >= _idx2pin.size() || !_idx2pin[node.pin] ||
           _idx2pin[node.pin]->_net != _idx2pin[net.driver]->_net ||
           !pins.insert(node.pin).second))
        OT_THROW(Error::RCT, "invalid or duplicate bound RC pin");
    }
    if (!pins.contains(net.driver) ||
        pins.size() != _idx2pin[net.driver]->_net->num_pins())
      OT_THROW(Error::RCT, "bound RC must cover every net pin");
    for (const auto &edge : net.edges) {
      if (edge.from >= net.nodes.size() || edge.to >= net.nodes.size() ||
          !std::isfinite(edge.resistance) || edge.resistance < 0)
        OT_THROW(Error::RCT, "invalid bound RC edge");
      const auto a = root(edge.from), b = root(edge.to);
      if (a == b)
        OT_THROW(Error::RCT, "bound RC contains a cycle");
      parents[a] = b;
    }
  }
  if (!_capacitance_unit)
    _capacitance_unit = capacitance_unit;
  if (!_resistance_unit)
    _resistance_unit = resistance_unit;
  const float cap_scale = (capacitance_unit / *_capacitance_unit).value();
  const float res_scale = (resistance_unit / *_resistance_unit).value();
  for (auto &network : networks) {
    if (std::fabs(cap_scale - 1.0f) >= 1e-2f)
      for (auto &node : network.nodes)
        node.capacitance *= cap_scale;
    if (std::fabs(res_scale - 1.0f) >= 1e-2f)
      for (auto &edge : network.edges)
        edge.resistance *= res_scale;
    _idx2pin[network.driver]->_net->_attach(network, _idx2pin);
    _insert_frontier(*_idx2pin[network.driver]);
  }
  _remove_state(EPTS_UPDATED | AREA_UPDATED | POWER_UPDATED);
}

// Procedure: _read_spef
void Timer::_read_spef(spef::Spef &spef, bool materialize) {
  for(auto& spef_net : spef.nets) {
    if(auto itr = _nets.find(spef_net.name); itr == _nets.end()) {
      OT_LOGW("spef net ", spef_net.name, " not found");
      continue;
    }
    else {
      itr->second._attach(std::move(spef_net));
      if (materialize) {
        itr->second._make_rct();
      }
      _insert_frontier(*itr->second._root);
    }
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------

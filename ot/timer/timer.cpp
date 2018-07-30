#include <ot/timer/timer.hpp>

namespace ot {


// ------------------------------------------------------------------------------------------------

// Function: num_threads
Timer& Timer::num_threads(unsigned n) {
  std::scoped_lock lock(_mutex);
  unsigned w = (n == 0) ? 0 : n-1;
  OT_LOGI("using ", n, " threads (", w, " worker)");
  _taskflow.num_workers(w);
  return *this;
}

// Procedure: _add_to_lineage
void Timer::_add_to_lineage(const tf::Taskflow::Task& last) {
  _lineage | [&] (auto& p) { p.precede(last); };
  _lineage = last;
}

// Function: _max_pin_name_size
size_t Timer::_max_pin_name_size() const {
  if(_pins.empty()) {
    return 0;
  }
  else {
    return std::max_element(_pins.begin(), _pins.end(), 
      [] (const auto& l, const auto& r) {
        return l.second._name.size() < r.second._name.size();
      }
    )->second._name.size();
  }
}

// Function: _max_net_name_size
size_t Timer::_max_net_name_size() const {
  if(_nets.empty()) {
    return 0;
  }
  else {
    return std::max_element(_nets.begin(), _nets.end(), 
      [] (const auto& l, const auto& r) {
        return l.second._name.size() < r.second._name.size();
      }
    )->second._name.size();
  }
}

// Function: repower_gate
// Change the size or level of an existing gate, e.g., NAND2_X2 to NAND2_X3. The gate's
// logic function and topology is guaranteed to be the same, along with the currently-connected
// nets. However, the pin capacitances of the new cell type might be different. 
Timer& Timer::repower_gate(std::string gate, std::string cell) {

  std::scoped_lock lock(_mutex);

  auto op = _taskflow.silent_emplace([this, gate=std::move(gate), cell=std::move(cell)] () {
    _repower_gate(gate, cell);
  });
  
  _add_to_lineage(op);
  
  return *this;
}

// Procedure: _repower_gate
void Timer::_repower_gate(const std::string& gname, const std::string& cname) {

  // Insert the gate if it doesn't exist.
  if(auto gitr = _gates.find(gname); gitr == _gates.end()) {
    OT_LOGW("gate ", gname, " doesn't exist (insert instead)");
    _insert_gate(gname, cname);
    return;
  }
  else {

    auto cell = CellView {_celllib[EARLY].cell(cname), _celllib[LATE].cell(cname)};

    OT_LOGE_RIF(!cell[EARLY] || !cell[LATE], "cell ", cname, " not found");

    auto& gate = gitr->second;
    
    // Remap the cell
    gate._repower(cell);

    // Insert the gate to the frontier
    for(auto pin : gate._pins) {
      _insert_frontier(*pin);
      for(auto arc : pin->_fanin) {
        _insert_frontier(arc->_from);
      }
    }
  }
}

// Fucntion: insert_gate
// Create a new gate in the design. This newly-created gate is "not yet" connected to
// any other gates or wires. The gate to insert cannot conflict with existing gates.
Timer& Timer::insert_gate(std::string gate, std::string cell) {  
  
  std::scoped_lock lock(_mutex);

  auto op = _taskflow.silent_emplace([this, gate=std::move(gate), cell=std::move(cell)] () {
    _insert_gate(gate, cell);
  });

  _add_to_lineage(op);

  return *this;
}

// Function: _insert_gate
void Timer::_insert_gate(const std::string& gname, const std::string& cname) {

  if(_gates.find(gname) != _gates.end()) {
    OT_LOGW("gate ", gname, " already existed");
    return;
  }

  auto cell = CellView {_celllib[EARLY].cell(cname), _celllib[LATE].cell(cname)};

  if(!cell[EARLY] || !cell[LATE]) {
    OT_LOGE("cell ", cname, " not found in celllib");
    return;
  }
  
  auto& gate = _gates.try_emplace(gname, gname, cell).first->second;
  
  // Insert pins
  for(const auto& [cpname, ecpin] : cell[EARLY]->cellpins) {

    CellpinView cpv {&ecpin, cell[LATE]->cellpin(cpname)};

    if(!cpv[EARLY] || !cpv[LATE]) {
      OT_LOGF("cellpin ", cpname, " mismatched in celllib");
    }

    auto& pin = _insert_pin(gname + ':' + cpname);
    pin._handle = cpv;
    
    gate._pins.push_back(&pin);
  }
  
  // Insert arcs
  FOR_EACH_EL(el) {

    for(const auto& [cpname, cp] : cell[el]->cellpins) {
      auto& to_pin = _insert_pin(gname + ':' + cpname);

      for(const auto& tm : cp.timings) {
        if(_is_redundant_timing(tm, el)) {
          continue;
        }

        TimingView tv{nullptr, nullptr};
        tv[el] = &tm;

        auto& from_pin = _insert_pin(gname + ':' + tm.related_pin);
        auto& arc = _insert_arc(from_pin, to_pin, tv);
        
        gate._arcs.push_back(&arc);
        if(tm.is_constraint()) {
          auto& test = _insert_test(arc);
          gate._tests.push_back(&test);
        }
      }
    }
  }
}

// Fucntion: remove_gate
// Remove a gate from the current design. This is guaranteed to be called after the gate has 
// been disconnected from the design using pin-level operations. The procedure iterates all 
// pins in the cell to which the gate was attached. Each pin that is being iterated is either
// a cell input pin or cell output pin. In the former case, the pin might have constraint arc
// while in the later case, the ot_pin.has no output connections and all fanin edges should be 
// removed here.
Timer& Timer::remove_gate(std::string gate) {  
  
  std::scoped_lock lock(_mutex);

  auto op = _taskflow.silent_emplace([this, gate=std::move(gate)] () {
    if(auto gitr = _gates.find(gate); gitr != _gates.end()) {
      _remove_gate(gitr->second);
    }
  });

  _add_to_lineage(op);

  return *this;
}

// Procedure: _remove_gate
void Timer::_remove_gate(Gate& gate) {

  // Disconnect this gate from the design.
  for(auto pin : gate._pins) {
    _disconnect_pin(*pin);
  }

  // Remove associated arcs
  for(auto arc : gate._arcs) {
    _remove_arc(*arc);
  }

  // Remove associated test
  for(auto test : gate._tests) {
    _remove_test(*test);
  }
  
  // Disconnect the gate and remove the pins from the gate
  for(auto pin : gate._pins) {
    _remove_pin(*pin);
  }

  // remove the gate
  _gates.erase(gate._name);
}

// Function: connect_pin
// Connect the pin to the corresponding net. The pin_name will either have the 
// <gate name>:<cell pin name> syntax (e.g., u4:ZN) or be a primary input. The net name
// will match an existing net read in from a .spef file.
Timer& Timer::connect_pin(std::string pin, std::string net) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, pin=std::move(pin), net=std::move(net)] () {
    auto pin_itr = _pins.find(pin);
    auto net_itr = _nets.find(net);
    if(pin_itr == _pins.end() || net_itr == _nets.end()) {
      OT_LOGW("ignore connecting pin ", pin,  " to net ", net, " (pin/net not found)");
      return;
    }
    _connect_pin(pin_itr->second, net_itr->second);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _connect_pin
void Timer::_connect_pin(Pin& pin, Net& net) {

  // Connect the pin to the net and construct the edge connections.
  net._insert_pin(pin);
  
  // Case 1: the pin is the root of the net.
  if(&pin == net._root) {
    for(auto leaf : net._pins) {
      if(leaf != &pin) {
        _insert_arc(pin, *leaf, net);
      }
    }
  }
  // Case 2: the pin is not a root of the net.
  else {
    if(net._root) {
      _insert_arc(*net._root, pin, net);
    }
  }

  // TODO(twhuang) Enable the clock tree update?
  //if(pin_ptr->node_ptr()->is_in_clock_tree()) {
  //  circuit_ptr()->clock_tree_ptr()->enable_clock_tree_update();
  //}
}

// Procedure: disconnect_pin
// Disconnect the pin from the net it is connected to. The pin_name will either have the 
// <gate name>:<cell pin name> syntax (e.g., u4:ZN) or be a primary input.
Timer& Timer::disconnect_pin(std::string name) {
  
  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name)] () {
    if(auto itr = _pins.find(name); itr != _pins.end()) {
      _disconnect_pin(itr->second);
    }
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: disconnect_pin
// TODO (twhuang)
// try get rid of find_fanin which can be wrong under multiple arcs.
void Timer::_disconnect_pin(Pin& pin) {

  auto net = pin._net;

  if(net == nullptr) return;

  // Case 1: the pin is a root of the net (i.e., root of the rctree)
  if(&pin == net->_root) {
    // Iterate the pinlist and delete the corresponding edge. Notice here we cannot iterate
    // fanout of the node during removal.
    for(auto leaf : net->_pins) {
      if(leaf != net->_root) {
        auto arc = leaf->_find_fanin(*net->_root);
        assert(arc);
        _remove_arc(*arc);
      }
    }
  }
  // Case 2: the pin is not a root of the net.
  else {
    if(net->_root) {
      auto arc = pin._find_fanin(*net->_root);
      assert(arc);
      _remove_arc(*arc);
    }
  }
  
  // TODO: Enable the clock tree update.
  //if(pin_ptr->node_ptr()->is_in_clock_tree()) {
  //  circuit_ptr()->clock_tree_ptr()->enable_clock_tree_update();
  //}
  
  // Remove the pin from the net and enable the rc timing update.
  net->_remove_pin(pin);
}

// Function: insert_net
// Creates an empty net object with the input "net_name". By default, it will not be connected 
// to any pins and have no parasitics (.spef). This net will be connected to existing pins in 
// the design by the "connect_pin" and parasitics will be loaded by "spef".
Timer& Timer::insert_net(std::string name) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name)] () {
    _insert_net(name);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Function: _insert_net
Net& Timer::_insert_net(const std::string& name) {
  return _nets.try_emplace(name, name).first->second;
}

// Procedure: remove_net
// Remove a net from the current design, which by default removes all associated pins.
Timer& Timer::remove_net(std::string name) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name)] () {
    if(auto itr = _nets.find(name); itr != _nets.end()) {
      _remove_net(itr->second);
    }
  });

  _add_to_lineage(modifier);

  return *this;
}

// Function: _remove_net
void Timer::_remove_net(Net& net) {

  if(net.num_pins() > 0) {
    auto fetch = net._pins;
    for(auto pin : fetch) {
      _disconnect_pin(*pin);
    }
  }

  _nets.erase(net._name);
}

// Function: _insert_pin
Pin& Timer::_insert_pin(const std::string& name) {
  
  // pin already exists
  if(auto [itr, inserted] = _pins.try_emplace(name, name); !inserted) {
    return itr->second;
  }
  // inserted a new pon
  else {
    
    // Generate the pin idx
    auto& pin = itr->second;
    
    // Assign the idx mapping
    pin._idx = _pin_idx_gen.get();
    resize_to_fit(pin._idx + 1, _idx2pin);
    _idx2pin[pin._idx] = &pin;

    // insert to frontier
    _insert_frontier(pin);

    return pin;
  }
}

// Function: _remove_pin
void Timer::_remove_pin(Pin& pin) {

  assert(pin.num_fanouts() == 0 && pin.num_fanins() == 0 && pin.net() == nullptr);

  _remove_frontier(pin);

  // remove the id mapping
  _idx2pin[pin._idx] = nullptr;
  _pin_idx_gen.recycle(pin._idx);

  // remove the pin
  _pins.erase(pin._name);
}

// Function: cppr
Timer& Timer::cppr(bool flag) {
  
  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, flag] () {
    _cppr(flag);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _cppr
// Enable/Disable common path pessimism removal (cppr) analysis
void Timer::_cppr(bool enable) {
  
  // nothing to do.
  if((enable && _cppr_analysis) || (!enable && !_cppr_analysis)) {
    return;
  }

  if(enable) {
    OT_LOGI("enable cppr analysis");
    _cppr_analysis.emplace();
  }
  else {
    OT_LOGI("disable cppr analysis");
    _cppr_analysis.reset();
  }
    
  for(auto& test : _tests) {
    _insert_frontier(test._constrained_pin());
  }
}

// Function: clock
Timer& Timer::clock(std::string name, float period) {
  
  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name), period] () {
    if(auto itr = _pins.find(name); itr != _pins.end()) {
      _clock(itr->first, itr->second, period);
    }
    else {
      OT_LOGW("can't create clock (pin ", name, " not found");
    }
  });
  
  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _clock
void Timer::_clock(const std::string& name, Pin& pin, float period) {
  _clocks.emplace(name, pin, period); 
  _insert_frontier(pin);
}

// Function: insert_primary_input
Timer& Timer::insert_primary_input(std::string name) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name)] () {
    _insert_primary_input(name);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _insert_primary_input
void Timer::_insert_primary_input(const std::string& name) {

  if(_pis.find(name) != _pis.end()) {
    OT_LOGW("can't insert PI ", name, " (already existed)");
    return;
  }

  assert(_pins.find(name) == _pins.end());

  // Insert the pin and and pi
  auto& pin = _insert_pin(name);
  auto& pi = _pis.try_emplace(name, pin).first->second;
  
  // Associate the connection.
  pin._handle = &pi;

  // Insert the pin to the frontier
  _insert_frontier(pin);

  // Create a net for the po and connect the pin to the net.
  auto& net = _insert_net(name); 
  
  // Connect the pin to the net.
  _connect_pin(pin, net);
}

// Function: insert_primary_output
Timer& Timer::insert_primary_output(std::string name) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name)] () {
    _insert_primary_output(name);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _insert_primary_output
void Timer::_insert_primary_output(const std::string& name) {

  if(_pos.find(name) != _pos.end()) {
    OT_LOGW("can't insert PO ", name, " (already existed)");
    return;
  }

  assert(_pins.find(name) == _pins.end());

  // Insert the pin and and pi
  auto& pin = _insert_pin(name);
  auto& po = _pos.try_emplace(name, pin).first->second;
  
  // Associate the connection.
  pin._handle = &po;

  // Insert the pin to the frontier
  _insert_frontier(pin);

  // Create a net for the po and connect the pin to the net.
  auto& net = _insert_net(name); 

  // Connect the pin to the net.
  _connect_pin(pin, net);
}

// Procedure: _insert_test
Test& Timer::_insert_test(Arc& arc) {
  auto& test = _tests.emplace_front(arc);
  test._satellite = _tests.begin();
  arc._to._tests.push_front(&test);
  return test;
}

// Procedure: _remove_test
void Timer::_remove_test(Test& test) {
  assert(test._satellite);
  _tests.erase(*test._satellite);
}

// Procedure: _remove_arc
// Remove an arc from the design. The procedure first disconnects the arc from its two ending
// pins, "from_pin" and "to_pin". Then it removes the arc from the design and insert both
// "from_pin" and "to_pin" into the pipeline.
void Timer::_remove_arc(Arc& arc) {

  assert(arc._satellite);
  
  arc._from._remove_fanout(arc);
  arc._to._remove_fanin(arc);

  // Insert the two ends to the frontier list.
  _insert_frontier(arc._from, arc._to);
  
  // remove the id mapping
  _idx2arc[arc._idx] = nullptr;
  _arc_idx_gen.recycle(arc._idx);

  // Remove this arc from the timer.
  _arcs.erase(*arc._satellite);
}

// Function: _insert_arc (net arc)
// Insert an net arc to the timer.
Arc& Timer::_insert_arc(Pin& from, Pin& to, Net& net) {

  // Create a new arc
  auto& arc = _arcs.emplace_front(from, to, net);
  arc._satellite = _arcs.begin();

  from._insert_fanout(arc);
  to._insert_fanin(arc);

  // Insert frontiers
  _insert_frontier(from, to);
   
  // Assign the idx mapping
  arc._idx = _arc_idx_gen.get();
  resize_to_fit(arc._idx + 1, _idx2arc);
  _idx2arc[arc._idx] = &arc;

  return arc;
}

// Function: _insert_arc (cell arc)
// Insert a cell arc to the timing graph. A cell arc is a combinational link.
Arc& Timer::_insert_arc(Pin& from, Pin& to, TimingView tv) {

  // Create a new arc
  auto& arc = _arcs.emplace_front(from, to, tv);
  arc._satellite = _arcs.begin();
  from._insert_fanout(arc);
  to._insert_fanin(arc);

  // insert the arc into frontier list.
  _insert_frontier(from, to);
  
  // Assign the idx mapping
  arc._idx = _arc_idx_gen.get();
  resize_to_fit(arc._idx + 1, _idx2arc);
  _idx2arc[arc._idx] = &arc;

  return arc;
}

// Procedure: _fprop_rc_timing
void Timer::_fprop_rc_timing(Pin& pin) {
  if(auto net = pin._net; net) {
    net->_update_rc_timing();
  }
}

// Procedure: _fprop_slew
void Timer::_fprop_slew(Pin& pin) {
  
  // clear slew  
  pin._reset_slew();

  // PI
  if(auto pi = pin.pi(); pi) {
    FOR_EACH_EL_RF_IF(el, rf, pi->_slew[el][rf]) {
      pin._relax_slew(nullptr, el, rf, el, rf, *(pi->_slew[el][rf]));
    }
  }
  
  // Relax the slew from its fanin.
  for(auto arc : pin._fanin) {
    arc->_fprop_slew();
  }
}

// Procedure: _fprop_delay
void Timer::_fprop_delay(Pin& pin) {

  // clear delay
  for(auto arc : pin._fanin) {
    arc->_reset_delay();
  }

  // Compute the delay from its fanin.
  for(auto arc : pin._fanin) {
    arc->_fprop_delay();
  }
}

// Procedure: _fprop_at
void Timer::_fprop_at(Pin& pin) {
  
  // clear at
  pin._reset_at();

  // PI
  if(auto pi = pin.pi(); pi) {
    FOR_EACH_EL_RF_IF(el, rf, pi->_at[el][rf]) {
      pin._relax_at(nullptr, el, rf, el, rf, *(pi->_at[el][rf]));
    }
  }

  // Relax the at from its fanin.
  for(auto arc : pin._fanin) {
    arc->_fprop_at();
  }
}

// Procedure: _fprop_test
void Timer::_fprop_test(Pin& pin) {
  
  // reset tests
  for(auto test : pin._tests) {
    test->_reset();
  }
  
  // Obtain the rat
  if(_clocks) {

    // Update the rat
    for(auto test : pin._tests) {
      test->_fprop_rat(_clocks->_period);
      
      // compute the cppr credit if any
      if(_cppr_analysis) {
        FOR_EACH_EL_RF_IF(el, rf, test->_rat[el][rf]) {
          test->_cppr_credit[el][rf] = _cppr_credit(*test, el, rf);
        }
      }
    }
  }
}

// Procedure: _bprop_rat
void Timer::_bprop_rat(Pin& pin) {

  pin._reset_rat();

  // PO
  if(auto po = pin.po(); po) {
    FOR_EACH_EL_RF_IF(el, rf, po->_rat[el][rf]) {
      pin._relax_rat(nullptr, el, rf, el, rf, *(po->_rat[el][rf]));
    }
  }

  // Test
  for(auto test : pin._tests) {
    FOR_EACH_EL_RF_IF(el, rf, test->_rat[el][rf]) {
      if(!_cppr_analysis) {
        pin._relax_rat(&test->_arc, el, rf, el, rf, *test->_rat[el][rf]);
      }
      else {
        pin._relax_rat(&test->_arc, el, rf, el, rf, *test->_rat[el][rf] + *test->_cppr_credit[el][rf]);
      }
    }
  }

  // Relax the rat from its fanout.
  for(auto arc : pin._fanout) {
    arc->_bprop_rat();
  }
}

// Procedure: _build_fprop_cands
// Performs DFS to find all nodes in the fanout cone of frontiers.
// Returns true if cycle is detected.
bool Timer::_build_fprop_cands(Pin& from) {
  
  assert(!from._has_state(Pin::FPROP_CAND) && !from._has_state(Pin::IN_FPROP_STACK));

  _fprop_cands.push_front(&from);
  from._insert_state(Pin::FPROP_CAND | Pin::IN_FPROP_STACK);

  for(auto arc : from._fanout) {
    if(auto& to = arc->_to; !to._has_state(Pin::FPROP_CAND) && _build_fprop_cands(to)) {
      return true;
    }
    else if (to._has_state(Pin::IN_FPROP_STACK)){
      return true;
    }
  }
  
  from._remove_state(Pin::IN_FPROP_STACK); 
  return false;
}

// Procedure: _build_fprop_tasks
// Construct the task dependency graph for the following fprop tasks:
// (1) propagate the rc timing
// (2) propagate the slew 
// (3) propagate the delay
// (4) propagate the arrival time.
void Timer::_build_fprop_tasks() {

  // Discover all fprop candidates.
  for(const auto& ftr : _frontiers) {
    if(!ftr->_has_state(Pin::FPROP_CAND)) {
      if(_build_fprop_cands(*ftr)) {
        OT_LOGF("timing graph contains a cycle");
      }
    }
  }

  // Emplace the fprop task
  for(auto pin : _fprop_cands) {
    assert(!pin->_ftask);
    pin->_ftask = _taskflow.silent_emplace([this, pin] () {
      _fprop_rc_timing(*pin);
      _fprop_slew(*pin);
      _fprop_delay(*pin);
      _fprop_at(*pin);
      _fprop_test(*pin);
    }).name("fprop_"s + pin->_name);
  }
  
  // Build the dependency
  for(auto to : _fprop_cands) {
    for(auto arc : to->_fanin) {
      if(auto& from = arc->_from; from._has_state(Pin::FPROP_CAND)) {
        from._ftask->precede(to->_ftask.value());
      }
    }
  }
}

// Procedure: _clear_fprop_tasks
void Timer::_clear_fprop_tasks() {
  for(auto& pin : _fprop_cands) {
    assert(pin->_has_no_state(Pin::IN_FPROP_STACK));
    pin->_ftask.reset();
    pin->_remove_state(Pin::FPROP_CAND);
  }
  _fprop_cands.clear();
}

// Procedure: _build_bprop_cands
// Perform the DFS to find all nodes in the fanin cone of fprop candidates.
// The function returns true if there contains a cycle.
bool Timer::_build_bprop_cands(Pin& to) {
  
  assert(!to._has_state(Pin::BPROP_CAND) && !to._has_state(Pin::IN_BPROP_STACK));

  _bprop_cands.push_front(&to);
  to._insert_state(Pin::BPROP_CAND | Pin::IN_BPROP_STACK);

  for(auto arc : to._fanin) {
    if(auto& from=arc->_from; !from._has_state(Pin::BPROP_CAND) && _build_bprop_cands(from)) {
      return true;
    }
    else if(from._has_state(Pin::IN_BPROP_STACK)) {
      return true;
    }
  }

  to._remove_state(Pin::IN_BPROP_STACK);
  return false;
}

// Procedure: _build_bprop_tasks
// Construct the task dependency graph for the following main bprop tasks:
// (1) propagate the rc timing
// (2) propagate the slew 
// (3) propagate the delay
// (4) propagate the arrival time.
void Timer::_build_bprop_tasks() {

  // Discover all bprop candidates.
  for(auto fcand : _fprop_cands) {
    if(!fcand->_has_state(Pin::BPROP_CAND)) {
      if(_build_bprop_cands(*fcand)) {
        OT_LOGF("timing graph contains a cycle");
      }
    }
  }
  
  // Emplace the bprop task
  for(auto pin : _bprop_cands) {
    assert(!pin->_btask);
    pin->_btask = _taskflow.silent_emplace([this, pin] () {
      _bprop_rat(*pin);
    }).name("bprop_"s + pin->_name);
  }

  // Build the task dependencies.
  for(auto to : _bprop_cands) {
    for(auto arc : to->_fanin) {
      if(auto& from = arc->_from; from._has_state(Pin::BPROP_CAND)) {
        to->_btask->precede(from._btask.value());
      }
    } 
  }

  // Connect with ftasks
  for(auto pin : _bprop_cands) {
    if(pin->_btask->num_dependents() == 0 && pin->_ftask) {
      pin->_ftask->precede(pin->_btask.value()); 
    }
  }
}

// Procedure: _clear_bprop_tasks
void Timer::_clear_bprop_tasks() {
  for(auto& pin : _bprop_cands) {
    assert(pin->_has_no_state(Pin::IN_BPROP_STACK));
    pin->_remove_state(Pin::BPROP_CAND);
    pin->_btask.reset();
  }
  _bprop_cands.clear();
}

// Function: update_timing
// Perform comprehensive timing update: 
// (1) grpah-based timing (GBA)
// (2) path-based timing (PBA)
void Timer::update_timing() {
  std::scoped_lock lock(_mutex);
  _update_timing();
}

// Function: _update_timing
void Timer::_update_timing() {
  
  // Timing is update-to-date
  if(!_lineage) return;

  _taskflow.wait_for_all();
  _lineage.reset();
  
  // Check if full update is required
  if(_has_state(FULL_TIMING)) {

    OT_LOGI("perform full timing update");

    // insert all zero-fanin pins to the frontier list
    for(auto& kvp : _pins) {
      if(auto& pin = kvp.second; pin.num_fanins() == 0) {
        _insert_frontier(pin);
      }
    }

    // clear the rc-net update flag
    for(auto& kvp : _nets) {
      kvp.second._rc_timing_updated = false;
    }
  }

  // Build the propagation tasks.
  _build_fprop_tasks();
  _build_bprop_tasks();
  
  //std::cout << _taskflow.dump() << '\n';

  // Execute the task
  _taskflow.wait_for_all();
  
  // Clear the propagation tasks.
  _clear_fprop_tasks();
  _clear_bprop_tasks();

  // Clear frontiers
  _clear_frontiers();

  // clear the state
  _remove_state();
}

// Procedure: _update_wns
void Timer::_update_wns() {
    
  _update_timing();

  if(_has_state(WNS_UPDATED)) {
    return;
  } 

  FOR_EACH_EL_RF(el, rf) {
    _wns[el][rf].reset();
    _taskflow.transform_reduce(_idx2pin.begin(), _idx2pin.end(),
      _wns[el][rf],
      [] (std::optional<float> l, std::optional<float> r) {
        return (!l || (r && *r < *l)) ? r : l;
      },
      [el, rf] (Pin* pin) -> std::optional<float> {
        return pin ? pin->slack(el, rf) : std::nullopt;
      }
    );
  } 

  _taskflow.wait_for_all();

  _insert_state(WNS_UPDATED);
}

// Procedure: _update_tns
void Timer::_update_tns() {
    
  _update_timing();

  if(_has_state(TNS_UPDATED)) {
    return;
  }

  FOR_EACH_EL_RF(el, rf) {
    _tns[el][rf].reset();
    _taskflow.transform_reduce(_idx2pin.begin(), _idx2pin.end(),
      _tns[el][rf],
      [] (std::optional<float> l, std::optional<float> r) {
        return (l && r) ? *l + *r : (l ? l : r);
      },
      [el, rf] (Pin* pin) -> std::optional<float> {
        if(!pin) return std::nullopt;
        if(auto slack = pin->slack(el, rf); slack && *slack < 0.0f) {
          return slack;
        }
        else {
          return std::nullopt;
        }
      }
    );
  }

  _taskflow.wait_for_all();

  _insert_state(TNS_UPDATED);
}

// Procedure: _update_endpoints
void Timer::_update_endpoints() {

  _update_timing();

  if(_has_state(EPTS_UPDATED)) {
    return;
  }

  // reset the storage and build task
  FOR_EACH_EL_RF(el, rf) {

    _endpoints[el][rf].clear();

    _taskflow.silent_emplace([this, el, rf] () {

      // for each po
      for(auto& po : _pos) {
        if(po.second.slack(el, rf).has_value()) {
          _endpoints[el][rf].emplace_back(el, rf, po.second);
        }
      }

      // for each test
      for(auto& test : _tests) {
        if(test.slack(el, rf).has_value()) {
          _endpoints[el][rf].emplace_back(el, rf, test);
        }
      }
      
      // sort endpoints
      std::sort(_endpoints[el][rf].begin(), _endpoints[el][rf].end());
    });
  }

  // run tasks
  _taskflow.wait_for_all();

  _insert_state(EPTS_UPDATED);
}

// Function: tns
// Update the total negative slack for any transition and timing split. The procedure applies
// the parallel reduction to compute the value.
std::optional<float> Timer::tns() {

  std::scoped_lock lock(_mutex);

  _update_tns();

  std::optional<float> v;

  FOR_EACH_EL_RF_IF(el, rf, _tns[el][rf]) {
    v = !v ? _tns[el][rf] : *v + *(_tns[el][rf]);
  }

  return v;
}

// Function: wns
// Update the total negative slack for any transition and timing split. The procedure apply
// the parallel reduction to compute the value.
std::optional<float> Timer::wns() {

  std::scoped_lock lock(_mutex);

  _update_wns();

  std::optional<float> v;

  FOR_EACH_EL_RF_IF(el, rf, _wns[el][rf]) {
    v = !v ? _wns[el][rf] : std::min(*v, *(_wns[el][rf]));
  }

  return v;
}
    
// Procedure: _enable_full_timing_update
void Timer::_enable_full_timing_update() {
  _insert_state(FULL_TIMING);
}

// Procedure: _insert_frontier
void Timer::_insert_frontier(Pin& pin) {
  if(!pin._frontier_satellite) {
    pin._frontier_satellite = _frontiers.insert(_frontiers.end(), &pin);
  }
}

// Procedure: _remove_frontier
void Timer::_remove_frontier(Pin& pin) {
  if(pin._frontier_satellite) {
    _frontiers.erase(*pin._frontier_satellite);
    pin._frontier_satellite.reset();
  }
}

// Procedure: _clear_frontiers
void Timer::_clear_frontiers() {
  for(auto& ftr : _frontiers) {
    ftr->_frontier_satellite.reset();
  }
  _frontiers.clear();
}

// Function: at   
// Report the arrival time in picoseconds at a given pin name.
std::optional<float> Timer::at(const std::string& name, Split m, Tran t) {
  std::scoped_lock lock(_mutex);
  return _at(name, m, t);
}

// Function: _at
std::optional<float> Timer::_at(const std::string& name, Split m, Tran t) {
  _update_timing();
  if(auto itr = _pins.find(name); itr != _pins.end() && itr->second._at[m][t]) {
    return itr->second._at[m][t]->numeric;
  }
  else return std::nullopt;
}

// Function: rat
// Report the required arrival time in picoseconds at a given pin name.
std::optional<float> Timer::rat(const std::string& name, Split m, Tran t) {
  std::scoped_lock lock(_mutex);
  return _rat(name, m, t);
}

// Function: _rat
std::optional<float> Timer::_rat(const std::string& name, Split m, Tran t) {
  _update_timing();
  if(auto itr = _pins.find(name); itr != _pins.end() && itr->second._at[m][t]) {
    return itr->second._rat[m][t];
  }
  else return std::nullopt;
}

// Function: slew
// Report the slew in picoseconds at a given pin name.
std::optional<float> Timer::slew(const std::string& name, Split m, Tran t) {
  std::scoped_lock lock(_mutex);
  return _slew(name, m, t);
}

// Function: _slew
std::optional<float> Timer::_slew(const std::string& name, Split m, Tran t) {
  _update_timing();
  if(auto itr = _pins.find(name); itr != _pins.end() && itr->second._slew[m][t]) {
    return itr->second._slew[m][t]->numeric;
  }
  else return std::nullopt;
}

// Function: slack
std::optional<float> Timer::slack(const std::string& pin, Split m, Tran t) {
  std::scoped_lock lock(_mutex);
  return _slack(pin, m, t);
}

// Function: _slack
std::optional<float> Timer::_slack(const std::string& pin, Split m, Tran t) {
  _update_timing();
  if(auto itr = _pins.find(pin); itr != _pins.end()) {
    return itr->second.slack(m, t);
  }
  else return std::nullopt;
}

// Function: load
// Report the load at a given pin name
std::optional<float> Timer::load(const std::string& name, Split m, Tran t) {
  std::scoped_lock lock(_mutex);
  return _load(name, m, t);
}

// Function: load
std::optional<float> Timer::_load(const std::string& name, Split m, Tran t) {
  _update_timing();
  if(auto itr = _nets.find(name); itr != _nets.end()) {
    return itr->second._load(m, t);
  }
  else return std::nullopt;
}

// Function: at
Timer& Timer::at(std::string name, Split m, Tran t, std::optional<float> v) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name), m, t, v] () {
    _at(name, m, t, v);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _at
void Timer::_at(const std::string& name, Split m, Tran t, std::optional<float> v) {
  if(auto itr = _pis.find(name); itr != _pis.end()) {
    _at(itr->second, m, t, v);
  }
  else {
    OT_LOGW("can't assert arrival time (PI ", name, " not found)");
  }
}

// Procedure: _at
void Timer::_at(PrimaryInput& pi, Split m, Tran t, std::optional<float> v) {
  pi._at[m][t] = v;
  _insert_frontier(pi._pin);
}

// Function: rat
Timer& Timer::rat(std::string name, Split m, Tran t, std::optional<float> v) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name), m, t, v] () {
    _rat(name, m, t, v);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _rat
void Timer::_rat(const std::string& name, Split m, Tran t, std::optional<float> v) {
  if(auto itr = _pos.find(name); itr != _pos.end()) {
    _rat(itr->second, m, t, v);
  }
  else {
    OT_LOGW("can't assert required arrival time (PO ", name, " not found)");
  }
}

// Procedure: _rat
void Timer::_rat(PrimaryOutput& po, Split m, Tran t, std::optional<float> v) {
  po._rat[m][t] = v;
  _insert_frontier(po._pin);
}

// Function: slew
Timer& Timer::slew(std::string name, Split m, Tran t, std::optional<float> v) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name), m, t, v] () {
    _slew(name, m, t, v);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: slew
void Timer::_slew(const std::string& name, Split m, Tran t, std::optional<float> v) {
  if(auto itr = _pis.find(name); itr != _pis.end()) {
    _slew(itr->second, m, t, v);
  }
  else {
    OT_LOGW("can't assert slew (PI ", name, " not found)");
  }
}

// Procedure: _slew
void Timer::_slew(PrimaryInput& pi, Split m, Tran t, std::optional<float> v) {
  pi._slew[m][t] = v;
  _insert_frontier(pi._pin);
}

// Function: load
Timer& Timer::load(std::string name, Split m, Tran t, std::optional<float> v) {

  std::scoped_lock lock(_mutex);

  auto modifier = _taskflow.silent_emplace([this, name=std::move(name), m, t, v] () {
    _load(name, m, t, v);
  });

  // parent -> modifier
  _add_to_lineage(modifier);

  return *this;
}

// Procedure: _load
void Timer::_load(const std::string& name, Split m, Tran t, std::optional<float> v) {
  if(auto itr = _pos.find(name); itr != _pos.end()) {
    _load(itr->second, m, t, v);
  }
  else {
    OT_LOGW("can't assert load to PO ", name, " (not found)");
  }
}

// Procedure: _load
void Timer::_load(PrimaryOutput& po, Split m, Tran t, std::optional<float> v) {

  po._load[m][t] = v ? *v : 0.0f;

  // Update the net load
  if(auto net = po._pin._net) {
    net->_rc_timing_updated = false;
  }
  
  // Enable the timing propagation.
  for(auto arc : po._pin._fanin) {
    _insert_frontier(arc->_from);
  }
  _insert_frontier(po._pin);
}


};  // end of namespace ot. -----------------------------------------------------------------------





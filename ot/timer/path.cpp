#include <ot/timer/timer.hpp>

namespace ot {

// Constructor
Point::Point(const Pin& p, Tran t, float v) :
  pin        {p},
  transition {t},
  at         {v} {
}

// ------------------------------------------------------------------------------------------------

// Constructor
Path::Path(float slk, const Endpoint* ept) :
  slack    {slk},
  endpoint {ept} {
}

// Procedure: dump_tau18
/*
void Path::dump_tau18(std::ostream& os) const{

  std::regex replace(":");

  auto el = endpoint->split();
  auto rf = endpoint->transition();

  os << "Endpoint: " << std::regex_replace(back().pin.name(), replace, "/")  << '\n';
  os << "Beginpoint: " << std::regex_replace(front().pin.name(), replace, "/") << '\n';
  //os << "= Required Time " << '\n'; //TODO: ignore RAT for tau18 benchmark
  float rat = 0.0;
  if(endpoint->test() != nullptr){
    rat = *(endpoint->test()->rat(el, rf));
  }
  else{
    rat = *(endpoint->primary_output()->rat(el, rf));
  }
  auto beg_at = front().at;
  auto end_at = back().at;
  auto path_slack = el == MIN ? ((end_at - beg_at) - rat) : (rat - (end_at - beg_at));
  os << "= Required Time " << rat << '\n';
  //Arrival Time is the total delay
  os << "- Arrival Time " << end_at - beg_at << '\n';
  //os << "- Arrival Time " << back().at << '\n';
  os << "= Slack Time " << path_slack << '\n';

  float at_offset = front().at;
  std::optional<float> pi_at;

  for(const auto& p : *this) {

    if(!pi_at){ os << "- "; }
    else{ os << p.at-*pi_at << " "; }
    os << p.at-at_offset << " ";

    if(p.transition == RISE){ os << "^ "; }
    else{ os << "v "; }

    os << std::regex_replace(p.pin.name(), replace, "/") << '\n';
    pi_at = p.at;
  }
  os << '\n';

} 
*/

// Procedure: dump_tau18
void Path::dump_tau18(std::ostream& os) const{

  std::regex replace(":");

  auto el = endpoint->split();
  auto rf = endpoint->transition();

  os << std::setw(20) << "Endpoint: "   
     << std::setw(10) << std::regex_replace(back().pin.name(), replace, "/")  << '\n';
  os << std::setw(20) << "Beginpoint: " 
     << std::setw(10) << std::regex_replace(front().pin.name(), replace, "/") << '\n';
  //os << "= Required Time " << '\n'; //TODO: ignore RAT for tau18 benchmark
  float rat = 0.0;
  if(endpoint->test() != nullptr){
    rat = *(endpoint->test()->rat(el, rf));
  }
  else{
    rat = *(endpoint->primary_output()->rat(el, rf));
  }
  auto beg_at = front().at;
  auto end_at = back().at;
  auto path_slack = el == MIN ? ((end_at - beg_at) - rat) : (rat - (end_at - beg_at));
  os << std::setw(20) << "= Required Time " << std::setw(10) << rat << '\n';
  //Arrival Time is the total delay
  os << std::setw(20) << "- Arrival Time " << std::setw(10) << end_at - beg_at << '\n';
  //os << "- Arrival Time " << back().at << '\n';
  os << std::setw(20) << "= Slack Time " << std::setw(10) << path_slack << '\n';

  float at_offset = front().at;
  std::optional<float> pi_at;

  for(const auto& p : *this) {
    os << std::setw(10);
    
    if(!pi_at){ os << "- "; }
    else{ os << p.at-*pi_at << " "; }

    os << std::setw(10);

    os << p.at-at_offset << " ";

    os << std::setw(3);

    if(p.transition == RISE){ os << "^ "; }
    else{ os << "v "; }

    os << std::setw(20);

    os << std::regex_replace(p.pin.name(), replace, "/") << '\n';
    pi_at = p.at;
  }
  os << '\n';

}

// Procedure: dump
// dump the path in the following format:
//
// Startpoint    : inp1
// Endpoint      : f1:D
// Analysis type : early
// ------------------------------------------------------
//        Type       Delay        Time   Dir  Description
// ------------------------------------------------------
//        port       0.000       0.000  fall  inp1
//         pin       0.000       0.000  fall  u1:A (NAND2X1)
//         pin       2.786       2.786  rise  u1:Y (NAND2X1)
//         pin       0.000       2.786  rise  u4:A (NOR2X1)
//         pin       0.181       2.967  fall  u4:Y (NOR2X1)
//         pin       0.000       2.967  fall  f1:D (DFFNEGX1)
//     arrival                   2.967        data arrival time
// 
//       clock      25.000      25.000  fall  f1:CLK (DFFNEGX1)
//  constraint       1.518      26.518        library hold_falling
//    required                  26.518        data required time
// ------------------------------------------------------
//       slack                 -23.551        VIOLATED
//
void Path::dump(std::ostream& os) const {

  if(empty()) {
    os << "empty path\n";
    return;
  }

  auto fmt   = os.flags();
  auto split = endpoint->split();
  auto tran  = endpoint->transition();
  auto at    = back().at;
  auto rat   = (split == MIN ? at - slack : at + slack);
  
  // Print the head
  os << "Startpoint    : " << front().pin.name() << '\n';
  os << "Endpoint      : " << back().pin.name()  << '\n';
  os << "Analysis type : " << to_string(split) << '\n';
  
  size_t w1 = 11;
  size_t w2 = 12;
  size_t w3 = 12;
  size_t w4 = 6;
  size_t w5 = 13;
  size_t W = w1 + w2 + w3 + w4 + w5;

  std::fill_n(std::ostream_iterator<char>(os), W, '-');
  os << '\n'
     << std::setw(w1) << "Type"
     << std::setw(w2) << "Delay"
     << std::setw(w3) << "Time"
     << std::setw(w4) << "Dir";
  std::fill_n(std::ostream_iterator<char>(os), 2, ' ');
  os << "Description" << '\n';
  std::fill_n(std::ostream_iterator<char>(os), W, '-');
  os << '\n';
  
  // trace
  os << std::fixed << std::setprecision(3);
  std::optional<float> pi_at;

  for(const auto& p : *this) {

    // type
    if(p.pin.primary_input() || p.pin.primary_output()) {
      os << std::setw(w1) << "port";
    }
    else {
      os << std::setw(w1) << "pin";
    }

    // delay
    os << std::setw(w2);
    if(pi_at) os << p.at - *pi_at;
    else os << p.at;
    
    // arrival time
    os << std::setw(w3) << p.at;

    // transition
    os << std::setw(w4) << to_string(p.transition);

    // pin name
    std::fill_n(std::ostream_iterator<char>(os), 2, ' ');
    if(os << p.pin.name(); p.pin.gate()) {
      os << " (" << p.pin.gate()->cell_name() << ')';
    }
    os << '\n';
    
    // cursor
    pi_at = p.at;
  }

  os << std::setw(w1) << "arrival" 
     << std::setw(w2+w3) << at;
  std::fill_n(std::ostream_iterator<char>(os), w4 + 2, ' ');
  os << "data arrival time" << '\n'; 

  // Print the required arrival time
  os << '\n';
  
  // test type
  std::visit(Functors{
    [&] (Test* test) {
        
      auto tv  = (test->_arc.timing_view())[split];
      auto sum = 0.0f;

      // related pin latency
      os << std::setw(w1) << "related pin";
      if(auto c = test->_related_at[split][tran]; c) {
        sum += *c;
        os << std::setw(w2) << *c << std::setw(w3) << sum;
      }
      else {
        os << std::setw(w2+w3) << "n/a";
      }

      if(tv && tv->is_rising_edge_triggered()) {
        os << std::setw(w4) << "rise";
      }
      else if(tv && tv->is_falling_edge_triggered()){
        os << std::setw(w4) << "fall";
      }
      else {
        os << "n/a";
      }

      std::fill_n(std::ostream_iterator<char>(os), 2, ' ');
      if(os << test->related_pin().name(); test->related_pin().gate()) {
        os << " (" << test->related_pin().gate()->cell_name() << ')';
      }
      os << '\n';

      // constraint value
      os << std::setw(w1) << "constraint";
      if(auto c = test->_constraint[split][tran]; c) {

        switch(split) {
          case MIN: 
            sum += *c;
            os << std::setw(w2) << c.value() << std::setw(w3) << sum;
          break;

          case MAX:
            sum -= *c;
            os << std::setw(w2) << -c.value() << std::setw(w3) << sum;
          break;
        }
        
        // timing type
        if(tv && tv->type) {
          std::fill_n(std::ostream_iterator<char>(os), w4+2, ' ');
          os << "library " << to_string(tv->type.value()) << '\n';
        }
        else {
          os << '\n';
        }
      }
      else {
        os << std::setw(w2) << "n/a" << '\n';
      }
      
      // cppr credit
      if(auto c = test->_cppr_credit[split][tran]; c) {
        os << std::setw(w1) << "cppr credit";
        sum += *c;
        os << std::setw(w2) << *c << std::setw(w3) << sum << '\n';
      }

      OT_LOGW_IF(
        std::fabs(sum - rat) > 1.0f, 
        "unstable numerics in PBA and GBA rats: ", sum, " vs ", rat
      );
    },
    [&] (PrimaryOutput* po) {
      os << std::setw(w1) << "port";
      if(auto v = po->rat(split, tran); v) {
        os << std::setw(w2) << *v << std::setw(w3) << *v;
        std::fill_n(std::ostream_iterator<char>(os), w4+2, ' ');
        os << "output port delay" << '\n';
      }
      else {
        os << std::setw(w2) << "n/a" << '\n';
      }
    }
  }, endpoint->_handle);
  
  os << std::setw(w1) << "required" << std::setw(w2+w3) << rat;
  std::fill_n(std::ostream_iterator<char>(os), w4+2, ' ');
  os << "data required time" << '\n';

  // slack
  std::fill_n(std::ostream_iterator<char>(os), W, '-');
  os << '\n' << std::setw(w1) << "slack" << std::setw(w2+w3) << slack;
  std::fill_n(std::ostream_iterator<char>(os), w4+2, ' ');
  os << (slack < 0.0f ? "VIOLATED" : "MET") << '\n';
  
  // restore the format
  os.flags(fmt);
}

// Operator <<
std::ostream& operator << (std::ostream& os, const Path& path) {
  path.dump(os);
  return os;
}

// ------------------------------------------------------------------------------------------------

// Functoin: _extract
// Extract the path in ascending order.
std::vector<Path> PathHeap::extract() {
  std::sort_heap(_paths.begin(), _paths.end(), _comp);
  std::vector<Path> P;
  P.reserve(_paths.size());
  std::transform(_paths.begin(), _paths.end(), std::back_inserter(P), [] (auto& ptr) {
    return std::move(*ptr);
  });
  _paths.clear();
  return P;
}

// Procedure: push
void PathHeap::push(std::unique_ptr<Path> path) {
  _paths.push_back(std::move(path));
  std::push_heap(_paths.begin(), _paths.end(), _comp);
}

// Procedure: pop
void PathHeap::pop() {
  if(_paths.empty()) {
    return;
  }
  std::pop_heap(_paths.begin(), _paths.end(), _comp);
  _paths.pop_back();
}

// Function: top
Path* PathHeap::top() const {
  return _paths.empty() ? nullptr : _paths.front().get();
}

// Procedure: fit
void PathHeap::fit(size_t K) {
  while(_paths.size() > K) {
    pop();
  }
}

// Procedure: heapify
void PathHeap::heapify() {
  std::make_heap(_paths.begin(), _paths.end(), _comp);   
}

// Procedure: merge_and_fit
void PathHeap::merge_and_fit(PathHeap&& rhs, size_t K) {

  if(_paths.capacity() < rhs._paths.capacity()) {
    _paths.swap(rhs._paths);
  }

  std::sort_heap(_paths.begin(), _paths.end(), _comp);
  std::sort_heap(rhs._paths.begin(), rhs._paths.end(), _comp);

  auto mid = _paths.insert(
    _paths.end(), 
    std::make_move_iterator(rhs._paths.begin()),
    std::make_move_iterator(rhs._paths.end())
  ); 

  rhs._paths.clear();

  std::inplace_merge(_paths.begin(), mid, _paths.end(), _comp);
  
  if(_paths.size() > K) {
    _paths.resize(K);
  }
  
  heapify(); 
}

// Function: dump
std::string PathHeap::dump() const {
  std::ostringstream oss;
  oss << "# Paths: " << _paths.size() << '\n';
  for(size_t i=0; i<_paths.size(); ++i) {
    oss << "slack[" << i << "]: " << _paths[i]->slack << '\n'; 
  }
  return oss.str();
}

// ------------------------------------------------------------------------------------------------

// Function: report_timing 
// Report the top-k report_timing
std::vector<Path> Timer::report_timing(size_t K) {
  std::scoped_lock lock(_mutex);
  return _report_timing(_worst_endpoints(K), K);
}

// Function: report_timing
std::vector<Path> Timer::report_timing(size_t K, Split el) {
  std::scoped_lock lock(_mutex);
  return _report_timing(_worst_endpoints(K, el), K);
}

// Function: report_timing
std::vector<Path> Timer::report_timing(size_t K, Tran rf) {
  std::scoped_lock lock(_mutex);
  return _report_timing(_worst_endpoints(K, rf), K);
}

// Function: report_timing
std::vector<Path> Timer::report_timing(size_t K, Split el, Tran rf) {
  std::scoped_lock lock(_mutex);
  return _report_timing(_worst_endpoints(K, el, rf), K);
}

// Function: _report_timing
// Report the top-k report_timing
std::vector<Path> Timer::_report_timing(std::vector<Endpoint*>&& epts, size_t K, PathGuide* pg) {

  // Comment out this for PathGuide
  //assert(epts.size() <= K);
  
  // No need to report anything.
  if(K == 0 || epts.empty()) {
    return {};
  }

  // No need to generate prefix tree
  if(K == 1) {
    std::vector<Path> paths;
    paths.emplace_back(epts[0]->slack(), epts[0]);
    auto sfxt = _sfxt_cache(*epts[0], pg);

    // Comment this out for PathGuide
    //OT_LOGW_IF(
    //  std::fabs(*sfxt.slack() - paths[0].slack) > 1.0f, 
    //  "unstable numerics in PBA and GBA slacks: ", *sfxt.slack(), " vs ", paths[0].slack
    //);

    //assert(std::fabs(*sfxt.slack() - paths[0].slack) < 0.1f);
    _recover_datapath(paths[0], sfxt);
    return paths;
  }
  
  // Generate the prefix tree
  PathHeap heap;

  _taskflow.transform_reduce(epts.begin(), epts.end(), heap,
    [&] (PathHeap l, PathHeap r) {
      l.merge_and_fit(std::move(r), K);
      return l;
    },
    [&, pg] (PathHeap heap, Endpoint* ept) {
      _spur(*ept, K, heap, pg);
      return heap;
    },
    [&, pg] (Endpoint* ept) {
      PathHeap heap;
      _spur(*ept, K, heap, pg);
      return heap;
    }
  );

  _taskflow.wait_for_all();

  return heap.extract();
}

// Procedure: _recover_prefix
// Recover the worst path prefix at a given pin.
void Timer::_recover_prefix(Path& path, const SfxtCache& sfxt, size_t idx) const {
  
  auto el = sfxt._el;
  auto [v, rf] = _decode_pin(idx);

  assert(v->_at[el][rf]);
  
  path.emplace_front(*v, rf, *v->_at[el][rf]);

  if(auto arc = v->_at[el][rf]->pi_arc; arc) {
    _recover_prefix(path, sfxt, _encode_pin(arc->_from, v->_at[el][rf]->pi_rf));
  }
}

// Procedure: _recover_datapath
// Recover the worst data path from a given suffix tree.
void Timer::_recover_datapath(Path& path, const SfxtCache& sfxt) const {
  
  if(!sfxt.__tree[sfxt._S]) {
    return;
  }

  auto u = *sfxt.__tree[sfxt._S];
  auto [upin, urf] = _decode_pin(u);

  // data path source
  assert(upin->_at[sfxt._el][urf]);
  path.emplace_back(*upin, urf, *upin->_at[sfxt._el][urf]);
  
  // recursive
  while(u != sfxt._T) {
    assert(sfxt.__link[u]);
    auto [arc, frf, trf] = _decode_arc(*sfxt.__link[u]);
    u = *sfxt.__tree[u];
    std::tie(upin, urf) = _decode_pin(u);
    assert(path.back().transition == frf && urf == trf);
    auto at = path.back().at + *arc->_delay[sfxt._el][frf][trf];
    path.emplace_back(*upin, urf, at);
  }
}

// Procedure: _recover_datapath
// recover the data path from a given prefix tree node w.r.t. a suffix tree
void Timer::_recover_datapath(
  Path& path, const SfxtCache& sfxt, const PfxtNode* node, size_t v
) const {

  if(node == nullptr) {
    return;
  }

  _recover_datapath(path, sfxt, node->parent, node->from);

  auto u = node->to;
  auto [upin, urf] = _decode_pin(u);
  
  // data path source
  if(node->from == sfxt._S) {
    assert(upin->_at[sfxt._el][urf]);
    path.emplace_back(*upin, urf, *upin->_at[sfxt._el][urf]);
  }
  // internal deviation
  else {
    assert(!path.empty());
    auto at = path.back().at + *node->arc->_delay[sfxt._el][path.back().transition][urf];
    path.emplace_back(*upin, urf, at);
  }

  while(u != v) {
    assert(sfxt.__link[u]);
    auto [arc, frf, trf] = _decode_arc(*sfxt.__link[u]);
    u = *sfxt.__tree[u];   
    std::tie(upin, urf) = _decode_pin(u);
    assert(path.back().transition == frf && urf == trf);
    auto at = path.back().at + *arc->_delay[sfxt._el][frf][trf]; 
    path.emplace_back(*upin, urf, at);
  }
}



// ------------------------------------------------------------------------------------------------
// PathGuide Related Functions  

// Function: report_timing
std::vector<Path> Timer::report_timing(PathGuide& guide) {
  std::scoped_lock lock(_mutex);
  _update_timing();
  _update_path_guide(guide);
  return _report_timing(std::move(_worst_endpoints(guide)), guide._num_request_paths.value(), &guide);
}

// DFS backward: Modify has_pin, dirty_entry, idx2rank
void Timer::_build_rank(PathGuide& pg, Pin& pin, size_t& rank) {
  //mark both rise and fall as visited
  size_t pin_idx = pin.idx();
  pg._has_pin[pin_idx] = true;
  pg._runtime.dirty_entry.push_back(pin_idx);
  _dirty_rank.push_back(_encode_pin(pin, RISE));
  _dirty_rank.push_back(_encode_pin(pin, FALL));

  // Stop at the data source
  if(!pin.is_datapath_source()) {
    for(auto arc : pin._fanin) {

      bool is_arc = false;
      FOR_EACH_EL_RF_RF(el, urf, vrf){
        is_arc = is_arc || arc->_delay[el][urf][vrf].has_value();
      }    

      //There is a possbile arc
      if(is_arc) {
        auto u = arc->_from.idx();
        if(!pg._has_pin[u] && !_idx2rank[u]) {
          _build_rank(pg, arc->_from, rank);
        }
      }
    }
  }

  //for the same pin, RISE and FALL are adjacent 
  _idx2rank[_encode_pin(pin, RISE)] = rank;
  _idx2rank[_encode_pin(pin, FALL)] = rank+1;
  rank += 2;
}

void Timer::_update_path_guide(PathGuide& pg) {

  if(_pins.find(pg._through.back().name) == _pins.end()) {
    throw std::runtime_error("Invalid to pin " + pg._through.back().name);
  }

  auto last_pin = &(_pins.find(pg._through.back().name)->second);

  pg._runtime.last_pin_is_endpoint = (!last_pin->_tests.empty() || last_pin->primary_output() != nullptr);

  pg._runtime.s = _idx2pin.size() << 1;
  if(pg._through.back().rf.has_value() && *pg._through.back().rf == RISE){
    pg._runtime.t = _encode_pin(*last_pin, RISE);
  }
  else {
    pg._runtime.t = _encode_pin(*last_pin, FALL);
  }

  resize_to_fit(std::max(pg._runtime.s, pg._runtime.t)+1, pg._idx2lvl, pg._has_pin);

  // This is a heuristic. We clear ranks after reusing certain number of times
  if(_sort_cnt == 32) {
    for(const auto& p : _dirty_rank){
      _idx2rank[p].reset();
    }
    _dirty_rank.clear();
    _max_rank = 0; 
    _sort_cnt = 0; 
  }

  // If ranks are not built yet
  if(!_idx2rank[last_pin->idx()]){
    size_t rank_offset {_max_rank};

    // Assign ranks to visited nodes
    _build_rank(pg, *last_pin, rank_offset);

    _max_rank = rank_offset;
    _sort_cnt ++;

    // Must clear the temporary storage: has_pin & dirty_entry  
    _clear_visited_list(pg);
  }

  // Assign ranks to through nodes
  for(auto &node: pg._through) {
    if(_pins.find(node.name) == _pins.end()) {
      throw std::runtime_error("Invalid through pin " + node.name);
    }

    auto thru_pin = &(_pins.find(node.name)->second);
    size_t thru_pinr = _encode_pin(*thru_pin, RISE);
    size_t thru_pinf = _encode_pin(*thru_pin, FALL);

    // No rise fall specified
    if(!node.rf){
      node.indices.push_back(thru_pinr);
      node.indices.push_back(thru_pinf);
      //thru with no rise/fall specified, let rank be RISE
      node.rank = _idx2rank[thru_pinr].value();
    }    
    // Rise
    else if(*node.rf == RISE){
      node.indices.push_back(thru_pinr);
      node.rank = _idx2rank[thru_pinr].value();
    }    
    // Fall
    else{
      node.indices.push_back(thru_pinf);
      node.rank =  _idx2rank[thru_pinf].value();
    }  
  }

  std::sort(pg._through.begin(), pg._through.end(), [](const auto& a, const auto& b){ return a.rank < b.rank;});

  _collect_pins(pg);  
}


bool Timer::_is_fanin_inbound(const PathGuide& pg, size_t from, size_t dst_level) const {
  if(!_idx2rank[from]){
    return false;
  }

  //decode level
  size_t level = dst_level % pg._through.size();

  if(level == 0){
    return true;
  }

  size_t from_rank = _idx2rank[from].value();
  //src_lvl = dst_lvl-1
  const auto& node = pg._through[level-1];
  if(!node.rf){
    return from_rank >= node.rank;
  }
  else if(*node.rf == RISE){
    return from_rank == node.rank || (from_rank > (node.rank+1));
  }
  else{
    return from_rank >= node.rank;
  }
}

// Check if the to pin is in range, level is the destination level
bool Timer::_is_fanout_inbound(const PathGuide& pg, size_t to, size_t dst_level) const {

  if(!_idx2rank[to]){
    return false;
  }

  const size_t max_level = pg._through.size();
  size_t level = dst_level % max_level;
  if(dst_level >= max_level && level < (max_level-1)){
    level++;
  }

  size_t to_rank = _idx2rank[to].value();
  const auto& thru_node = pg._through[level];

  // Two conditions: 
  //   1. the to_pin is the same as the through node
  //   2. the to_pin is a different node
  if(!thru_node.rf){
    return to_rank < (thru_node.rank+2);
  }
  else if(*thru_node.rf == RISE){
    return to_rank <= thru_node.rank;
  }
  else{
    return (to_rank < (thru_node.rank-1)) || to_rank == thru_node.rank;
  }
}

void Timer::_clear_visited_list(PathGuide& pg) {
  for(const auto& p: pg._runtime.dirty_entry){
    pg._has_pin[p].reset();
  }
  pg._runtime.dirty_entry.clear();
}


// Modify idx2lvl, pins
void Timer::_build_level(PathGuide& pg, size_t v, size_t level){

  pg._idx2lvl[v] = level;
  auto [pin, vrf] = _decode_pin(v);

  // Stop at the data source only at the first level
  if(!pin->is_datapath_source() || level > 0) {
    for(auto arc : pin->_fanin) {
      //TODO: Question: el affect connection between pins?
      FOR_EACH_EL_RF_IF(el, urf, arc->_delay[el][urf][vrf]) {
        auto u = _encode_pin(arc->_from, urf);
        //if the pin is not visited before AND reachable from source AND a fanin arc in range
        if(!pg._idx2lvl[u] && 
           (pg._has_pin[u] || level==0) && 
           _is_fanin_inbound(pg, u, level)) {
          _build_level(pg, u, level);
        }
      }
    }
  }

  pg._runtime.pins.push_back(v);
}

// Modify: idx2lvl, pins
// DFS backward
void Timer::_dfs_backward(PathGuide& pg, size_t dst_level) {
  const auto& node = pg._through[dst_level];
  for(auto idx : node.indices){
    if(dst_level == 0 || pg._has_pin[idx]){
      _build_level(pg, idx, dst_level);
    }

    // requires a special encoding to differentiate internal nodes and boundary nodes
    // dst_level + _though.size() denotes the boundary nodes
    if(pg._idx2lvl[idx]) {
      pg._idx2lvl[idx] = pg._through.size() + dst_level;
      if(dst_level == pg._through.size()-1) { //last level
        pg._runtime.pins.pop_back(); 
      }
    }
  }

  // If this is the last level 
  if(dst_level == pg._through.size()-1){

    _clear_visited_list(pg);

    for(auto idx : node.indices){
      if(pg._idx2lvl[idx].has_value()){
        // We leave only the last level as marked
        // to find the tail section later
        pg._has_pin[idx] = true;
        pg._runtime.dirty_entry.push_back(idx);
      }
    }
  }
}


// Modify: has_pin, dirty_entry
// BFS forward
void Timer::_bfs_forward(PathGuide& pg, size_t src_level) {
  const auto& src_node = pg._through[src_level];
  std::queue<size_t> queue;
  
  for(auto idx: src_node.indices) {
    pg._has_pin[idx] = true;
    queue.push(idx);
  }
  
  while(!queue.empty()) {
    size_t u = queue.front();
    pg._runtime.dirty_entry.push_back(u);
    queue.pop();

    auto [pin_u, urf] = _decode_pin(u);

    for(auto arc: pin_u->_fanout) {
      FOR_EACH_EL_RF_IF(el, vrf, arc->_delay[el][urf][vrf]){
        size_t v = _encode_pin(arc->_to, vrf);
        if(_is_fanout_inbound(pg, v, src_level+1) && !pg._has_pin[v]){
          pg._has_pin[v] = true;
          queue.push(v); 
        } 
      }
    }
  }

}

void Timer::_update_pathguide_endpoints(PathGuide& pg, std::optional<Tran> rf) {
  
  FOR_EACH_EL_RF(el, rf){ pg._runtime.endpoints[el][rf].clear(); }

  //get the ptr to the last pin
  auto to_pin = &(_pins.find(pg._through.back().name)->second);
  std::optional<Split> el = pg._el;

  if(!el && !rf){
    FOR_EACH_EL_RF(tel, trf){
      auto po = to_pin->_primary_output();    
      if(po != nullptr && po->slack(tel, trf).has_value()) {
        pg._runtime.endpoints[tel][trf].emplace_back(tel, trf, *po);
      }  
   
      for(auto test : to_pin->_tests){
        if(test != nullptr && test->slack(tel, trf).has_value()){
          pg._runtime.endpoints[tel][trf].emplace_back(tel, trf, *test);
        }
      }    
    }
  }
  else if(el && !rf) {
    FOR_EACH_RF(trf){
      auto po = to_pin->_primary_output();    
			if(po != nullptr && po->slack(*el, trf).has_value()) {
				pg._runtime.endpoints[*el][trf].emplace_back(*el, trf, *po);
			}
   
      for(auto test : to_pin->_tests){
        if(test != nullptr && test->slack(*el, trf).has_value()){
          pg._runtime.endpoints[*el][trf].emplace_back(*el, trf, *test);
        }
      }
    }
  }
  else if(!el && rf){
    FOR_EACH_EL(tel){
      auto po = to_pin->_primary_output();
			if(po != nullptr && po->slack(tel, *rf).has_value()) {
				pg._runtime.endpoints[tel][*rf].emplace_back(tel, *rf, *po);
			}
   
      for(auto test : to_pin->_tests){
        if(test != nullptr && test->slack(tel, *rf).has_value()){
          pg._runtime.endpoints[tel][*rf].emplace_back(tel, *rf, *test);
        }
      }
    }
  }
  else{
    auto po = to_pin->_primary_output();
		if(po != nullptr && po->slack(*el, *rf).has_value()) {
			pg._runtime.endpoints[*el][*rf].emplace_back(*el, *rf, *po);
		}
   
    for(auto test : to_pin->_tests){
      if(test != nullptr && test->slack(*el, *rf).has_value()){
        pg._runtime.endpoints[*el][*rf].emplace_back(*el, *rf, *test);
      }
    } 
  }
}


void Timer::_update_tail_connection(PathGuide& pg) {
  const auto& last_node = pg._through.back();
  std::queue<size_t> queue;

  for(auto idx : last_node.indices){
    if(pg._has_pin[idx]){
      queue.push(idx); //only index connected from previous thru pins are added
    }
  }

  while(!queue.empty()) {
    size_t u = queue.front();
    pg._runtime.dirty_entry.push_back(u);
    queue.pop();

    auto [pin, urf] = _decode_pin(u);

    if(!pin->_tests.empty() || pin->primary_output() != nullptr) {
      if(!pg._el){
        FOR_EACH_EL(el){
          auto po = pin->_primary_output();
          if(po != nullptr && po->slack(el, urf).has_value()) {
              pg._runtime.endpoints[el][urf].emplace_back(el, urf, *po);
          }
          for(auto test : pin->_tests){
            if(test != nullptr && test->slack(el, urf).has_value()){
              pg._runtime.endpoints[el][urf].emplace_back(el, urf, *test);
            }
          }
        }
      }
      else{
        auto po = pin->_primary_output();
        Split el = *pg._el;
        if(po != nullptr && po->slack(el, urf).has_value()) {
          pg._runtime.endpoints[el][urf].emplace_back(el, urf, *po);
        }
        for(auto test : pin->_tests){
          if(test != nullptr && test->slack(el, urf).has_value()){
            pg._runtime.endpoints[el][urf].emplace_back(el, urf, *test);
          }
        }
      }
    }

    //check all fanout edges 
    for(auto arc : pin->_fanout){
      FOR_EACH_EL_RF_IF(el, vrf, arc->_delay[el][urf][vrf]){
        size_t v = _encode_pin(arc->_to, vrf);
        //arc is in range and pin v is not visited
        if(!pg._has_pin[v].has_value()){
          pg._has_pin[v] = true;
          queue.push(v);
        }
      }
    }
  }
}

void Timer::_collect_pins(PathGuide& pg) {

  for(size_t l=0; l < pg._through.size(); l++) {
    if(l > 0) {
      _bfs_forward(pg, l-1); // BFS
    } 
    _dfs_backward(pg, l); // DFS
  }

  if(pg._runtime.last_pin_is_endpoint){
    // tau18 assumption: use MAX
    // By default, generate endpoints on both Split=min/max
    _update_pathguide_endpoints(pg, pg._through.back().rf);
  }
  else{
    //TODO: redo the arrival time forward propogation if to pin is not specified
    _update_tail_connection(pg);
  }
}



bool Timer::_is_from_inbound(const PathGuide& pg, size_t self, size_t from) const {
  // Both pins in the tail
  if(pg._has_pin[self] && pg._has_pin[from]){
    return true;
  }

  if(!pg._idx2lvl[self] || !pg._idx2lvl[from]){
    return false;
  } 

  return _is_fanin_inbound(pg, from, pg._idx2lvl[self].value());
}


bool Timer::_is_to_inbound(const PathGuide& pg, size_t self, size_t to) const {
  // Both pins in the tail
  if(pg._has_pin[self] && pg._has_pin[to]){
    return true;
  }
  if(!pg._idx2lvl[self] || !pg._idx2lvl[to]){
    return false;
  }
  
  return _is_fanout_inbound(pg, to, pg._idx2lvl[self].value());
}


// Ctor
PathGuide::PathGuide(const std::string& cmd) {
  std::string whitespace {" "};
  auto tokens = ot::split(cmd, whitespace);

  // The number of tokens must be odd & first token (command) must be report_timing
  if(!(tokens.size() & 1) || tokens[0] != "report_timing") {
    throw std::runtime_error("PathGuide only accepts report_timing");
  }

  const std::regex replace("/");

  for(unsigned i=1; i<tokens.size(); i+=2) {
    if(tokens[i] == "-through" || tokens[i] == "-from" || tokens[i] == "-to") {
      //_through.emplace_back(std::regex_replace(tokens[i+1], replace, ":"), std::nullopt);
      // Use RISE by default?  
      _through.emplace_back(std::regex_replace(tokens[i+1], replace, ":"), std::nullopt);
    }
    else if(tokens[i] == "-rise_through" || tokens[i] == "-rise_from" || tokens[i] == "-rise_to") {
      _through.emplace_back(std::regex_replace(tokens[i+1], replace, ":"), RISE);
    }
    else if(tokens[i] == "-fall_through" || tokens[i] == "-fall_from" || tokens[i] == "-fall_to") {
      _through.emplace_back(std::regex_replace(tokens[i+1], replace, ":"), FALL);
    }
    else if(tokens[i] == "-max_path") {
      _num_request_paths = std::stoi(tokens[i+1]);
    }
    else {
      OT_LOGE("Not supported option: ", tokens[i]);
      assert(false);
    }
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------







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

// TODO (Guannan)
// Function: report_timing
std::vector<Path> Timer::report_timing(PathGuide guide) {
  std::scoped_lock lock(_mutex);
  auto epts = _worst_endpoints(guide);
  return {};
}

// Function: _report_timing
// Report the top-k report_timing
std::vector<Path> Timer::_report_timing(std::vector<Endpoint*>&& epts, size_t K) {

  assert(epts.size() <= K);
  
  // No need to report anything.
  if(K == 0 || epts.empty()) {
    return {};
  }

  // No need to generate prefix tree
  if(K == 1) {
    std::vector<Path> paths;
    paths.emplace_back(epts[0]->slack(), epts[0]);
    auto sfxt = _sfxt_cache(*epts[0]);

    OT_LOGW_IF(
      std::fabs(*sfxt.slack() - paths[0].slack) > 1.0f, 
      "unstable numerics in PBA and GBA slacks: ", *sfxt.slack(), " vs ", paths[0].slack
    );

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
    [&] (PathHeap heap, Endpoint* ept) {
      _spur(*ept, K, heap);
      return heap;
    },
    [&] (Endpoint* ept) {
      PathHeap heap;
      _spur(*ept, K, heap);
      return heap;
    }
  );

  _executor.run(_taskflow).wait();
  _taskflow.clear();

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

};  // end of namespace ot. -----------------------------------------------------------------------







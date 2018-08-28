#ifndef OT_TIMER_TIMER_HPP_
#define OT_TIMER_TIMER_HPP_

#include <ot/timer/gate.hpp>
#include <ot/timer/pin.hpp>
#include <ot/timer/arc.hpp>
#include <ot/timer/net.hpp>
#include <ot/timer/test.hpp>
#include <ot/timer/clock.hpp>
#include <ot/timer/endpoint.hpp>
#include <ot/timer/path.hpp>
#include <ot/timer/sfxt.hpp>
#include <ot/timer/pfxt.hpp>
#include <ot/timer/cppr.hpp>
#include <ot/timer/scc.hpp>
#include <ot/static/logger.hpp>
#include <ot/spef/spef.hpp>
#include <ot/verilog/verilog.hpp>
#include <ot/sdc/sdc.hpp>
#include <ot/tau/tau15.hpp>

namespace ot {

// Class: Timer
class Timer {

  friend class Shell;
  
  constexpr static int FULL_TIMING   = 0x01;
  constexpr static int EPTS_UPDATED  = 0x02;
  constexpr static int AREA_UPDATED  = 0x04;
  constexpr static int POWER_UPDATED = 0x08;

  public:
    
    // Builder
    Timer& num_threads(unsigned);
    Timer& celllib(std::filesystem::path, std::optional<Split> = {});
    Timer& verilog(std::filesystem::path);
    Timer& spef(std::filesystem::path);
    Timer& sdc(std::filesystem::path);
    Timer& timing(std::filesystem::path);
    Timer& insert_net(std::string);
    Timer& insert_gate(std::string, std::string);
    Timer& repower_gate(std::string, std::string);
    Timer& remove_net(std::string);
    Timer& remove_gate(std::string);
    Timer& disconnect_pin(std::string);
    Timer& connect_pin(std::string, std::string);
    Timer& insert_primary_input(std::string);
    Timer& insert_primary_output(std::string);
    Timer& at(std::string, Split, Tran, std::optional<float>);
    Timer& rat(std::string, Split, Tran, std::optional<float>);
    Timer& slew(std::string, Split, Tran, std::optional<float>);
    Timer& load(std::string, Split, Tran, std::optional<float>);
    Timer& clock(std::string, float);
    Timer& clock(std::string, std::string, float);
    Timer& cppr(bool);
    Timer& time_unit(second_t);
    Timer& capacitance_unit(farad_t);
    Timer& resistance_unit(ohm_t);
    Timer& voltage_unit(volt_t);
    Timer& power_unit(watt_t);
    Timer& current_unit(ampere_t);

    // Action.
    void update_timing();

    std::optional<float> at(const std::string&, Split, Tran);
    std::optional<float> rat(const std::string&, Split, Tran);
    std::optional<float> slew(const std::string&, Split, Tran);
    std::optional<float> slack(const std::string&, Split, Tran);
    std::optional<float> load(const std::string&, Split, Tran);
    std::optional<float> area();
    std::optional<float> leakage_power();
    std::optional<float> tns();
    std::optional<float> wns();
    std::optional<size_t> fep();
    
    std::vector<Path> worst_paths(size_t);
    std::vector<Path> worst_paths(size_t, Split);
    std::vector<Path> worst_paths(size_t, Tran);
    std::vector<Path> worst_paths(size_t, Split, Tran);

    // Accessor
    std::string dump_graph() const;
    std::string dump_lineage() const;
    std::string dump_cell(const std::string&, Split) const;
    std::string dump_celllib(Split) const;
    std::string dump_net_load() const;
    std::string dump_pin_cap() const;
    std::string dump_at() const;
    std::string dump_rat() const;
    std::string dump_slew() const;
    std::string dump_slack() const;
    std::string dump_timer() const;
    
    inline auto num_primary_inputs() const;
    inline auto num_primary_outputs() const;
    inline auto num_pins() const;
    inline auto num_nets() const;
    inline auto num_arcs() const;
    inline auto num_gates() const;
    inline auto num_tests() const;
    inline auto num_sccs() const;

    inline auto time_unit() const;
    inline auto power_unit() const;
    inline auto resistance_unit() const;
    inline auto current_unit() const;
    inline auto voltage_unit() const;
    inline auto capacitance_unit() const;

  private:

    mutable std::shared_mutex _mutex;

    tf::Taskflow _taskflow {std::thread::hardware_concurrency()};

    int _state {0};
    
    bool _scc_analysis {false};

    std::optional<tf::Task> _lineage;
    std::optional<CpprAnalysis> _cppr_analysis;

    std::optional<second_t> _time_unit;
    std::optional<watt_t> _power_unit;
    std::optional<ohm_t> _resistance_unit;
    std::optional<farad_t> _capacitance_unit;
    std::optional<ampere_t> _current_unit;
    std::optional<volt_t> _voltage_unit;

    TimingData<Celllib, MAX_SPLIT> _celllib;

    std::unordered_map<std::string, PrimaryInput> _pis;
    std::unordered_map<std::string, PrimaryOutput> _pos; 
    std::unordered_map<std::string, Pin> _pins;
    std::unordered_map<std::string, Net> _nets;
    std::unordered_map<std::string, Gate> _gates;
    std::unordered_map<std::string, Clock> _clocks;
 
    std::list<Test> _tests;
    std::list<Arc> _arcs;
    std::list<Pin*> _frontiers;
    std::list<SCC> _sccs;

    TimingData<std::vector<Endpoint>, MAX_SPLIT, MAX_TRAN> _endpoints;
    TimingData<std::optional<float>,  MAX_SPLIT, MAX_TRAN> _wns;
    TimingData<std::optional<float>,  MAX_SPLIT, MAX_TRAN> _tns;
    TimingData<std::optional<size_t>, MAX_SPLIT, MAX_TRAN> _fep;
    
    std::optional<float> _area;
    std::optional<float> _leakage_power;

    std::deque<Pin*> _fprop_cands;
    std::deque<Pin*> _bprop_cands;

    IndexGenerator<size_t> _pin_idx_gen {0u};
    IndexGenerator<size_t> _arc_idx_gen {0u};
    
    std::vector<Pin*> _scc_cands;
    std::vector<Pin*> _idx2pin;
    std::vector<Arc*> _idx2arc;

    std::vector<Endpoint*> _worst_endpoints(size_t);
    std::vector<Endpoint*> _worst_endpoints(size_t, Split);
    std::vector<Endpoint*> _worst_endpoints(size_t, Tran);
    std::vector<Endpoint*> _worst_endpoints(size_t, Split, Tran);

    std::vector<Path> _worst_paths(std::vector<Endpoint*>&&, size_t);
    
    bool _is_redundant_timing(const Timing&, Split) const;

    void _to_time_unit(const second_t&);
    void _to_capacitance_unit(const farad_t&);
    void _to_resistance_unit(const ohm_t&);
    void _to_power_unit(const watt_t&);
    void _to_current_unit(const ampere_t&);
    void _to_voltage_unit(const volt_t&);
    void _rebase_unit(Celllib&);
    void _rebase_unit(spef::Spef&);
    void _update_timing();
    void _update_endpoints();
    void _update_area();
    void _update_power();
    void _fprop_rc_timing(Pin&);
    void _fprop_slew(Pin&);
    void _fprop_delay(Pin&);
    void _fprop_at(Pin&);
    void _fprop_test(Pin&);
    void _bprop_rat(Pin&);
    void _build_prop_cands();
    void _build_fprop_cands(Pin&);
    void _build_bprop_cands(Pin&);
    void _build_prop_tasks();
    void _clear_prop_tasks();
    void _spef(spef::Spef&);;
    void _verilog(vlog::Module&);
    void _timing(tau15::Timing&);
    void _sdc(sdc::SDC&);
    void _sdc(sdc::SetInputDelay&);
    void _sdc(sdc::SetInputTransition&);
    void _sdc(sdc::SetOutputDelay&);
    void _sdc(sdc::SetLoad&);
    void _sdc(sdc::CreateClock&);
    void _add_to_lineage(const tf::Task&);
    void _connect_pin(Pin&, Net&);
    void _disconnect_pin(Pin&);
    void _insert_frontier(Pin&);
    void _remove_frontier(Pin&);
    void _remove_scc(SCC&);
    void _clear_frontiers();
    void _insert_primary_output(const std::string&);
    void _insert_primary_input(const std::string&);
    void _insert_gate(const std::string&, const std::string&);
    void _insert_gate_arcs(Gate&);
    void _remove_gate_arcs(Gate&);
    void _repower_gate(const std::string&, const std::string&);
    void _remove_gate(Gate&);
    void _remove_net(Net&);
    void _remove_pin(Pin&);
    void _remove_arc(Arc&);
    void _remove_test(Test&);
    void _at(PrimaryInput&, Split, Tran, std::optional<float>);
    void _slew(PrimaryInput&, Split, Tran, std::optional<float>);
    void _rat(PrimaryOutput&, Split, Tran, std::optional<float>);
    void _load(PrimaryOutput&, Split, Tran, std::optional<float>);
    void _cppr(bool);
    void _spfa(SfxtCache&, std::queue<size_t>&) const;
    void _recover_prefix(Path&, const SfxtCache&, size_t) const;
    void _recover_datapath(Path&, const SfxtCache&) const;
    void _recover_datapath(Path&, const SfxtCache&, const PfxtNode*, size_t) const;
    void _enable_full_timing_update();
    void _merge_celllib(Celllib&, Split);
    void _insert_full_timing_frontiers();
    void _spur(Endpoint&, size_t, PathHeap&) const;
    void _spur(PfxtCache&, const PfxtNode&) const;

    template <typename... T, std::enable_if_t<(sizeof...(T)>1), void>* = nullptr >
    void _insert_frontier(T&&...);

    SfxtCache _sfxt_cache(const Endpoint&) const;
    SfxtCache _sfxt_cache(const PrimaryOutput&, Split, Tran) const;
    SfxtCache _sfxt_cache(const Test&, Split, Tran) const;
    CpprCache _cppr_cache(const Test&, Split, Tran) const;
    PfxtCache _pfxt_cache(const SfxtCache&) const;

    Net& _insert_net(const std::string&);
    Pin& _insert_pin(const std::string&);
    Arc& _insert_arc(Pin&, Pin&, Net&);
    Arc& _insert_arc(Pin&, Pin&, Test&);
    Arc& _insert_arc(Pin&, Pin&, TimingView);
    SCC& _insert_scc(std::vector<Pin*>&);
    Test& _insert_test(Arc&);
    Clock& _insert_clock(const std::string&, Pin&, float);
    Clock& _insert_clock(const std::string&, float);

    std::optional<float> _at(const std::string&, Split, Tran);
    std::optional<float> _rat(const std::string&, Split, Tran);
    std::optional<float> _slew(const std::string&, Split, Tran);
    std::optional<float> _slack(const std::string&, Split, Tran);
    std::optional<float> _load(const std::string&, Split, Tran);
    std::optional<float> _cppr_credit(const Test&, Split, Tran) const;
    std::optional<float> _cppr_credit(const CpprCache&, Pin&, Split, Tran) const;
    std::optional<float> _cppr_offset(const CpprCache&, Pin&, Split, Tran) const;
    std::optional<float> _sfxt_offset(const SfxtCache&, size_t) const;

    std::string _dump_graph() const;
    std::string _dump_lineage() const;
    std::string _dump_cell(const std::string&, Split) const;
    std::string _dump_celllib(Split) const;
    std::string _dump_net_load() const;
    std::string _dump_pin_cap() const;
    std::string _dump_slew() const;
    std::string _dump_slack() const;
    std::string _dump_at() const;
    std::string _dump_rat() const;
    std::string _dump_timer() const;
    std::string _dump_timing() const;
    
    size_t _max_pin_name_size() const;
    size_t _max_net_name_size() const;

    inline auto _encode_pin(Pin&, Tran) const;
    inline auto _decode_pin(size_t) const;
    inline auto _encode_arc(Arc&, Tran, Tran) const;
    inline auto _decode_arc(size_t) const;
    
    constexpr auto _has_state(int) const;
    constexpr auto _insert_state(int);
    constexpr auto _remove_state(int = 0);
};

// Procedure: _insert_frontier
template <typename... T, std::enable_if_t<(sizeof...(T)>1), void>*>
void Timer::_insert_frontier(T&&... pins) {
  (_insert_frontier(pins), ...);
}
    
// Function: num_primary_inputs
inline auto Timer::num_primary_inputs() const {
  return _pis.size();
}

// Function: num_primary_outputs
inline auto Timer::num_primary_outputs() const {
  return _pos.size();
}

// Function: num_pins
inline auto Timer::num_pins() const {
  return _pins.size();
}

// Function: num_nets
inline auto Timer::num_nets() const {
  return _nets.size();
}

// Function: num_arcs
inline auto Timer::num_arcs() const {
  return _arcs.size();
}

// Function: num_gates
inline auto Timer::num_gates() const {
  return _gates.size();
}

// Function: num_tests
inline auto Timer::num_tests() const {
  return _tests.size();
}

// Function: num_sccs
inline auto Timer::num_sccs() const {
  return _sccs.size();
}
    
// Function: time_unit
inline auto Timer::time_unit() const {
  return _time_unit;
}

// Function: power_unit
inline auto Timer::power_unit() const {
  return _power_unit;
}

// Function: resistance_unit
inline auto Timer::resistance_unit() const {
  return _resistance_unit;
}

// Function: current_unit
inline auto Timer::current_unit() const {
  return _current_unit;
}

// Function: voltage_unit
inline auto Timer::voltage_unit() const {
  return _voltage_unit;
}

// Function: capacitance_unit
inline auto Timer::capacitance_unit() const {
  return _capacitance_unit;
}

// Function: _encode_pin
inline auto Timer::_encode_pin(Pin& pin, Tran rf) const {
  return rf == RISE ? pin._idx : pin._idx + _idx2pin.size();
}

// Function: _decode_pin
inline auto Timer::_decode_pin(size_t idx) const {
  return std::make_tuple(_idx2pin[idx%_idx2pin.size()], idx<_idx2pin.size() ? RISE : FALL);
}

// Function: _encode_arc
inline auto Timer::_encode_arc(Arc& arc, Tran frf, Tran trf) const {
  if(frf == RISE) {
    return arc._idx + (trf == RISE ? 0 : _idx2arc.size());
  }
  else {
    return arc._idx + (trf == RISE ? _idx2arc.size()*2 : _idx2arc.size()*3);
  }
}

// Function: _decode_arc
inline auto Timer::_decode_arc(size_t idx) const {
  if(auto s = _idx2arc.size(); idx < s) {
    return std::make_tuple(_idx2arc[idx % s], RISE, RISE);
  }
  else if(idx < 2*s) {
    return std::make_tuple(_idx2arc[idx % s], RISE, FALL);
  }
  else if(idx < 3*s) {
    return std::make_tuple(_idx2arc[idx % s], FALL, RISE);
  }
  else {
    return std::make_tuple(_idx2arc[idx % s], FALL, FALL);
  }
}

// Function: _has_state
constexpr auto Timer::_has_state(int s) const {
  return _state & s;
}

// Procedure: _insert_state
constexpr auto Timer::_insert_state(int s) {
  _state |= s;
}
  
// Procedure: _remove_state
constexpr auto Timer::_remove_state(int s) {
  if(s == 0) _state = 0;
  else {
    _state &= ~s;
  }
}

};  // end of namespace ot ------------------------------------------------------------------------

#endif





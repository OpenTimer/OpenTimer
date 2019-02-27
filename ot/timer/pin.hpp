#ifndef OT_TIMER_PIN_HPP_
#define OT_TIMER_PIN_HPP_

#include <ot/liberty/celllib.hpp>

namespace ot {

// Forward declaration
class Timer;
class Net;
class Gate;
class RctNode;
class Arc;
class Pin;
class Test;
class SCC;

// ------------------------------------------------------------------------------------------------

// PrimayInput
class PrimaryInput {

  friend class Timer;
  friend class Pin;

  public:
    
    PrimaryInput(Pin&);

  private:

    Pin& _pin;

    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN> _slew;
    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN> _at;

    void _scale_time(float s);
    void _scale_capacitance(float s);
};

// ------------------------------------------------------------------------------------------------

// PrimaryOutput
class PrimaryOutput {

  friend class Timer;
  friend class Pin;

  public:
    
    PrimaryOutput(Pin&);

    std::optional<float> rat(Split, Tran) const;
    std::optional<float> slack(Split, Tran) const;

  private:

    Pin& _pin;
    
    TimingData<float, MAX_SPLIT, MAX_TRAN> _load {{{.0f, .0f}, {.0f, .0f}}};
    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN> _rat;

    void _scale_time(float);
    void _scale_capacitance(float);
};

// ------------------------------------------------------------------------------------------------

// Class: Pin
class Pin {
  
  // At storage
  struct At {
    Arc*  pi_arc {nullptr}; 
    Split pi_el;
    Tran  pi_rf;
    float numeric;
    At(Arc*, Split, Tran, float);
    inline operator float () const;
    inline auto pi() const;
  };
  
  // Slew storage.
  struct Slew {
    Arc*  pi_arc {nullptr};
    Split pi_el;
    Tran  pi_rf;
    float numeric;
    Slew(Arc*, Split, Tran, float);
    inline operator float () const;
    inline auto pi() const;
  };

  // Rat storage
  struct Rat {
    Arc*  pi_arc {nullptr};
    Split pi_el;
    Tran  pi_rf;
    float numeric;
    Rat(Arc*, Split, Tran, float);
    inline operator float () const;
    inline auto pi() const;
  };

  friend class Timer;
  friend class Net;
  friend class Arc;
  friend class Gate;
  friend class Test;
  friend class PrimaryOutput;
  friend class PrimaryInput;
  friend class SCC;

  friend struct Point;
  friend struct Path;

  constexpr static int FPROP_CAND       = 0x01;
  constexpr static int BPROP_CAND       = 0x02;
  constexpr static int IN_FPROP_STACK   = 0x04;
  constexpr static int IN_BPROP_STACK   = 0x08;
  constexpr static int UNLOOP_CAND      = 0x10;
  constexpr static int IN_UNLOOP_STACK  = 0x20;

  public:
    
    Pin(const std::string&);

    inline const std::string& name() const;
    inline const PrimaryInput* primary_input() const;
    inline const PrimaryOutput* primary_output() const;
    inline const Cellpin* cellpin(Split) const;
    inline const Net* net() const;
    inline const Gate* gate() const;
    
    bool is_input() const;
    bool is_output() const;
    bool is_rct_root() const;
    bool is_datapath_source() const;
    bool has_self_loop() const;

    std::optional<float> slew(Split, Tran) const;
    std::optional<float> at(Split, Tran) const;
    std::optional<float> rat(Split, Tran) const;
    std::optional<float> slack(Split, Tran) const;

    float cap(Split, Tran) const;

    inline size_t num_fanouts() const;
    inline size_t num_fanins() const;
    inline size_t idx() const;

  private:

    std::string _name;

    size_t _idx;

    Net*  _net  {nullptr};
    SCC*  _scc  {nullptr};
    Gate* _gate {nullptr};

    std::variant<PrimaryInput*, PrimaryOutput*, CellpinView> _handle;

    std::list<Arc*> _fanout;
    std::list<Arc*> _fanin;
    std::list<Test*> _tests;

    std::optional<std::list<Pin*>::iterator> _frontier_satellite;
    std::optional<std::list<Pin*>::iterator> _net_satellite;

    TimingData<std::optional<Slew>, MAX_SPLIT, MAX_TRAN> _slew;
    TimingData<std::optional<Rat >, MAX_SPLIT, MAX_TRAN> _rat;
    TimingData<std::optional<At  >, MAX_SPLIT, MAX_TRAN> _at;

    int _state {0};

    std::optional<tf::Task> _ftask;
    std::optional<tf::Task> _btask;
    
    bool _has_state(int) const;
    bool _has_no_state(int) const;
    
    void _insert_fanout(Arc&);
    void _insert_fanin(Arc&);
    void _remove_fanout(Arc&);
    void _remove_fanin(Arc&);
    void _remap_cellpin(Split, const Cellpin*);
    void _remap_cellpin(Split, const Cellpin&);
    void _reset_slew();
    void _reset_at();
    void _reset_rat();
    void _relax_slew(Arc*, Split, Tran, Split, Tran, float);
    void _relax_at(Arc*, Split, Tran, Split, Tran, float);
    void _relax_rat(Arc*, Split, Tran, Split, Tran, float);
    void _insert_state(int);
    void _remove_state(int = 0);
    
    Arc* _find_fanin(Pin&);
    Arc* _find_fanout(Pin&);
    
    inline PrimaryOutput* _primary_output();
    inline PrimaryInput* _primary_input();
    
    std::optional<float> _delta_at(Split, Tran, Split, Tran) const;
    std::optional<float> _delta_slew(Split, Tran, Split, Tran) const;
    std::optional<float> _delta_rat(Split, Tran, Split, Tran) const;
}; 

// ------------------------------------------------------------------------------------------------

// Operator
inline Pin::At::operator float () const { 
  return numeric; 
}

// Function: pi
inline auto Pin::At::pi() const {
  return std::make_tuple(pi_arc, pi_el, pi_rf);
}

// ------------------------------------------------------------------------------------------------

// Operator
inline Pin::Slew::operator float () const { 
  return numeric; 
}

// Function: pi
inline auto Pin::Slew::pi() const {
  return std::make_tuple(pi_arc, pi_el, pi_rf);
}

// ------------------------------------------------------------------------------------------------

// Operator
inline Pin::Rat::operator float () const { 
  return numeric; 
}

// Function: pi
inline auto Pin::Rat::pi() const {
  return std::make_tuple(pi_arc, pi_el, pi_rf);
}

// ------------------------------------------------------------------------------------------------

// Function: name
inline const std::string& Pin::name() const {
  return _name;
}

// Function: idx
inline size_t Pin::idx() const {
  return _idx;
}

// Function: _pi
inline PrimaryInput* Pin::_primary_input() {
  if(auto ptr = std::get_if<PrimaryInput*>(&_handle)) {
    return *ptr; 
  }
  return nullptr;
}

// Function: pi
inline const PrimaryInput* Pin::primary_input() const {
  if(auto ptr = std::get_if<PrimaryInput*>(&_handle)) {
    return *ptr; 
  }
  return nullptr;
}

// Function: _po
inline PrimaryOutput* Pin::_primary_output() {
  if(auto ptr = std::get_if<PrimaryOutput*>(&_handle)) {
    return *ptr;
  }
  return nullptr;
}

// Function: po
inline const PrimaryOutput* Pin::primary_output() const {
  if(auto ptr = std::get_if<PrimaryOutput*>(&_handle)) {
    return *ptr;
  }
  return nullptr;
}

// Function: gate
inline const Gate* Pin::gate() const {
  return _gate;
}

// Function: cellpin
inline const Cellpin* Pin::cellpin(Split m) const {
  if(auto cp = std::get_if<CellpinView>(&_handle); cp) {
    return (*cp)[m];
  } 
  return nullptr;
}

// Function: net
inline const Net* Pin::net() const {
  return _net;
}

inline size_t Pin::num_fanins() const {
  return _fanin.size();
}

inline size_t Pin::num_fanouts() const {
  return _fanout.size();
}

};  // end of namespace ot. -----------------------------------------------------------------------

#endif







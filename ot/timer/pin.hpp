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

// ------------------------------------------------------------------------------------------------

// PrimayInput
class PrimaryInput {

  friend class Timer;
  friend class Pin;

  public:
    
    PrimaryInput(Pin&);

  private:

    Pin& _pin;

    std::array<std::array<std::optional<float>, MAX_TRAN>, MAX_SPLIT> _slew;
    std::array<std::array<std::optional<float>, MAX_TRAN>, MAX_SPLIT> _at;

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
    
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> _load {.0f, .0f, .0f, .0f};
    std::array<std::array<std::optional<float>, MAX_TRAN>, MAX_SPLIT> _rat;

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
  friend class Path;
  friend class Point;

  constexpr static int FPROP_CAND     = 0x01;
  constexpr static int IN_FPROP_STACK = 0x02;
  constexpr static int BPROP_CAND     = 0x04;
  constexpr static int IN_BPROP_STACK = 0x08;

  public:
    
    Pin(const std::string&);

    inline const std::string& name() const;
    inline const PrimaryInput* pi() const;
    inline const PrimaryOutput* po() const;
    inline const Cellpin* cellpin(Split) const;
    inline const Net* net() const;

    bool is_rct_root() const;
    bool is_datapath_source() const;
    
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

    Net* _net {nullptr};

    std::variant<PrimaryInput*, PrimaryOutput*, CellpinView> _handle;

    std::list<Arc*> _fanout;
    std::list<Arc*> _fanin;
    std::list<Test*> _tests;

    std::optional<std::list<Pin*>::iterator> _frontier_satellite;
    std::optional<std::list<Pin*>::iterator> _net_satellite;

    std::array<std::array<std::optional<Slew>, MAX_TRAN>, MAX_SPLIT> _slew;
    std::array<std::array<std::optional<Rat>, MAX_TRAN>, MAX_SPLIT> _rat;
    std::array<std::array<std::optional<At>, MAX_TRAN>, MAX_SPLIT> _at;

    int _state {0};

    std::optional<tf::Taskflow::Task> _ftask;
    std::optional<tf::Taskflow::Task> _btask;
    
    bool _has_state(int) const;
    bool _has_no_state(int) const;
    
    void _insert_fanout(Arc&);
    void _insert_fanin(Arc&);
    void _remove_fanout(Arc&);
    void _remove_fanin(Arc&);
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
    
    inline PrimaryOutput* _po();
    inline PrimaryInput* _pi();
    
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
inline PrimaryInput* Pin::_pi() {
  if(auto ptr = std::get_if<PrimaryInput*>(&_handle)) {
    return *ptr; 
  }
  return nullptr;
}

// Function: pi
inline const PrimaryInput* Pin::pi() const {
  if(auto ptr = std::get_if<PrimaryInput*>(&_handle)) {
    return *ptr; 
  }
  return nullptr;
}

// Function: _po
inline PrimaryOutput* Pin::_po() {
  if(auto ptr = std::get_if<PrimaryOutput*>(&_handle)) {
    return *ptr;
  }
  return nullptr;
}

// Function: po
inline const PrimaryOutput* Pin::po() const {
  if(auto ptr = std::get_if<PrimaryOutput*>(&_handle)) {
    return *ptr;
  }
  return nullptr;
}

// Function: cellpin
inline const Cellpin* Pin::cellpin(Split m) const {
  if(auto cp = std::get_if<std::array<const Cellpin*, MAX_SPLIT>>(&_handle); cp) {
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







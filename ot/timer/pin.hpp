#ifndef OT_TIMER_PIN_HPP_
#define OT_TIMER_PIN_HPP_

#include <cstdint>
#include <limits>
#include <memory>
#include <ot/liberty/celllib.hpp>
#include <ot/timer/arc_list.hpp>

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
class Clock;

// ------------------------------------------------------------------------------------------------

// PrimayInput
class PrimaryInput {

  friend class Timer;
  friend class Pin;

  public:
    
    PrimaryInput(Pin&);

  private:

    Pin& _pin;
    struct InputDriver {
      std::string cell, from, to;
      std::array<float, 2> input_slew;
    };
    TimingData<std::optional<InputDriver>, MAX_SPLIT, MAX_TRAN> _driver;
    TimingData<float, MAX_SPLIT, MAX_TRAN> _driver_delay {{{0, 0}, {0, 0}}};

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
  class Borrowed_name {
    friend class Timer;
    Borrowed_name() = default;
  };
  
  // Keep the two-valued provenance enums compact inside each optional record.
  // Public Split/Tran types and all float values retain their representation.
  // At storage
  struct At {
    Arc*  pi_arc {nullptr}; 
    Split pi_el : 8 {MIN};
    Tran  pi_rf : 8 {RISE};
    bool  valid {false};
    float numeric {0};
    At() = default;
    At(Arc*, Split, Tran, float);
    inline operator float () const;
    inline auto pi() const;
  };
  
  // Slew storage.
  struct Slew {
    Arc*  pi_arc {nullptr};
    Split pi_el : 8 {MIN};
    Tran  pi_rf : 8 {RISE};
    bool  valid {false};
    float numeric {0};
    Slew() = default;
    Slew(Arc*, Split, Tran, float);
    inline operator float () const;
    inline auto pi() const;
  };

  // Rat storage
  struct Rat {
    Arc*  pi_arc {nullptr};
    Split pi_el : 8 {MIN};
    Tran  pi_rf : 8 {RISE};
    bool  valid {false};
    float numeric {0};
    Rat() = default;
    Rat(Arc*, Split, Tran, float);
    inline operator float () const;
    inline auto pi() const;
  };

  // Each payload has room for its engagement bit beside the provenance enums.
  // std::optional otherwise adds an aligned 8-byte flag to every 16-byte
  // record. Keep all float representations (including NaN) independent of
  // validity; a missing record is never encoded in its numeric value.
  template <typename T>
  class Propagation_optional {
    T _value;

    public:
      explicit operator bool() const { return _value.valid; }
      bool has_value() const { return _value.valid; }
      void reset() { _value.valid = false; }
      template <typename... Args>
      T& emplace(Args&&... args) {
        _value = T(std::forward<Args>(args)...);
        return _value;
      }
      T& operator*() { return _value; }
      const T& operator*() const { return _value; }
      T* operator->() { return &_value; }
      const T* operator->() const { return &_value; }
      operator std::optional<float>() const {
        return _value.valid ? std::optional<float>(_value.numeric) : std::nullopt;
      }
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
  constexpr static int SERIAL_VISITED   = 0x40;

  public:
    
    Pin(const std::string&);
    // Only Timer can create this tag. Its unordered-map key owns the name;
    // rehash preserves the key's address, and the pin dies before that key.
    Pin(const std::string&, Borrowed_name);

    inline const std::string& name() const;
    inline const PrimaryInput* primary_input() const;
    inline const PrimaryOutput* primary_output() const;
    inline const Cellpin* cellpin(Split) const;
    inline const Net* net() const;
    inline const Gate* gate() const;
    
    bool is_ideal_clock() const { return _ideal_clock; }
    bool is_input() const;
    bool is_output() const;
    bool is_rct_root() const;
    bool is_datapath_source() const;
    bool has_self_loop() const;

    std::optional<float> slew(Split, Tran) const;
    std::optional<float> at(Split, Tran) const;
    std::optional<float> rat(Split, Tran) const;
    std::optional<float> slack(Split, Tran) const;
    std::pair<float,float> power() const;

    float cap(Split, Tran) const;

    const SlewThresholds& thresholds(Split) const;
    inline size_t num_fanouts() const;
    inline size_t num_fanins() const;
    inline size_t idx() const;

  private:

    std::unique_ptr<std::string> _owned_name;
    const std::string* _name_ref;
    std::array<const SlewThresholds*, 2> _default_thresholds {nullptr, nullptr};
    uint32_t _rc_index{std::numeric_limits<uint32_t>::max()};
    bool _ideal_clock {false};
    Clock* _ideal_clock_source {nullptr};
    // Ideal clock reaching this clock sink (the fastest one when several do);
    // setup checks at the sink use its period.
    Clock* _ideal_clock_domain {nullptr};

    size_t _idx;

    Net*  _net  {nullptr};
    SCC*  _scc  {nullptr};
    Gate* _gate {nullptr};

    std::variant<PrimaryInput*, PrimaryOutput*, CellpinView> _handle;

    detail::Arc_list _fanout;
    detail::Arc_list _fanin;
    std::list<Test*> _tests;

    std::optional<std::list<Pin*>::iterator> _frontier_satellite;
    std::optional<std::list<Pin*>::iterator> _net_satellite;

    TimingData<Propagation_optional<Slew>, MAX_SPLIT, MAX_TRAN> _slew;
    TimingData<Propagation_optional<Rat>, MAX_SPLIT, MAX_TRAN> _rat;
    TimingData<Propagation_optional<At>, MAX_SPLIT, MAX_TRAN> _at;

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
  return std::make_tuple(pi_arc, static_cast<Split>(pi_el), static_cast<Tran>(pi_rf));
}

// ------------------------------------------------------------------------------------------------

// Operator
inline Pin::Slew::operator float () const { 
  return numeric; 
}

// Function: pi
inline auto Pin::Slew::pi() const {
  return std::make_tuple(pi_arc, static_cast<Split>(pi_el), static_cast<Tran>(pi_rf));
}

// ------------------------------------------------------------------------------------------------

// Operator
inline Pin::Rat::operator float () const { 
  return numeric; 
}

// Function: pi
inline auto Pin::Rat::pi() const {
  return std::make_tuple(pi_arc, static_cast<Split>(pi_el), static_cast<Tran>(pi_rf));
}

// ------------------------------------------------------------------------------------------------

// Function: name
inline const std::string& Pin::name() const {
  return *_name_ref;
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







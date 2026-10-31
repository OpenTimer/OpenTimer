#ifndef OT_TIMER_NET_HPP_
#define OT_TIMER_NET_HPP_

#include <cstdint>
#include <limits>
#include <memory>
#include <ot/spef/spef.hpp>
#include <ot/timer/pin.hpp>
#include <ot/timer/rc_ceff.hpp>
#include <ot/traits.hpp>

namespace ot {

enum class RcModel { effective_capacitance, elmore };

// In-memory ground-capacitance network. Segment endpoints index nodes in
// their authored order; no SPEF endpoint names or name-map expansion is needed.
struct IndexedRc {
  struct Node {
    std::string name;
    float capacitance;
  };
  struct Edge {
    size_t from, to;
    float resistance;
  };
  std::string name;
  std::vector<Node> nodes;
  std::vector<Edge> edges;
};

// Placement-owned RC with numeric pin identity. no_pin denotes an internal
// Steiner node. Pin indices must come from this timer's current netlist; any
// topology edit requires rebuilding the bindings. Internal nodes have no
// authored names; exports generate names only when requested.
struct BoundRc {
  static constexpr size_t no_pin = std::numeric_limits<size_t>::max();
  struct Node {
    size_t pin;
    float capacitance;
  };
  size_t driver;
  std::vector<Node> nodes;
  std::vector<IndexedRc::Edge> edges;
};

// Forward declaration
class RctEdge;
class RctNode;
class Rct;

// ------------------------------------------------------------------------------------------------

// Class: RctNode
class RctNode {

  friend class Rct;
  friend class Net;
  friend class Timer;
  
  public:

    RctNode() = default;
    RctNode(const std::string&);

    float load (Split, Tran) const;
    float cap  (Split, Tran) const;
    float slew (Split, Tran, float) const;
    float delay(Split, Tran) const;

  private:
    TimingData<float, MAX_TRAN, MAX_SPLIT> _load;
    TimingData<float, MAX_TRAN, MAX_SPLIT> _delay;
    TimingData<float, MAX_TRAN, MAX_SPLIT> _impulse;

    // Wire capacitance is shared by all corners/transitions. Pin capacitance
    // remains corner-specific and is added by cap().
    float _ncap{0.0f};
    uint32_t _index{0};
    static constexpr uint32_t no_edge = std::numeric_limits<uint32_t>::max();
    // Append-ordered links live in the edges, avoiding a vector per node.
    uint32_t _fanout{no_edge};
    uint32_t _last_fanout{no_edge};
    Pin* _pin {nullptr};

    void _scale_capacitance(float);
};

// ------------------------------------------------------------------------------------------------

// Class: RctEdge
class RctEdge {

  friend class Rct;
  friend class Net;
  friend class Timer;
  
  public:
    
    RctEdge(RctNode&, RctNode&, float);

    inline float res() const;
    inline void res(float);
  
  private:

    RctNode& _from;
    RctNode& _to;
    
    float _res {0.0f};
    uint32_t _next{std::numeric_limits<uint32_t>::max()};

    void _scale_resistance(float);
};

// Function: res
inline float RctEdge::res() const {
  return _res;
}

// Procedure: res
inline void RctEdge::res(float v) {
  _res = v; 
}

// ------------------------------------------------------------------------------------------------

// Class: Rct
class Rct {

  friend class Net;
  friend class Timer;

  public:

    void update_rc_timing();
    void insert_segment(const std::string&, const std::string&, float);
    void insert_node(const std::string&, float = 0.0f);
    void insert_edge(const std::string&, const std::string&, float);
    
    float total_ncap() const;
    float slew(const std::string&, Split, Tran, float) const;
    float delay(const std::string&, Split, Tran) const;

    inline size_t num_nodes() const;
    inline size_t num_edges() const;
    
    const RctNode* node(const std::string&) const;
    // Numeric access is available for the bound placement representation.
    const RctNode *indexed_node(size_t) const;

  private:

    RctNode* _root {nullptr};
    TimingData<RcPi, MAX_SPLIT, MAX_TRAN> _pi;

    using NamedNodes = std::unordered_map<std::string, RctNode>;
    struct BoundNodes {
      std::vector<RctNode> values;
      Pin *driver;
      BoundNodes(size_t count, Pin *pin) : values(count), driver(pin) {}
      size_t size() const { return values.size(); }
      auto begin() { return values.begin(); }
      auto end() { return values.end(); }
      auto begin() const { return values.begin(); }
      auto end() const { return values.end(); }
      auto &operator[](size_t i) { return values[i]; }
      const auto &operator[](size_t i) const { return values[i]; }
    };
    std::variant<NamedNodes, BoundNodes> _nodes;

    template <class F> void _each_node(F &&f) {
      std::visit(
          [&](auto &nodes) {
            for (auto &entry : nodes) {
              if constexpr (std::is_same_v<std::decay_t<decltype(nodes)>,
                                           NamedNodes>)
                f(entry.second);
              else
                f(entry);
            }
          },
          _nodes);
    }
    template <class F> void _each_node(F &&f) const {
      std::visit(
          [&](const auto &nodes) {
            for (const auto &entry : nodes) {
              if constexpr (std::is_same_v<std::decay_t<decltype(nodes)>,
                                           NamedNodes>)
                f(entry.second);
              else
                f(entry);
            }
          },
          _nodes);
    }
    std::vector<std::pair<const RctNode *, std::string>>
    _node_names(const std::string &) const;
    std::vector<RctEdge> _edges;

    // Moments are needed only while updating one tree. Do not retain them
    // on every node between timing evaluations.
    struct Moments {
      TimingData<float, MAX_TRAN, MAX_SPLIT> beta{};
      TimingData<float, MAX_TRAN, MAX_SPLIT> ldelay{};
    };
    RctNode &_insert_node(const std::string &);
    void _insert_edge(RctNode &, RctNode &, float);
    void _update_load(RctNode*, RctNode*);
    void _update_delay(RctNode*, RctNode*);
    void _update_ldelay(RctNode *, RctNode *, std::vector<Moments> &);
    void _update_response(RctNode *, RctNode *, std::vector<Moments> &);
    void _scale_capacitance(float);
    void _scale_resistance(float);

    RctNode* _node(const std::string&);
};

// Function: num_nodes
inline size_t Rct::num_nodes() const {
  return std::visit([](const auto &nodes) { return nodes.size(); }, _nodes);
}

// Function: num_edges
inline size_t Rct::num_edges() const {
  return _edges.size();
}

// ------------------------------------------------------------------------------------------------

// Class: Net
class Net {

  friend class Timer;
  friend class Arc;
  friend class Pin;
  
  struct EmptyRct {
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> load;
  };

  public:
    
    Net() = default;
    Net(const std::string&);

    inline const std::string& name() const;
    inline size_t num_pins() const;

    inline const Rct* rct() const;
    float load(Split s, Tran t) const { return _load(s,t); }
    std::optional<float> effective_load(Split s, Tran t) const { return _effective_cap[s][t]; }
    bool elmore_fallback(Split s, Tran t) const { return _elmore_fallback[s][t]; }

  private:

    std::string _name;

    Pin* _root {nullptr};

    std::list<Pin*> _pins;

    std::variant<EmptyRct, Rct> _rct;

    // Pending imports are immutable and short-lived. Keep their payload out
    // of every timing net once it has been materialized into an RC tree.
    std::shared_ptr<const spef::Net> _spef_net;

    bool _rc_timing_updated {false};
    RcModel _rc_model{RcModel::effective_capacitance};
    TimingData<std::optional<RcDriver>, MAX_SPLIT, MAX_TRAN> _driver;
    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN> _effective_cap;
    TimingData<bool, MAX_SPLIT, MAX_TRAN> _elmore_fallback {};

    float _load(Split, Tran) const;
    std::optional<RcGateResult> _gate_response(const Timing&, Split, Tran, Tran, float, const SlewThresholds* = nullptr) const;

    std::optional<float> _slew(Split, Tran, float, Pin&) const;
    std::optional<float> _delay(Split, Tran, Pin&) const;
    std::optional<RcResponse> _response(Split, Tran, float, Pin&) const;
    
    void _update_rc_timing();
    void _attach(spef::Net&&);
    void _attach(IndexedRc &&);
    void _attach(const BoundRc &, const std::vector<Pin *> &);
    void _make_rct();
    //void _make_rct(const spef::Net&);
    void _insert_pin(Pin&);
    void _remove_pin(Pin&);
    void _scale_capacitance(float);
    void _scale_resistance(float);
}; 

// Function: name
inline const std::string& Net::name() const {
  return _name;
}

// Function: num_pins
inline size_t Net::num_pins() const {
  return _pins.size();
}

// Function: rct
inline const Rct* Net::rct() const {
  return std::get_if<Rct>(&_rct);
}

};  // end of namespace ot. -----------------------------------------------------------------------

#endif







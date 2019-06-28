#ifndef OT_TIMER_PATH_HPP_
#define OT_TIMER_PATH_HPP_

#include <ot/headerdef.hpp>

namespace ot {

class Pin;
class Endpoint;

// ------------------------------------------------------------------------------------------------
  
// Struct: Point
struct Point {

  const Pin& pin;     // pin reference
  Tran  transition;   // rise/fall
  float at;           // arrival

  Point(const Pin&, Tran, float);
};

// ------------------------------------------------------------------------------------------------

// Struct: Path
struct Path : std::list<Point> {

  Path(float, const Endpoint*);
  Path(const Path&) = delete;
  Path(Path&&) = default; 

  Path& operator = (const Path&) = delete;
  Path& operator = (Path&&) = default;
  
  void dump(std::ostream&) const;
  void dump_tau18(std::ostream&) const;

  float slack {std::numeric_limits<float>::quiet_NaN()};
  
  const Endpoint* endpoint {nullptr};
};

// Operator << ostream
std::ostream& operator << (std::ostream&, const Path&);

// ------------------------------------------------------------------------------------------------

// Class: PathHeap
// A max-heap to maintain the top-k critical paths during the path ranking process.
class PathHeap {

  friend class Timer;
  
  // max heap
  struct PathComparator {
    bool operator () (const std::unique_ptr<Path>& a, const std::unique_ptr<Path>& b) const {
      return a->slack < b->slack;
    }
  };
  
  public:
    
    PathHeap() = default;
    PathHeap(PathHeap&&) = default;
    PathHeap(const PathHeap&) = delete;

    PathHeap& operator = (PathHeap&&) = default;
    PathHeap& operator = (const PathHeap&) = delete;
    
    inline size_t num_paths() const;
    inline size_t size() const;
    inline bool empty() const;
    
    std::vector<Path> extract();

    void push(std::unique_ptr<Path>);
    void fit(size_t);
    void pop();
    void merge_and_fit(PathHeap&&, size_t);
    void heapify();

    Path* top() const;

    std::string dump() const;

  private:

    PathComparator _comp;

    std::vector<std::unique_ptr<Path>> _paths;
};

// Function: num_paths
inline size_t PathHeap::num_paths() const {
  return _paths.size();
}

// Function: size
inline size_t PathHeap::size() const {
  return _paths.size();
}

// Function: empty
inline bool PathHeap::empty() const {
  return _paths.empty();
}

// ----------------------------------------------------------------------------

// Class: PathGuide
struct PathGuide {

  //std::optional<size_t> max_paths;
  //std::optional<size_t> num_paths_per_endpoint;
  //std::vector<std::string> from;
  //std::vector<std::string> rise_from;
  //std::vector<std::string> fall_from;
  //std::vector<std::string> to;
  //std::vector<std::string> rise_to;
  //std::vector<std::string> fall_to;
  //std::vector<std::string> through;
  //std::vector<std::string> rise_through;
  //std::vector<std::string> fall_through;

  friend class PathGuideData; 
  friend class Timer;

  PathGuide(const std::string&);
  void split(Split el) { _el = el; }

  ~PathGuide() {
    // Must clear the static variables in Dtor 
    _idx2lvl[_runtime.s].reset();
    _has_pin[_runtime.s].reset();
    for(const auto& p: _runtime.pins){
      _idx2lvl[p].reset();
    }

    const auto& last_node = _through.back();
    for(size_t idx : last_node.indices){
      _idx2lvl[idx].reset();
    }

    for(const auto& p: _runtime.dirty_entry){
      _has_pin[p].reset();
    }
  }
 
  private:

   struct GuideNode{
     std::string name;
     std::optional<Tran> rf; 
   
     std::vector<size_t> indices;
    
     // 2n=RISE, 2n+1=FALL, 2n & idices has size 2= RISE & FALL  
     size_t rank;
   
     GuideNode(const std::string& name, const std::optional<Tran>& rf):  name{name}, rf{rf} {}
   };

   // PathGuideData stores the runtime data for building paths 
   struct PathGuideRuntime {
     size_t s {0};
     size_t t {0};
   
     TimingData<std::vector<Endpoint>, MAX_SPLIT, MAX_TRAN> endpoints;
   
     std::vector<size_t> dirty_entry;
     std::vector<size_t> pins;

     bool last_pin_is_endpoint {false};
   };

   PathGuideRuntime _runtime;
   std::vector<GuideNode> _through;

   // TAU18 only requires reporting setup checks. So 
   // you should call split(MAX) when testing tau18 benchmarks
   std::optional<Split> _el; 
   std::optional<size_t> _num_request_paths {1};

   inline static std::vector<std::optional<size_t>> _idx2lvl;
   inline static std::vector<std::optional<bool>> _has_pin;

   bool _in_tail(size_t, size_t) const;

};

// Function: _in_tail
inline bool PathGuide::_in_tail(size_t pin1, size_t pin2) const {
  return _has_pin[pin1] && _has_pin[pin2];
}

};  // end of namespace ot. ---------------------------------------------------



#endif


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


// Class: PathConstraint
// A PathConstraint records the constraints specified by users 
struct PathConstraint {

  friend class Timer;
  friend class PathGuide;

  PathConstraint() = default;
  PathConstraint(const std::string&);

  void from_string(const std::string& cmd);
  void split(Split);

  void max_paths(unsigned);  //  -max_paths count

  void from(const std::string&, std::optional<Tran> = std::nullopt);
  void through(const std::string&, std::optional<Tran> = std::nullopt);
  void to(const std::string&, std::optional<Tran> = std::nullopt);

  // These two functions are to set the range of slack
  void slack_upper_bound(float);
  void slack_lower_bound(float);

  void clear();
  void reset(const std::string&);

  unsigned num_through_pins() const { return _through.size(); }

  private:

    struct GuideNode{
      std::string name;
      std::optional<Tran> rf; 
  
      std::vector<size_t> indices;
  
      // 2n=RISE, 2n+1=FALL, 2n & idices has size 2= RISE & FALL  
      size_t rank;
  
      GuideNode(const std::string& name, const std::optional<Tran>& rf):  name{name}, rf{rf} {}
    };

    std::vector<GuideNode> _through;
    // TAU18 only requires reporting setup checks. So 
    // you should call split(MAX) when testing tau18 benchmarks
    std::optional<Split> _el;  
    // By default we report the top-1 worst path
    std::optional<size_t> _num_request_paths {1};
    std::optional<float> _slack_upper_bound;
    std::optional<float> _slack_lower_bound;
};

// Class: PathGuide 
// A PathGuide is associated with a PathConstraint. PathGuide stores the runtime data related to 
// each PathConstraint
struct PathGuide {

  friend class PathSet;
  friend class Timer;

  PathGuide() = default;
  PathGuide(PathConstraint& pc) : constraint(&pc) {}

  // id is the index of the per-thread data structure that 
  // to be used in PBA
  size_t id;  
  size_t s {0};
  size_t t {0};
  std::vector<size_t> dirty_entry;
  std::vector<size_t> pins;
  void reset();

  PathConstraint* constraint {nullptr};
};

// Class: PathSet
// For each PathConstraint, OpenTimer returns a PathSet to the users. A PathSet contains 
// a set of paths that satisfy the constraint.
struct PathSet {

  PathSet() = default;
  PathSet(std::vector<Path> &&paths): paths(std::move(paths)) {}

  // Move Ctor
  PathSet(PathSet&& other) : paths(std::move(other.paths)) {
    std::swap(endpoints, other.endpoints);
  }

  // Move assignment
  PathSet& operator=(PathSet&& other) {
    paths = std::move(other.paths);
    std::swap(endpoints, other.endpoints);
    return *this;
  }

  // Disable copy constructor & assignment
  PathSet & operator=(const PathSet&) = delete;
  PathSet(const PathSet&) = delete;  

  bool empty() const { return paths.empty(); }
  unsigned size() const { return paths.size(); }
  Path& operator[] (unsigned i) { return paths[i]; }

  std::vector<Path> paths;
  TimingData<std::vector<Endpoint>, MAX_SPLIT, MAX_TRAN> endpoints;
};


};  // end of namespace ot. ---------------------------------------------------



#endif


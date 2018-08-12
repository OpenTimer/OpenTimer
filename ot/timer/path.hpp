#ifndef OT_TIMER_PATH_HPP_
#define OT_TIMER_PATH_HPP_

#include <ot/headerdef.hpp>

namespace ot {

// ------------------------------------------------------------------------------------------------
  
// Struct: Point
struct Point {

  std::string pin;    // pin name
  Tran  tran;         // rise/fall
  float at;           // delay

  Point(const std::string&, Tran, float);
};

// ------------------------------------------------------------------------------------------------

// Struct: Path
struct Path : std::list<Point> {
  
  Path() = default;
  Path(Split, float);

  Path(const Path&) = delete;
  Path(Path&&) = default; 

  Path& operator = (const Path&) = delete;
  Path& operator = (Path&&) = default;

  std::optional<Split> split;
  std::optional<float> slack;
};

// Operator << ostream
std::ostream& operator << (std::ostream&, const Path&);

// EmptyPath
inline const Path empty_path;

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

};  // end of namespace ot. -----------------------------------------------------------------------



#endif








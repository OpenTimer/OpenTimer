#ifndef OT_TIMER_PATH_HPP_
#define OT_TIMER_PATH_HPP_

#include <ot/headerdef.hpp>

namespace ot {

// ------------------------------------------------------------------------------------------------
  
// Struct: Point
struct Point {

  std::string pin;    // pin name
  Tran  tran;         // rise/fall
  float at;           // arrival time

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
    bool operator () (std::unique_ptr<Path>& a, std::unique_ptr<Path>& b) const {
      return a->slack < b->slack;
    }
  };
  
  public:

    inline size_t num_paths() const;

  private:

    PathComparator _comp;

    std::vector<std::unique_ptr<Path>> _paths;

    std::vector<Path> _extract();

    void _insert(std::unique_ptr<Path>);
    void _fit(size_t);
    void _pop();

    Path* _top() const;
};

// Function: num_paths
inline size_t PathHeap::num_paths() const {
  return _paths.size();
}


};  // end of namespace ot. -----------------------------------------------------------------------



#endif








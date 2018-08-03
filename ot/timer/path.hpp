#ifndef OT_TIMER_PATH_HPP_
#define OT_TIMER_PATH_HPP_

#include <ot/headerdef.hpp>

namespace ot {

// ------------------------------------------------------------------------------------------------
  
// Struct: Point
struct Point {

  std::string pin;    // pin name
  Split split;        // min/max (early/late)
  Tran  tran;         // rise/fall
  float at;           // arrival time
  float rat;          // required arrival time

  Point() = default;
  Point(const std::string&, Split, Tran, float, float);

  inline float slack() const;
};

// Function: slack
inline float Point::slack() const {
  return split == EARLY ? at - rat : rat - at;
}

// ------------------------------------------------------------------------------------------------

// Struct: Path
struct Path : std::list<Point> {
};

// Operator << ostream
std::ostream& operator << (std::ostream&, const Path&);


};  // end of namespace ot. -----------------------------------------------------------------------



#endif








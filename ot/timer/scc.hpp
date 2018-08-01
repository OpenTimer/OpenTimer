#ifndef OT_TIMER_SCC_HPP_
#define OT_TIMER_SCC_HPP_

#include <vector>

namespace ot {

class Pin;

// ------------------------------------------------------------------------------------------------

// Class: SCC
// strongly connected component
class SCC {

  friend class Timer;

  public:

  private:
    
    bool _valid {true};

    std::vector<Pin*> pins;


};

};  // end of namespace ot. -----------------------------------------------------------------------

#endif

#include <ot/timer/pin.hpp>
#include <ot/timer/scc.hpp>

namespace ot {

// Constructor
SCC::SCC(std::vector<Pin*>&& pins) : _pins {std::move(pins)} {
}

};  // end of namespace ot. -----------------------------------------------------------------------

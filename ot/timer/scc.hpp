#ifndef OT_TIMER_SCC_HPP_
#define OT_TIMER_SCC_HPP_

#include <vector>
#include <optional>

namespace ot {

class Pin;

// ------------------------------------------------------------------------------------------------

// Class: SCC
// strongly connected component
class SCC {

  friend class Timer;

  public:

    SCC(std::vector<Pin*>&&);

  private:
    
    std::optional<std::list<SCC>::iterator> _satellite;

    std::vector<Pin*> _pins;

    std::string _dump() const;

    bool _is_entry(const Pin&) const;
    bool _is_exit(const Pin&) const;

    void _clear();
    void _unloop(Pin&);
    void _unloop();
};

};  // end of namespace ot. -----------------------------------------------------------------------

#endif






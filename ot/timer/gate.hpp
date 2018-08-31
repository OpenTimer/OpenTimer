#ifndef OT_TIMER_GATE_HPP_
#define OT_TIMER_GATE_HPP_

#include <ot/liberty/celllib.hpp>

namespace ot {

// Forward declaration
class Pin;
class Arc;
class Test;

// ------------------------------------------------------------------------------------------------

// Class: Gate
class Gate {

  friend class Timer;

  public:
    
    Gate(const std::string&, CellView);

    inline const std::string& name() const;

    const std::string& cell_name() const;

  private:

    std::string _name;

    CellView _cell;

    std::vector<Pin*> _pins;
    std::vector<Arc*> _arcs;
    std::vector<Test*> _tests;

    //void _repower(CellView);
}; 

// Function: name
inline const std::string& Gate::name() const {
  return _name;
}

};  // end of namespace ot. -----------------------------------------------------------------------

#endif







#include <ot/timer/timer.hpp>

namespace ot {

// Function: _is_redundant_timing
bool Timer::_is_redundant_timing(const Timing& timing, Split el) const {

  switch(el) {
    case EARLY:
      if(timing.is_setup_constraint()) {
        return true; 
      }
    break;

    case LATE:
      if(timing.is_hold_constraint()) {
        return true;
      }
    break;
  }

  return false;
}

// Function: read_celllib
Timer& Timer::read_celllib(std::filesystem::path path, std::optional<Split> el) {
  
  auto lib = std::make_shared<Celllib>();
  
  std::scoped_lock lock(_mutex);
  
  // Library reader
  auto reader = _taskflow.silent_emplace([path=std::move(path), lib] () {
    lib->read(path);
  });

  // Placeholder to add_lineage
  auto modifier = _taskflow.silent_emplace([this, lib, el] () {
    if(el) {
      _merge_celllib(*lib, *el);
    }
    else {
      auto cpy = *lib;
      _merge_celllib(cpy, EARLY);
      _merge_celllib(*lib, LATE);
    }
  });

  // Reader -> modifier
  reader.precede(modifier);

  // parent -> modifier
  _add_to_lineage(modifier);
  
  return *this;
}

// Procedure: _merge_celllib
void Timer::_merge_celllib(Celllib& lib, Split el) {

  _rebase_unit(lib);
  
  // Merge the lut template
  _celllib[el].lut_templates.merge(std::move(lib.lut_templates));
  
  // Merge the cell
  _celllib[el].cells.merge(std::move(lib.cells)); 
    
  OT_LOGI(
    "added ", to_string(el), " celllib ", std::quoted(lib.name), 
    " [cells:", _celllib[el].cells.size(), ']'
  );
}

};  // end of namespace ot. -----------------------------------------------------------------------





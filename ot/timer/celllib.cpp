#include <ot/timer/timer.hpp>

namespace ot {

// Function: celllib
Timer& Timer::celllib(std::filesystem::path path, Split mode) {
  
  auto lib = std::make_shared<Celllib>();
  
  std::scoped_lock lock(_mutex);
  
  // Library reader
  auto reader = _taskflow.silent_emplace([this, path=std::move(path), mode, lib] () {
    lib->read(path, mode);
  });

  // Placeholder to add_lineage
  auto modifier = _taskflow.silent_emplace([this, lib, mode] () {
    _merge(*lib, mode);
  });

  // Reader -> modifier
  reader.precede(modifier);

  // parent -> modifier
  _add_to_lineage(modifier);
  
  return *this;
}

// Procedure: _merge
void Timer::_merge(Celllib& lib, Split el) {

  _rebase_unit(lib);

  _celllib[el].merge(lib);
    
  OT_LOGI(
    "added ", to_string(el), " celllib ", std::quoted(lib.name), 
    " [cells:", _celllib[el].cells.size(), ']'
  );
}

};  // end of namespace ot. -----------------------------------------------------------------------





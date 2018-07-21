#include <ot/timer/timer.hpp>

namespace ot {

// Function: celllib
Timer& Timer::celllib(std::filesystem::path path, Split mode) {
  
  auto lib = std::make_shared<Celllib>();
  
  std::scoped_lock lock(_mutex);
  
  // Library reader
  auto reader = _taskflow.silent_emplace([this, path=std::move(path), mode, lib] () {
    OT_LOGI("loading ", to_string(mode), " celllib ", path, " ...");
    lib->read(path, mode);
  });

  // Placeholder to add_lineage
  auto modifier = _taskflow.silent_emplace([this, lib, mode] () {

    OT_LOGI(
      "add ", to_string(mode), " celllib ", std::quoted(lib->name), 
      " [cells:", lib->cells.size(), ']'
    );
    
    _celllib[mode] = std::move(*lib);

    _rebase_unit(_celllib[mode]);
  });

  // Reader -> modifier
  reader.precede(modifier);

  // parent -> modifier
  _add_to_lineage(modifier);
  
  return *this;
}

};  // end of namespace ot. -----------------------------------------------------------------------

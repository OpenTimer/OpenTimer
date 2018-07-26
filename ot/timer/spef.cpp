#include <ot/timer/timer.hpp>

namespace ot {

// Function: spef
Timer& Timer::spef(std::filesystem::path path) {

  // Create a spefnet shared pointer
  auto spef = std::make_shared<spef::Spef>(); 
  
  std::scoped_lock lock(_mutex);

  // Reader task
  auto reader = _taskflow.silent_emplace([path=std::move(path), spef] () {
    spef->read(path);
  });
  
  // Spef update task (this has to be after reader)
  auto modifier = _taskflow.silent_emplace([this, spef] () {
    _rebase_unit(*spef);
    _spef(*spef);
    OT_LOGI("added ", spef->nets.size(), " spef nets");
  });
  
  // Build the task dependency.
  reader.precede(modifier);
  
  // parent -> modifier
  _add_to_lineage(modifier);
  
  return *this;
}

// Procedure: _spef
void Timer::_spef(spef::Spef& spef) {
  for(const auto& spefnet : spef.nets) {
    if(auto itr = _nets.find(spefnet.name); itr == _nets.end()) {
      OT_LOGW("ignore updating spef on net ", spefnet.name, " (net not found)");
      continue;
    }
    else {
      itr->second._make_rct(spefnet);
      _insert_frontier(*itr->second._root);
    }
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------

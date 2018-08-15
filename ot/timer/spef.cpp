#include <ot/timer/timer.hpp>

namespace ot {

// Function: spef
Timer& Timer::spef(std::filesystem::path path) {

  // Create a spefnet shared pointer
  auto spef = std::make_shared<spef::Spef>(); 
  
  std::scoped_lock lock(_mutex);

  // Reader task
  auto reader = _taskflow.silent_emplace([path=std::move(path), spef] () {
    if(spef->read(path); spef->error) {
      OT_LOGD("Parser-SPEF error:\n", *spef->error);
    }
    spef->expand_name();
  });
  
  // Spef update task (this has to be after reader)
  auto modifier = _taskflow.silent_emplace([this, spef] () {
    if(!spef->error) {
      _rebase_unit(*spef);
      _spef(*spef);
      OT_LOGI("added ", spef->nets.size(), " spef nets");
    }
  });
  
  // Build the task dependency.
  reader.precede(modifier);
  
  // parent -> modifier
  _add_to_lineage(modifier);
  
  return *this;
}

// Procedure: _spef
void Timer::_spef(spef::Spef& spef) {
  for(auto& spef_net : spef.nets) {
    if(auto itr = _nets.find(spef_net.name); itr == _nets.end()) {
      OT_LOGW("spef net ", spef_net.name, " not found");
      continue;
    }
    else {
      itr->second._attach(std::move(spef_net));
      _insert_frontier(*itr->second._root);
    }
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------

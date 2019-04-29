#include <ot/timer/timer.hpp>

namespace ot {

// Function: read_spef
Timer& Timer::read_spef(std::filesystem::path path) {

  // Create a spefnet shared pointer
  auto spef = std::make_shared<spef::Spef>(); 
  
  std::scoped_lock lock(_mutex);

  // Reader task
  auto parser = _taskflow.emplace([path=std::move(path), spef] () {
    OT_LOGI("loading spef ", path);
    if(spef->read(path); spef->error) {
      OT_LOGE("Parser-SPEF error:\n", *spef->error);
    }
    spef->expand_name();
  });
  
  // Spef update task (this has to be after parser)
  auto reader = _taskflow.emplace([this, spef] () {
    if(!(spef->error)) {
      _rebase_unit(*spef);
      _read_spef(*spef);
      OT_LOGI("added ", spef->nets.size(), " spef nets");
    }
  });
  
  // Build the task dependency.
  parser.precede(reader);
  
  _add_to_lineage(reader);

  return *this;
}

// Procedure: _read_spef
void Timer::_read_spef(spef::Spef& spef) {
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

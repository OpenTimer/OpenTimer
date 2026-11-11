#include <ot/timer/timer.hpp>

namespace ot {

// Function: _is_redundant_timing
bool Timer::_is_redundant_timing(const Timing& timing, Split el) const {

  if(!_preset_clear_arcs && timing.type && (*timing.type == TimingType::PRESET || *timing.type == TimingType::CLEAR)) {
    return true;
  }

  switch(el) {
    case MIN:
      if(timing.is_max_constraint()) {
        return true; 
      }
    break;

    case MAX:
      if(timing.is_min_constraint()) {
        return true;
      }
    break;
  }

  return false;
}

// Function: read_celllib
Timer& Timer::read_celllib(std::filesystem::path path, std::optional<Split> el) {
  
  auto libraries = std::make_shared<std::vector<Celllib>>();
  
  std::scoped_lock lock(_mutex);
  
  // Library parser
  auto parser = _taskflow.emplace([path=std::move(path), libraries] () {
    OT_LOGI("loading celllib ", path);
    *libraries = Celllib::read_all(path);
  });

  // Placeholder to add_lineage
  auto reader = _taskflow.emplace([this, libraries, el] () {
    for(auto& lib : *libraries) {
      // Rebase each library independently before merging. Concatenated SRAM
      // and standard-cell libraries can have different time/capacitance units.
      if(el) {
        _merge_celllib(lib, *el);
      }
      else {
        auto cpy = lib;
        _merge_celllib(cpy, MIN);
        _merge_celllib(lib, MAX);
      }
    }
  });

  // Reader -> reader
  parser.precede(reader);

  _add_to_lineage(reader);

  return *this;
}

// Procedure: _merge_celllib
void Timer::_merge_celllib(Celllib& lib, Split el) {

  _rebase_unit(lib);

  // initialize a library
  if(!_celllib[el]) {
    _celllib[el] = std::move(lib);
    for(auto& [name, pin] : _pins) pin._default_thresholds[el] = &_celllib[el]->thresholds;
    OT_LOGI(
      "added ", to_string(el), " celllib ", std::quoted(_celllib[el]->name), 
      " [cells:", _celllib[el]->cells.size(), ']'
    );
  }
  // merge the library
  else {
    // Merge the lut template
    _celllib[el]->lut_templates.merge(std::move(lib.lut_templates));
    
    // Merge the cell
    _celllib[el]->cells.merge(std::move(lib.cells)); 
    
    OT_LOGI(
      "merged with library ", std::quoted(lib.name), 
      " [cells:", _celllib[el]->cells.size(), ']'
    );
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------





#include <ot/timer/timer.hpp>

namespace ot {

// Function: _is_redundant_timing
bool Timer::_is_redundant_timing(const Timing& timing, Split el) const {

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
  
  auto lib = std::make_shared<Celllib>();
  
  std::scoped_lock lock(_mutex);
  
  // Library parser
  auto parser = _insert_builder(to_string("parse_celllib ", path), false);
  auto reader = _insert_builder(
    to_string("digest_celllib ", path, ' ', (el ? to_string(*el) : ""s)),
    true
  );

  parser.work([path=std::move(path), lib] () {
    OT_LOGI("loading celllib ", path);
    lib->read(path);
  });

  // Placeholder to add_lineage
  reader.work([this, lib, el] () {
    if(el) {
      _merge_celllib(*lib, *el);
    }
    else {
      auto cpy = *lib;
      _merge_celllib(cpy, MIN);
      _merge_celllib(*lib, MAX);
    }
  });

  // Reader -> reader
  parser.precede(reader);

  return *this;
}

// Procedure: _merge_celllib
void Timer::_merge_celllib(Celllib& lib, Split el) {

  _rebase_unit(lib);

  // initialize a library
  if(!_celllib[el]) {
    _celllib[el] = std::move(lib);
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





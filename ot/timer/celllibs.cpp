#include <ot/timer/timer.hpp>
#include <ot/headerdef.hpp>  // Contains Json type definition 

namespace ot {

// Function: read_cellibs
Timer& Timer::read_celllibs(std::filesystem::path path, std::optional<Split> el) {
  
  // std::scoped_lock lock(_mutex);
  
  OT_LOGI("loading cell libraries config ", path);
  
  // Parse JSON directly
  Json json_data;
  std::ifstream file(path);
  if (file.is_open()) {
    file >> json_data;
    if (!json_data.empty()) {
      // Process the JSON data directly
      if (json_data.contains("lib_input")) {
        for (const auto& path_str : json_data["lib_input"]) {
          if (path_str.is_string()) {
            std::filesystem::path lib_path = path_str.get<std::string>();
            OT_LOGI("Reading cell library: ", lib_path);
            read_celllib(lib_path, el);
          } else {
            OT_LOGW("Skipping non-string entry in 'lib_input' array");
          }
        }
        OT_LOGI("processed multiple cell libraries configuration");
      } else {
        OT_LOGE("JSON must contain 'lib_input' array");
      }
    }
  } else {
    OT_LOGE("Failed to open JSON file: ", path);
  }

  return *this;
}

}  // end of namespace ot

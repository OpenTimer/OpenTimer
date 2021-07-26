#include <ot/sdc/sdc.hpp>

namespace ot::sdc {

// Function: home
std::filesystem::path home() {
  
  auto path = std::filesystem::path(OT_HOME) / "ot" / "sdc";

  if(std::filesystem::exists(path)) {
    return path;
  }

  path = std::filesystem::path(OT_INSTALL_PREFIX) / "include" / "ot" / "sdc";

  if(std::filesystem::exists(path)) {
    return path;
  }
  
  return "";
}

// Procedure: read
void SDC::read(const std::filesystem::path& path) {

  OT_LOGE_RIF(!std::filesystem::exists(path), "sdc ", path, " doesn't exist");

  auto sdc_home = home();

  OT_LOGE_RIF(
    sdc_home.empty() || !std::filesystem::exists(sdc_home), 
    "sdc home ", sdc_home, " doesn't exist"
  );
  
  OT_LOGI("loading sdc ", path, " ...");

  auto sdc_path = std::filesystem::absolute(path);
  auto sdc_json = sdc_path;
  sdc_json.replace_extension(".json");
    
  if(auto cpid = ::fork(); cpid == -1) {
    OT_LOGE("can't fork sdc reader");
    return;
  }
  // child
  else if(cpid == 0) {

    if(::chdir(sdc_home.c_str()) == -1) {
      OT_LOGE("can't enter into ", sdc_home,  ": ", strerror(errno)); 
      ::exit(EXIT_FAILURE);
    }
    
    std::vector<std::string> args {
      OT_TCLSH_PATH,
      "sdc.tcl",
      sdc_path.c_str(),
      sdc_json.c_str()
    };

    ::execvp(args[0].c_str(), c_args(args).get());
    OT_LOGE("exec failed: ", strerror(errno));
    ::exit(EXIT_FAILURE);
  }
  // parent
  else {

    int s;

    do {
      int w = ::waitpid(cpid, &s, WUNTRACED | WCONTINUED);

      OT_LOGE_RIF(w == -1, "failed to reap sdc reader");

      if(WIFEXITED(s)) {
        OT_LOGE_RIF(WEXITSTATUS(s) != EXIT_SUCCESS, "sdc reader exited with failure");
      } else if (WIFSIGNALED(s)) {
        OT_LOGE("sdc reader killed by signal ", WTERMSIG(s));
        return;
      } else if (WIFSTOPPED(s)) {
        OT_LOGI("sdc reader stopped");
      } else if (WIFCONTINUED(s)) {
        OT_LOGI("sdc reader continued");
      }
    } while (!WIFEXITED(s) && !WIFSIGNALED(s));

    std::ifstream ifs(sdc_json);

    OT_LOGE_RIF(!ifs, "failed to open ", sdc_json);

    Json json;
    ifs >> json;

    for(const auto& j : json) {

      if(const auto& c = j["command"]; c == "set_input_delay") {
        commands.emplace_back(std::in_place_type_t<SetInputDelay>{}, j);  
      }
      else if(c == "set_input_transition") {
        commands.emplace_back(std::in_place_type_t<SetInputTransition>{}, j);
      }
      else if(c == "set_output_delay") {
        commands.emplace_back(std::in_place_type_t<SetOutputDelay>{}, j);
      }
      else if(c == "set_load") {
        commands.emplace_back(std::in_place_type_t<SetLoad>{}, j);
      }
      else if(c == "create_clock") {
        commands.emplace_back(std::in_place_type_t<CreateClock>{}, j);
      }
      else {
        OT_LOGE("sdc command ", c, " not supported yet");
      }
    }

    try {
      std::filesystem::remove(sdc_json);
    }
    catch(const std::exception& e) {
      OT_LOGW("can't remove ", sdc_json, ": ", e.what());
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Constructor
SetInputDelay::SetInputDelay(const Json& json) {

  for(auto itr = json.begin(); itr != json.end(); ++itr) {
    
    if(auto& key = itr.key(); key == "-clock") {
      clock = itr.value();
    }
    else if(key == "-clock_fall") {
      clock_fall.emplace();
    }
    else if(key == "-level_sensitive") {
      level_sensitive.emplace();
    }
    else if(key == "-rise") {
      rise.emplace();
    }
    else if(key == "-fall") {
      fall.emplace();
    }
    else if(key == "-min") {
      min.emplace();
    }
    else if(key == "-max") {
      max.emplace();
    }
    else if(key == "-add_delay") {
      add_delay.emplace();
    }
    else if(key == "-network_latency_included") {
      network_latency_included.emplace();
    }
    else if(key == "-source_latency_included") {
      source_latency_included.emplace();
    }
    else if(key == "delay_value") {
      delay_value = std::stof(unquoted(itr.value())); 
    }
    else if(key == "port_pin_list") {
      port_pin_list = parse_port(unquoted(itr.value()));
    }
    else if(key == "command") {
      OT_LOGE_IF(itr.value() != command, "wrong command field: ", itr.value());
    }
    else {
      OT_LOGE(command, ": ", std::quoted(key), " not supported");
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Constructor
SetInputTransition::SetInputTransition(const Json& json) {
  
  for(auto itr = json.begin(); itr != json.end(); ++itr) {
    
    if(const auto& key = itr.key(); key == "-clock") {
      clock = itr.value();
    }
    else if(key == "-clock_fall") {
      clock_fall.emplace();
    }
    else if(key == "-rise") {
      rise.emplace();
    }
    else if(key == "-fall") {
      fall.emplace();
    }
    else if(key == "-min") {
      min.emplace();
    }
    else if(key == "-max") {
      max.emplace();
    }
    else if(key == "transition") {
      transition = std::stof(unquoted(itr.value()));
    }
    else if(key == "port_list") {
      port_list = parse_port(unquoted(itr.value()));
    }
    else if(key == "command") {
      OT_LOGE_IF(itr.value() != command, "wrong command field: ", itr.value());
    }
    else {
      OT_LOGE(command, ": ", std::quoted(key), " not supported");
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Constructor
SetOutputDelay::SetOutputDelay(const Json& json) {
  
  for(auto itr = json.begin(); itr != json.end(); ++itr) {
    
    if(auto& key = itr.key(); key == "-clock") {
      clock = itr.value();
    }
    else if(key == "-clock_fall") {
      clock_fall.emplace();
    }
    else if(key == "-level_sensitive") {
      level_sensitive.emplace();
    }
    else if(key == "-rise") {
      rise.emplace();
    }
    else if(key == "-fall") {
      fall.emplace();
    }
    else if(key == "-min") {
      min.emplace();
    }
    else if(key == "-max") {
      max.emplace();
    }
    else if(key == "-add_delay") {
      add_delay.emplace();
    }
    else if(key == "-network_latency_included") {
      network_latency_included.emplace();
    }
    else if(key == "-source_latency_included") {
      source_latency_included.emplace();
    }
    else if(key == "delay_value") {
      delay_value = std::stof(unquoted(itr.value())); 
    }
    else if(key == "port_pin_list") {
      port_pin_list = parse_port(unquoted(itr.value()));
    }
    else if(key == "command") {
      OT_LOGE_IF(itr.value() != command, "wrong command field: ", itr.value());
    }
    else {
      OT_LOGE(command, ": ", std::quoted(key), " not supported");
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Constructor
SetLoad::SetLoad(const Json& json) {

  for(auto itr = json.begin(); itr != json.end(); ++itr) {
    
    if(const auto& key = itr.key(); key == "-min") {
      min.emplace();
    }
    else if(key == "-max") {
      max.emplace();
    }
    else if(key == "-subtract_pin_load") {
      subtract_pin_load.emplace();
    }
    else if(key == "-pin_load") {
      pin_load.emplace();
    }
    else if(key == "-wire_load") {
      wire_load.emplace();
    }
    else if(key == "objects") {
      objects = parse_port(unquoted(itr.value()));
    }
    else if(key == "value") {
      value = std::stof(unquoted(itr.value()));
    }
    else if(key == "command") {
      OT_LOGE_IF(itr.value() != command, "wrong command field: ", itr.value());
    }
    else {
      OT_LOGE(command, ": ", std::quoted(key), " not supported");
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Constructor
CreateClock::CreateClock(const Json& json) {

  for(auto itr = json.begin(); itr != json.end(); ++itr) {
    if(auto& key = itr.key(); key == "-period") {
      period = std::stof(unquoted(itr.value()));
    }
    else if(key == "-add") {
      add.emplace();
    }
    else if(key == "-comment") {
      comment = itr.value();
    }
    else if(key == "-name") {
      name = itr.value();
    }
    else if(key == "-waveform") {
      // TODO
    }
    else if(key == "port_pin_list") {
      port_pin_list = parse_port(unquoted(itr.value()));
    }
    else if(key == "command") {
      OT_LOGE_IF(itr.value() != command, "wrong command field: ", itr.value());
    }
    else {
      OT_LOGE(command, ": ", std::quoted(key), " not supported");
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Constructor
SetClockUncertainty::SetClockUncertainty(const Json& json) {
  for(auto itr = json.begin(); itr != json.end(); ++itr) {
    if(auto& key = itr.key(); key == "uncertainty") {
    }
    else if(key == "object_list") {
      object_list = parse_port(unquoted(itr.value()));
    }
    else if(key == "command") {
      OT_LOGE_IF(itr.value() != command, "wrong command field: ", itr.value());
    }
    else {
      OT_LOGE(command, ": ", std::quoted(key), " not supported");
    }
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------








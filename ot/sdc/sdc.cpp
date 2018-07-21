#include <ot/sdc/sdc.hpp>

namespace ot::sdc {

// operator <<
std::ostream& operator << (std::ostream& os, const SetInputDelay& obj) {

  os << obj.command;
  
  if(obj.value)                    os << ' ' << *obj.value;
  if(obj.clock)                    os << " -clock " << *obj.clock;
  if(obj.clock_fall)               os << " -clock_fall";
  if(obj.level_sensitive)          os << " -level_sensitive";
  if(obj.add_delay)                os << " -add_delay";
  if(obj.network_latency_included) os << " -network_latency_included";
  if(obj.source_latency_included)  os << " -source_latency_included";
  if(obj.min)                      os << " -min";
  if(obj.max)                      os << " -max";
  if(obj.rise)                     os << " -rise";
  if(obj.fall)                     os << " -fall";
  if(obj.port)                     os << ' ' << *obj.port;

  return os;
}

// operator <<
std::ostream& operator << (std::ostream& os, const SetInputTransition& obj) {

  os << obj.command;
  
  if(obj.value)                    os << ' ' << *obj.value;
  if(obj.clock)                    os << " -clock " << *obj.clock;
  if(obj.clock_fall)               os << " -clock_fall";
  if(obj.min)                      os << " -min";
  if(obj.max)                      os << " -max";
  if(obj.rise)                     os << " -rise";
  if(obj.fall)                     os << " -fall";
  if(obj.port)                     os << ' ' << *obj.port;

  return os;
}

// operator <<
std::ostream& operator << (std::ostream& os, const SetOutputDelay& obj) {

  os << obj.command;
  
  if(obj.value)                    os << ' ' << *obj.value;
  if(obj.clock)                    os << " -clock " << *obj.clock;
  if(obj.clock_fall)               os << " -clock_fall";
  if(obj.level_sensitive)          os << " -level_sensitive";
  if(obj.add_delay)                os << " -add_delay";
  if(obj.network_latency_included) os << " -network_latency_included";
  if(obj.source_latency_included)  os << " -source_latency_included";
  if(obj.min)                      os << " -min";
  if(obj.max)                      os << " -max";
  if(obj.rise)                     os << " -rise";
  if(obj.fall)                     os << " -fall";
  if(obj.port)                     os << ' ' << *obj.port;

  return os;
}

// Operator <<
std::ostream& operator << (std::ostream& os, const SetLoad& obj) {
  
  os << obj.command;

  if(obj.min)               os << " -min";
  if(obj.max)               os << " -max";
  if(obj.subtract_pin_load) os << " -subtract_pin_load";
  if(obj.pin_load)          os << " -pin_load";
  if(obj.wire_load)         os << " -wire_load";
  if(obj.value)             os << ' ' << *obj.value;
  if(obj.port)              os << ' ' << *obj.port;
  
  return os;
}

// Operator <<
std::ostream& operator << (std::ostream& os, const CreateClock& obj) {
  
  os << obj.command;

  if(!obj.name.empty())    os << " -name " << obj.name;
  if(obj.period)           os << " -period " << *obj.period;
  if(obj.add)              os << " -add ";

  if(obj.waveform) {
    os << " -waveform {" << (*obj.waveform)[RISE] << ' ' << (*obj.waveform)[FALL] << "}";
  }
  
  if(!obj.comment.empty()) os << " -comment " << std::quoted(obj.comment);

  if(obj.port) os << ' ' << *obj.port;

  return os;
}

// ------------------------------------------------------------------------------------------------

// Procedure: extract_set_input_delay
SetInputDelay extract_set_input_delay(token_iter_t& beg, token_iter_t end) {

  SetInputDelay obj;

  static const std::unordered_map<
    std::string, 
    void(*)(SetInputDelay&, token_iter_t&, token_iter_t)
  > handle = {
    {"-clock", [] (auto& obj, auto& beg, auto end) { 
      obj.clock = extract_clock(++beg, end);      
    }},
    {"-reference_pin", [] (auto& obj, auto& beg, auto end) {
      OT_LOGW("set_input_delay doesn't support 'reference_pin' yet");
    }},
    {"-clock_fall", [] (auto& obj, auto& beg, auto end) {
      obj.clock_fall.emplace();
    }},
    {"-level_sensitive", [] (auto& obj, auto& beg, auto end) {
      obj.level_sensitive.emplace();
    }},
    {"-rise", [] (auto& obj, auto& beg, auto end) {
      obj.rise.emplace();
    }},
    {"-fall", [] (auto& obj, auto& beg, auto end) {
      obj.fall.emplace();
    }},
    {"-min", [] (auto& obj, auto& beg, auto end) {
      obj.min.emplace();
    }},
    {"-max", [] (auto& obj, auto& beg, auto end) {
      obj.max.emplace();
    }},
    {"-add_delay", [] (auto& obj, auto& beg, auto end) {
      obj.add_delay.emplace();
      OT_LOGW("set_input_delay doesn't support 'add_delay' yet");
    }},
    {"-network_latency_included", [] (auto& obj, auto& beg, auto end) {
      obj.network_latency_included.emplace();
    }},
    {"-source_latency_included", [] (auto& obj, auto& beg, auto end) {
      obj.source_latency_included.emplace();
    }}
  };

  while(beg != end) {
    
    // command
    if(auto& token = *beg; token == "set_input_delay"){
    }
    // options
    else if(auto h = handle.find(token); h != handle.end()) {
      std::invoke(h->second, obj, beg, end);
    }
    // value
    else if(is_numeric(token)) {
      obj.value = std::stof(token);
    }
    // port
    else {
      obj.port = extract_port(beg, end);
    }

    // move forward
    ++beg;
  }
  return obj;
}

// ------------------------------------------------------------------------------------------------

// Procedure: extract_set_input_transition
SetInputTransition extract_set_input_transition(token_iter_t& beg, token_iter_t end) {

  SetInputTransition obj;

  static const std::unordered_map<
    std::string, 
    void(*)(SetInputTransition&, token_iter_t&, token_iter_t)
  > handle = {
    {"-clock", [] (auto& obj, auto& beg, auto end) { 
      obj.clock = extract_clock(++beg, end);      
    }},
    {"-clock_fall", [] (auto& obj, auto& beg, auto end) {
      obj.clock_fall.emplace();
    }},
    {"-rise", [] (auto& obj, auto& beg, auto end) {
      obj.rise.emplace();
    }},
    {"-fall", [] (auto& obj, auto& beg, auto end) {
      obj.fall.emplace();
    }},
    {"-min", [] (auto& obj, auto& beg, auto end) {
      obj.min.emplace();
    }},
    {"-max", [] (auto& obj, auto& beg, auto end) {
      obj.max.emplace();
    }}
  };

  while(beg != end) {
    
    // command
    if(auto& token = *beg; token == "set_input_transition"){
    }
    // options
    else if(auto h = handle.find(token); h != handle.end()) {
      std::invoke(h->second, obj, beg, end);
    }
    // value
    else if(is_numeric(token)) {
      obj.value = std::stof(token);
    }
    // port
    else {
      obj.port = extract_port(beg, end);
    }

    // move forward
    ++beg;
  }
  return obj;
}

// ------------------------------------------------------------------------------------------------

// Procedure: extract_set_output_delay
SetOutputDelay extract_set_output_delay(token_iter_t& beg, token_iter_t end) {

  SetOutputDelay obj;

  static const std::unordered_map<
    std::string, 
    void(*)(SetOutputDelay&, token_iter_t&, token_iter_t)
  > handle = {
    {"-clock", [] (auto& obj, auto& beg, auto end) { 
      obj.clock = extract_clock(++beg, end);      
    }},
    {"-reference_pin", [] (auto& obj, auto& beg, auto end) {
      OT_LOGW("set_output_delay doesn't support 'reference_pin' yet");
    }},
    {"-clock_fall", [] (auto& obj, auto& beg, auto end) {
      obj.clock_fall.emplace();
    }},
    {"-level_sensitive", [] (auto& obj, auto& beg, auto end) {
      obj.level_sensitive.emplace();
    }},
    {"-rise", [] (auto& obj, auto& beg, auto end) {
      obj.rise.emplace();
    }},
    {"-fall", [] (auto& obj, auto& beg, auto end) {
      obj.fall.emplace();
    }},
    {"-min", [] (auto& obj, auto& beg, auto end) {
      obj.min.emplace();
    }},
    {"-max", [] (auto& obj, auto& beg, auto end) {
      obj.max.emplace();
    }},
    {"-add_delay", [] (auto& obj, auto& beg, auto end) {
      obj.add_delay.emplace();
      OT_LOGW("set_output_delay doesn't support 'add_delay' yet");
    }},
    {"-network_latency_included", [] (auto& obj, auto& beg, auto end) {
      obj.network_latency_included.emplace();
    }},
    {"-source_latency_included", [] (auto& obj, auto& beg, auto end) {
      obj.source_latency_included.emplace();
    }}
  };

  while(beg != end) {
    
    // command
    if(auto& token = *beg; token == "set_output_delay"){
    }
    // options
    else if(auto h = handle.find(token); h != handle.end()) {
      std::invoke(h->second, obj, beg, end);
    }
    // value
    else if(is_numeric(token)) {
      obj.value = std::stof(token);
    }
    // port
    else {
      obj.port = extract_port(beg, end);
    }

    // move forward
    ++beg;
  }

  return obj;
}

// ------------------------------------------------------------------------------------------------

// Procedure: extract_set_load
SetLoad extract_set_load(token_iter_t& beg, token_iter_t end) {

  SetLoad obj;

  static const std::unordered_map<
    std::string, 
    void(*)(SetLoad&, token_iter_t&, token_iter_t)
  > handle = {
    {"-min", [] (auto& obj, auto& beg, auto end) {
      obj.min.emplace();
    }},
    {"-max", [] (auto& obj, auto& beg, auto end) {
      obj.max.emplace();
    }},
    {"-subtract_pin_load", [] (auto& obj, auto& beg, auto end) {
      obj.subtract_pin_load.emplace();
    }},
    {"-pin_load", [] (auto& obj, auto& beg, auto end) {
      obj.pin_load.emplace();
    }},
    {"-wire_load", [] (auto& obj, auto& beg, auto end) {
      obj.wire_load.emplace();
    }}
  };

  while(beg != end) {
    
    // command
    if(auto& token = *beg; token == "set_load"){
    }
    // options
    else if(auto h = handle.find(token); h != handle.end()) {
      std::invoke(h->second, obj, beg, end);
    }
    // value
    else if(is_numeric(token)) {
      obj.value = std::stof(token);
    }
    // port
    else {
      obj.port = extract_port(beg, end);
    }

    // move forward
    ++beg;
  }

  return obj;
}

// ------------------------------------------------------------------------------------------------

// Procedure: extract_create_clock
CreateClock extract_create_clock(token_iter_t& beg, token_iter_t end) {

  CreateClock obj;

  static const std::unordered_map<
    std::string, 
    void(*)(CreateClock&, token_iter_t&, token_iter_t)
  > handle = {
    {"-period", [] (auto& obj, auto& beg, auto end) {
      OT_LOGF_IF(beg == end, "clock period not found");
      obj.period = std::stof(*(++beg));
    }},
    {"-add", [] (auto& obj, auto& beg, auto end) {
      obj.add.emplace();
    }},
    {"-comment", [] (auto& obj, auto& beg, auto end) {
      OT_LOGF_IF(beg == end, "clock comment not found");
      obj.comment = std::move(*(++beg));
    }},
    {"-name", [] (auto& obj, auto& beg, auto end) {
      OT_LOGF_IF(beg == end, "clock name not found");
      auto clock = extract_clock(++beg, end);
      if(auto ptr = std::get_if<GetClocks>(&clock); ptr) {
        OT_LOGF_IF(ptr->clocks.empty(), "clock name not found");
        obj.name = std::move(ptr->clocks.front());
      }
      else {
        OT_LOGF("clock name not found");
      }
    }},
    {"-waveform", [] (auto& obj, auto& beg, auto end) {
      OT_LOGF_IF(std::distance(beg, end) < 4, "syntax error in clock waveform");
      ++beg;
      auto& waveform = obj.waveform.emplace();
      waveform[RISE] = std::stof(*(++beg));
      waveform[FALL] = std::stof(*(++beg));
      ++beg;
    }}
  };

  while(beg != end) {
    
    // command
    if(auto& token = *beg; token == "create_clock"){
    }
    // options
    else if(auto h = handle.find(token); h != handle.end()) {
      std::invoke(h->second, obj, beg, end);
    }
    // soure objects
    else {
      obj.port = extract_port(beg, end);
    }

    // move forward
    ++beg;
  }

  return obj;
}

// ------------------------------------------------------------------------------------------------

// Procedure: read
void SDC::read(const std::string& line) {

  auto tokens = tokenize(line); 

  if(tokens.empty()) return;

  static const std::unordered_map<
    std::string,
    Command(*)(token_iter_t&, token_iter_t)
  > cmd2acts = {
   {"set_input_delay", [] (auto& beg, auto end) -> Command {
     return extract_set_input_delay(beg, end);
   }},
   {"set_input_transition", [] (auto& beg, auto end) -> Command {
    return extract_set_input_transition(beg, end);
   }},
   {"set_output_delay", [] (auto& beg, auto end) -> Command {
     return extract_set_output_delay(beg, end);
   }},
   {"set_load", [] (auto& beg, auto end) -> Command {
     return extract_set_load(beg, end);
   }},
   {"create_clock", [] (auto& beg, auto end) -> Command {
     return extract_create_clock(beg, end);
   }}
  };

  if(auto itr = cmd2acts.find(tokens[0]); itr != cmd2acts.end()) {
    auto beg = tokens.begin();
    auto end = tokens.end();
    commands.push_back(std::invoke(itr->second, beg, end));
  }
  else {
    OT_LOGW("unexpected sdc command ", tokens[0]);
  }
}

// Function: read
void SDC::read(const std::filesystem::path& path) {
  if(std::ifstream ifs(path); ifs) {
    std::string line;
    while(std::getline(ifs, line)) {
      read(line); 
    }
  }
  else {
    OT_LOGW("can't open ", path);
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------








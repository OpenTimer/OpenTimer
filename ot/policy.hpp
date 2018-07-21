#ifndef OT_POLICY_HPP_
#define OT_POLICY_HPP_

namespace ot::env {

// Function: log_file
inline std::string log_file() {
  if(auto str = std::getenv("OT_LOG_FILE")) {
    return str;
  }
  else return "";
}


};  // end of namespace ot. -----------------------------------------------------------------------


#endif



#ifndef OT_EXCEPTION_HPP_
#define OT_EXCEPTION_HPP_

#include <exception>
#include <system_error>
#include <sstream>
#include <iostream>

namespace ot {

// The error category that describes the task informaction. 
struct Error : public std::error_category {

  enum Code : int {
    SUCCESS = 0,
    CELLLIB,
    TIMING,
    VERILOG,
    SPEF,
    TIMER,
    RCT,
    NET,
    ENDPOINT,
    CONNECT_PIN,
    DISCONNECT_PIN,
    INSERT_GATE,
    REMOVE_GATE,
    REPOWER_GATE
  };

  inline const char* name() const noexcept override final;
  inline static const std::error_category& get();
  
  inline std::string message(int) const override final;
};

// Function: name
inline const char* Error::name() const noexcept {
  return "OpenTimer error";
}

// Function: get 
inline const std::error_category& Error::get() {
  static Error instance;
  return instance;
}

// Function: message
inline std::string Error::message(int code) const {
  switch(code) {
    
    case SUCCESS:
      return "success";
    break;

    case CELLLIB:
      return "celllib error";
    break;

    case TIMING:
      return "timing error";
    break;

    case VERILOG:
      return "verilog parse error";
    break;

    case SPEF:
      return "spef parse error";
    break;

    case TIMER:
      return "timer error";
    break;

    case RCT:
      return "rc-tree error";
    break;

    case NET:
      return "net error";
    break;
    
    case ENDPOINT:
      return "endpoint error";
    break;

    case CONNECT_PIN:
      return "connect pin error";
    break;

    case INSERT_GATE:
      return "insert gate error";
    break;

    case REMOVE_GATE:
      return "remove gate error";
    break;
    
    case REPOWER_GATE:
      return "repower gate error";
    break;

    default:
      return "undefined";
    break;
  };
}

// Function: make_error_code
// Argument dependent lookup.
inline std::error_code make_error_code(Error::Code e) {
  return std::error_code(static_cast<int>(e), Error::get());
}

};  // end of namespace ot ------------------------------------------------------------------------


// Register for implicit conversion  
namespace std {
  template <>
  struct is_error_code_enum<ot::Error::Code> : true_type {};
};

// ------------------------------------------------------------------------------------------------

namespace ot {

template <typename... ArgsT>
void throw_se(const char* fname, const size_t line, Error::Code c, ArgsT&&... args) {
  std::ostringstream oss;
  oss << "[" << fname << ":" << line << "] ";
  (oss << ... << args);
  throw std::system_error(c, oss.str());
}

};  // end of namespace ot. -----------------------------------------------------------------------

#define OT_THROW(...) ot::throw_se(__FILE__, __LINE__, __VA_ARGS__);






#endif

#include <ot/utility/os.hpp>

namespace ot {

// Function: user_home
std::filesystem::path user_home() {

  auto home = ::getenv("HOME");

  if(home == nullptr) {
    home = ::getpwuid(::getuid())->pw_dir;
  }

  return home ? home : std::filesystem::current_path();
}

// Function: c_args
std::unique_ptr<char*, std::function<void(char**)>> c_args(
  const std::vector<std::string>& args
) {  

  std::unique_ptr<char*, std::function<void(char**)>> ptr(
    new char*[args.size()+1],
    [n=args.size()] (char** ptr) {
      for(size_t i=0; i<=n; ++i) {
        delete [] ptr[i];
      }
      delete [] ptr;
    }
  );

  for(size_t i=0; i<args.size(); ++i) {
    ptr.get()[i] = new char [args[i].length() + 1];
    std::strcpy(ptr.get()[i], args[i].c_str());
  }

  ptr.get()[args.size()] = nullptr;
  
  return ptr;
}

};  // end of namespace ot. -----------------------------------------------------------------------

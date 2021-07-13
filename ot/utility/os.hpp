#ifndef OT_UTILITY_OS_HPP_
#define OT_UTILITY_OS_HPP_

#include <vector>
#include <string>
#include <functional>
#include <unistd.h>
#include <memory>
#include <cstring>
#include <pwd.h>
#include <experimental/filesystem>

namespace ot {

namespace filesystem = std::experimental::filesystem;

// Function: user_home
filesystem::path user_home();

// Function: c_args
std::unique_ptr<char*, std::function<void(char**)>> c_args(const std::vector<std::string>&);


};  // end of namespace ot. -----------------------------------------------------------------------


#endif

#ifndef OT_HEADERDEF_HPP_
#define OT_HEADERDEF_HPP_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <sstream>
#include <mutex>
#include <shared_mutex>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <dirent.h>
#include <vector>
#include <cstring>
#include <string_view>
#include <memory>
#include <map>
#include <future>
#include <atomic>
#include <list>
#include <forward_list>
#include <unordered_map>
#include <set>
#include <stack>
#include <queue>
#include <deque>
#include <tuple>
#include <unordered_set>
#include <numeric>
#include <iterator>
#include <functional>
#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <cassert>
#include <random>
#include <regex>
#include <variant>
#include <ratio>
#include <experimental/filesystem>
#include <optional>
#include <unistd.h>
#include <sys/wait.h>

// third-party include
#include <ot/taskflow/taskflow.hpp>
#include <ot/json/json.hpp>

// Top header declaration.
#include <ot/config.hpp>


namespace std {
  namespace filesystem = experimental::filesystem;
};

namespace ot {

using Json = nlohmann::json;

// --------------------------------------------------------

using namespace std::chrono_literals;
using namespace std::literals::string_literals;

enum Split {
  EARLY = 0,
  LATE  = 1
};

enum Tran {
  RISE = 0,
  FALL = 1
};

constexpr int MAX_SPLIT = 2;
constexpr int MAX_TRAN = 2;

// Function: to_string
inline auto to_string(Split m) {
  switch(m) {
    case EARLY:
      return "early"s;
    break;

    case LATE:
      return "late"s;
    break;

    default:
      return "unknown split"s;
    break;
  };
}

// Function: to_string
inline auto to_string(Tran t) {
  switch(t) {
    case RISE:
      return "rise"s;
    break;

    case FALL:
      return "fall"s;
    break;

    default:
      return "unknown tran"s;
    break;
  };
}

// Function: to_string
inline auto to_string(Tran from, Tran to) {
  return to_string(from) + "->" + to_string(to);
}

constexpr std::initializer_list<Split> SPLIT = {EARLY, LATE};

constexpr std::initializer_list<Tran> TRAN = {RISE, FALL};

constexpr std::initializer_list<std::pair<Split, Tran>> SPLIT_TRAN = { 
  {EARLY, RISE},
  {EARLY, FALL},
  {LATE,  RISE},
  {LATE,  FALL} 
};

constexpr std::initializer_list<std::pair<Tran, Tran>> TRANX2 = {
  {RISE, RISE},
  {RISE, FALL},
  {FALL, RISE},
  {FALL, FALL}
};

constexpr std::initializer_list<std::tuple<Split, Tran, Tran>> SPLIT_TRANx2 = {
  {EARLY, RISE, RISE},
  {EARLY, RISE, FALL},
  {EARLY, FALL, RISE},
  {EARLY, FALL, FALL},
  {LATE, RISE, RISE},
  {LATE, RISE, FALL},
  {LATE, FALL, RISE},
  {LATE, FALL, FALL}
};

#define FOR_EACH_EL(el) for(auto el : SPLIT)
#define FOR_EACH_RF(rf) for(auto rf : TRAN)
#define FOR_EACH_RF_RF(irf, orf) for(auto [irf, orf] : TRANX2)
#define FOR_EACH_EL_RF(el, rf) for(auto [el, rf] : SPLIT_TRAN)
#define FOR_EACH_EL_RF_RF(el, rf1, rf2) for(auto [el, rf1, rf2] : SPLIT_TRANx2)

#define FOR_EACH_EL_IF(el, c) for(auto el : SPLIT) if(c)
#define FOR_EACH_RF_IF(rf, c) for(auto rf : TRAN) if(c)
#define FOR_EACH_RF_RF_IF(irf, orf, c) for(auto [irf, orf] : TRANX2) if(c)
#define FOR_EACH_EL_RF_IF(el, rf, c) for(auto [el, rf] : SPLIT_TRAN) if(c)
#define FOR_EACH_EL_RF_RF_IF(el, rf1, rf2, c) for(auto [el, rf1, rf2] : SPLIT_TRANx2) if(c)

// ------------------------------------------------------------------------------------------------  

// Class: SplitView
template <typename T>
class SplitView {

  public:
    
    SplitView(const SplitView&) = default;
    SplitView(const T&, const T&);
    
    inline const T& get(Split) const;
    inline void set(Split, const T&);

    const T& operator [] (Split) const;

  private:

    std::array<std::reference_wrapper<const T>, MAX_SPLIT> _view;
};

template <typename T>
SplitView<T>::SplitView(const T& e, const T& l) : _view {e, l} {
}

template <typename T>
inline const T& SplitView<T>::get(Split m) const {
  return _view[m];
}

template <typename T>
inline const T& SplitView<T>::operator [] (Split m) const {
  return _view[m];
}

template <typename T>
inline void SplitView<T>::set(Split el, const T& t) {
  _view[el] = t;
}



};  // End of namespace ot. -----------------------------------------------------------------------


#endif


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
#include <ratio>
#include <experimental/filesystem>
#include <optional>
#include <unistd.h>
#include <sys/wait.h>

// Clang mis-interprets variant's get as a non-friend of variant and cannot
// get compiled correctly. We use the patch: 
// https://gcc.gnu.org/viewcvs/gcc?view=revision&revision=258854
// to get rid of this.
#if defined(__clang__)
  #include <ot/patch/clang_variant.hpp>
#else
  #include <variant>
#endif

// third-party include
#include <ot/taskflow/taskflow.hpp>
#include <ot/json/json.hpp>
#include <ot/parser-spef/parser-spef.hpp>
#include <ot/unit/units.hpp>

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
  MIN = 0,
  MAX = 1
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
    case MIN:
      return "min"s;
    break;

    case MAX:
      return "max"s;
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

inline std::ostream& operator << (std::ostream& os, Split t) {
  switch(t) {
    case MIN:
      os << "min";
    break;

    case MAX:
      os << "max";
    break;

    default:
      os << "unknown split";
    break;
  };
  return os;
}

inline std::ostream& operator << (std::ostream& os, Tran t) {
  switch(t) {
    case RISE:
      os << "rise";
    break;

    case FALL:
      os << "fall";
    break;

    default:
      os << "unknown tran";
    break;
  };
  return os;
}

// Function: to_string
inline auto to_string(Tran from, Tran to) {
  return to_string(from) + "->" + to_string(to);
}

// ------------------------------------------------------------------------------------------------  

constexpr std::initializer_list<Split> SPLIT = {MIN, MAX};

constexpr std::initializer_list<Tran> TRAN = {RISE, FALL};

constexpr std::initializer_list<std::pair<Split, Tran>> SPLIT_TRAN = { 
  {MIN, RISE},
  {MIN, FALL},
  {MAX, RISE},
  {MAX, FALL} 
};

constexpr std::initializer_list<std::pair<Tran, Tran>> TRANX2 = {
  {RISE, RISE},
  {RISE, FALL},
  {FALL, RISE},
  {FALL, FALL}
};

constexpr std::initializer_list<std::tuple<Split, Tran, Tran>> SPLIT_TRANx2 = {
  {MIN, RISE, RISE},
  {MIN, RISE, FALL},
  {MIN, FALL, RISE},
  {MIN, FALL, FALL},
  {MAX, RISE, RISE},
  {MAX, RISE, FALL},
  {MAX, FALL, RISE},
  {MAX, FALL, FALL}
};

// ------------------------------------------------------------------------------------------------  

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

#define FOR_EACH(i, C) for(auto& i : C)
#define FOR_EACH_IF(i, C, s) for(auto& i : C) if(s)

// ------------------------------------------------------------------------------------------------  

// TimingData
template <typename, size_t ...>
struct TimingDataHelper;

template <typename T, size_t D0, size_t ... Ds>
struct TimingDataHelper<T, D0, Ds...> { 
  using type = std::array<typename TimingDataHelper<T, Ds...>::type, D0>; 
};

template <typename T>
struct TimingDataHelper<T>{ 
  using type = T; 
};

template <typename T, size_t ... Ds>
using TimingData = typename TimingDataHelper<T, Ds...>::type;




};  // End of namespace ot. -----------------------------------------------------------------------


#endif


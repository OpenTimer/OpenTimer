#ifndef OT_TRAITS_HPP_
#define OT_TRAITS_HPP_

#include <functional>
#include <tuple>
#include <utility>
#include <string>
#include <vector>
#include <list>
#include <forward_list>
#include <queue>
#include <stack>
#include <type_traits>
#include <memory>
#include <map>
#include <set>
#include <chrono>
#include <any>
#include <unordered_map>
#include <unordered_set>

//-------------------------------------------------------------------------------------------------
// Functors.
//-------------------------------------------------------------------------------------------------

// Overloadded.
template <typename... Ts>
struct Functors : Ts... { 
  using Ts::operator()... ;
};

template <typename... Ts>
Functors(Ts...) -> Functors<Ts...>;

//-------------------------------------------------------------------------------------------------
// Optional
//-------------------------------------------------------------------------------------------------

// Composition of optional object.
template <typename T>
struct add_optionality {
  using type = std::optional<T>;
};

template <typename T>
struct add_optionality < std::optional<T> > {
  using type = std::optional<T>;
};

template <>
struct add_optionality <void> {
  using type = void;
};

template <typename T>
using add_optionality_t = typename add_optionality<T>::type;

template <typename T, typename L>
auto operator | (std::optional<T>& opt, L&& lambda) -> add_optionality_t<decltype(lambda(*opt))> {

	if constexpr (std::is_same_v<void, decltype(lambda(*opt))>) {
    if(opt) {
      lambda(*opt);
    }
  }
  else {
    if(opt) {
      return lambda(*opt);
    }
    else {
      return {};
    }
  }
}


#endif

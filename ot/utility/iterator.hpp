#ifndef OT_UTILITY_ITERATOR_HPP_
#define OT_UTILITY_ITERATOR_HPP_

#include <tuple>
#include <type_traits>
#include <vector>
#include <iterator>

namespace ot::iter {

template <typename... T>
class ZipHelper {

  public:

  class Iterator : std::iterator<
    std::forward_iterator_tag,
    std::tuple<decltype(*std::declval<T>().begin())...>
  > {

    std::tuple<decltype(std::declval<T>().begin())...> _iters;

    template <std::size_t... I>
    auto deref(std::index_sequence<I...>) const {
      return typename Iterator::value_type{*std::get<I>(_iters)...};
      //return forward_as_tuple(*std::get<I>(_iters)...);
    }

    template <std::size_t... I>
    void increment(std::index_sequence<I...>) {
      //auto l = {(++std::get<I>(_iters), 0)...};
      (++std::get<I>(_iters), ...);
    }

    public:

    explicit Iterator(auto&&... iters) : _iters {std::forward<decltype(iters)>(iters)...} {
    }

    Iterator& operator++() {
      increment(std::index_sequence_for<T...>{});
      return *this;
    }

    bool operator!=(const Iterator& other) const {
      return _iters != other._iters;
    }

    auto operator*() const { return deref(std::index_sequence_for<T...>{}); }

  };

    ZipHelper(T&&... seqs) : 
      _begin {std::make_tuple(std::begin(seqs)...)},
      _end   {std::make_tuple(std::end(seqs)...)} {
    }

    auto begin() const { return _begin; }
    auto end()   const { return _end;   }

  private:
    
    Iterator _begin;
    Iterator _end;

};

template <typename... T>
auto zip(T&&... seqs) {
  return ZipHelper<T...>(std::forward<T>(seqs)...);
}

};  // end of namespace ot::iter. -----------------------------------------------------------------

#endif







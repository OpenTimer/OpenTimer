#ifndef OT_UTILITY_UTILITY_HPP_
#define OT_UTILITY_UTILITY_HPP_

#include <ot/utility/lambda.hpp>
#include <ot/utility/logger.hpp>
#include <ot/utility/tokenizer.hpp>
#include <ot/utility/index.hpp>
#include <ot/utility/iterator.hpp>
#include <ot/utility/os.hpp>
#include <ot/utility/scope_guard.hpp>
#include <ot/utility/CLI11.hpp>

// Miscellaneous
namespace ot {

// Resize a container to fit a given size
template <typename T>
void resize_to_fit(size_t N, T& v) {
  auto sz = v.size();
  for(; sz < N; sz = (sz==0) ? 32 : (sz << 1));
  v.resize(sz);
}

// Resize containers to fit a given size
template <typename... T, std::enable_if_t<(sizeof...(T)>1), void>* = nullptr>
void resize_to_fit(size_t N, T&...vs) {
  (resize_to_fit(N, vs), ...);
}

};

#endif

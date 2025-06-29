// Copyright (c) 2014-2023 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_INTERNAL_LIST_TAIL_HPP
#define TAO_PEGTL_INTERNAL_LIST_TAIL_HPP

#include "seq.hpp"
#include "star_partial.hpp"

#include "../config.hpp"

namespace TAO_PEGTL_NAMESPACE::internal
{
   template< typename Rule, typename Sep >
   using list_tail = seq< Rule, star_partial< Sep, Rule > >;

}  // namespace TAO_PEGTL_NAMESPACE::internal

#endif

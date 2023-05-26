// Copyright (c) 2017-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_INTERNAL_HAS_APPLY0_HPP
#define TAO_PEGTL_INTERNAL_HAS_APPLY0_HPP

#include <utility>

namespace tao::pegtl::internal
{
   template< typename, typename, template< typename... > class, typename... >
   inline constexpr bool has_apply0 = false;

   template< typename C, template< typename... > class Action, typename... S >
   inline constexpr bool has_apply0< C, decltype( C::template apply0< Action >( std::declval< S >()... ) ), Action, S... > = true;

}  // namespace tao::pegtl::internal

#endif

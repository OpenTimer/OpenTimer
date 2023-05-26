// Copyright (c) 2014-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_INTERNAL_EOLF_HPP
#define TAO_PEGTL_INTERNAL_EOLF_HPP

#include "enable_control.hpp"

#include "../type_list.hpp"

namespace tao::pegtl::internal
{
   struct eolf
   {
      using rule_t = eolf;
      using subs_t = empty_list;

      template< typename ParseInput >
      [[nodiscard]] static bool match( ParseInput& in ) noexcept( noexcept( ParseInput::eol_t::eol_match( in ) ) )
      {
         const auto p = ParseInput::eol_t::eol_match( in );
         return p.data || ( p.size == 0 );
      }
   };

   template<>
   inline constexpr bool enable_control< eolf > = false;

}  // namespace tao::pegtl::internal

#endif

// Copyright (c) 2017-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_INTERNAL_APPLY_HPP
#define TAO_PEGTL_INTERNAL_APPLY_HPP

#include "apply_single.hpp"
#include "enable_control.hpp"

#include "../apply_mode.hpp"
#include "../rewind_mode.hpp"
#include "../type_list.hpp"

namespace tao::pegtl::internal
{
   template< typename... Actions >
   struct apply
   {
      using rule_t = apply;
      using subs_t = empty_list;

      template< apply_mode A,
                rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename ParseInput,
                typename... States >
      [[nodiscard]] static bool match( [[maybe_unused]] ParseInput& in, [[maybe_unused]] States&&... st )
      {
         if constexpr( ( A == apply_mode::action ) && ( sizeof...( Actions ) > 0 ) ) {
            using action_t = typename ParseInput::action_t;
            const action_t i2( in.frobnicator(), in );  // No data -- range is from begin to begin.
            return ( apply_single< Actions >::match( i2, st... ) && ... );
         }
         else {
#if defined( _MSC_VER )
            ( (void)st, ... );
#endif
            return true;
         }
      }
   };

   template< typename... Actions >
   inline constexpr bool enable_control< apply< Actions... > > = false;

}  // namespace tao::pegtl::internal

#endif

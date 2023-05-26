// Copyright (c) 2019-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_CHANGE_STATE_HPP
#define TAO_PEGTL_CHANGE_STATE_HPP

#include <type_traits>

#include "apply_mode.hpp"
#include "match.hpp"
#include "nothing.hpp"
#include "rewind_mode.hpp"

#include "internal/dependent_false.hpp"

namespace tao::pegtl
{
   template< typename NewState >
   struct change_state
      : maybe_nothing
   {
      template< typename Rule,
                apply_mode A,
                rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename ParseInput,
                typename... States >
      [[nodiscard]] static bool match( ParseInput& in, States&&... st )
      {
         if constexpr( std::is_constructible_v< NewState, const ParseInput&, States... > ) {
            NewState s( static_cast< const ParseInput& >( in ), st... );
            if( tao::pegtl::match< Rule, A, M, Action, Control >( in, s ) ) {
               if constexpr( A == apply_mode::action ) {
                  Action< Rule >::success( static_cast< const ParseInput& >( in ), s, st... );
               }
               return true;
            }
            return false;
         }
         else if constexpr( std::is_default_constructible_v< NewState > ) {
            NewState s;
            if( tao::pegtl::match< Rule, A, M, Action, Control >( in, s ) ) {
               if constexpr( A == apply_mode::action ) {
                  Action< Rule >::success( static_cast< const ParseInput& >( in ), s, st... );
               }
               return true;
            }
            return false;
         }
         else {
            static_assert( internal::dependent_false< NewState >, "unable to instantiate new state" );
         }
      }

      template< typename ParseInput,
                typename... States >
      static void success( const ParseInput& in, NewState& s, States&&... st ) noexcept( noexcept( s.success( in, st... ) ) )
      {
         s.success( in, st... );
      }
   };

}  // namespace tao::pegtl

#endif

// Copyright (c) 2014-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_NORMAL_HPP
#define TAO_PEGTL_NORMAL_HPP

#include <string>
#include <type_traits>
#include <utility>

#include "apply_mode.hpp"
#include "match.hpp"
#include "parse_error.hpp"
#include "rewind_mode.hpp"

#include "internal/enable_control.hpp"
#include "internal/has_error_message.hpp"
#include "internal/has_match.hpp"

#if defined( __cpp_exceptions )
#include "demangle.hpp"
#else
#include "internal/dependent_false.hpp"
#include <exception>
#endif

namespace tao::pegtl
{
   template< typename Rule >
   struct normal
   {
      static constexpr bool enable = internal::enable_control< Rule >;

      template< typename ParseInput, typename... States >
      static void start( const ParseInput& /*unused*/, States&&... /*unused*/ ) noexcept
      {}

      template< typename ParseInput, typename... States >
      static void success( const ParseInput& /*unused*/, States&&... /*unused*/ ) noexcept
      {}

      template< typename ParseInput, typename... States >
      static void failure( const ParseInput& /*unused*/, States&&... /*unused*/ ) noexcept
      {}

      template< typename ParseInput, typename... States >
      [[noreturn]] static void raise( const ParseInput& in, States&&... /*unused*/ )
      {
#if defined( __cpp_exceptions )
         if constexpr( internal::has_error_message< Rule > ) {
            throw parse_error( Rule::error_message, in );
         }
         else {
            throw parse_error( "parse error matching " + std::string( demangle< Rule >() ), in );
         }
#else
         static_assert( internal::dependent_false< Rule >, "exception support required for normal< Rule >::raise()" );
         (void)in;
         std::terminate();
#endif
      }

      template< template< typename... > class Action,
                typename Frobnicator,
                typename ParseInput,
                typename... States >
      static auto apply( const Frobnicator& begin, const ParseInput& in, States&&... st ) noexcept( noexcept( Action< Rule >::apply( std::declval< const typename ParseInput::action_t& >(), st... ) ) )
         -> decltype( Action< Rule >::apply( std::declval< const typename ParseInput::action_t& >(), st... ) )
      {
         const typename ParseInput::action_t action_input( begin, in );
         return Action< Rule >::apply( action_input, st... );
      }

      template< template< typename... > class Action,
                typename ParseInput,
                typename... States >
      static auto apply0( const ParseInput& /*unused*/, States&&... st ) noexcept( noexcept( Action< Rule >::apply0( st... ) ) )
         -> decltype( Action< Rule >::apply0( st... ) )
      {
         return Action< Rule >::apply0( st... );
      }

      template< apply_mode A,
                rewind_mode M,
                template< typename... >
                class Action,
                template< typename... >
                class Control,
                typename ParseInput,
                typename... States >
      [[nodiscard]] static bool match( ParseInput& in, States&&... st )
      {
         if constexpr( internal::has_match< bool, Rule, A, M, Action, Control, ParseInput, States... > ) {
            return Action< Rule >::template match< Rule, A, M, Action, Control >( in, st... );
         }
         else {
            return tao::pegtl::match< Rule, A, M, Action, Control >( in, st... );
         }
      }
   };

}  // namespace tao::pegtl

#endif

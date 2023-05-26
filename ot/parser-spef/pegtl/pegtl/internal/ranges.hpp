// Copyright (c) 2014-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_INTERNAL_RANGES_HPP
#define TAO_PEGTL_INTERNAL_RANGES_HPP

#include <utility>

#include "bump_help.hpp"
#include "enable_control.hpp"
#include "failure.hpp"
#include "one.hpp"
#include "range.hpp"

#include "../type_list.hpp"

namespace tao::pegtl::internal
{
   template< typename Char, Char Lo, Char Hi >
   constexpr bool validate_range( Char c ) noexcept
   {
      static_assert( Lo <= Hi, "invalid range" );
      return ( Lo <= c ) && ( c <= Hi );
   }

   template< typename Peek, typename Peek::data_t... Cs >
   struct ranges
   {
      using peek_t = Peek;
      using data_t = typename Peek::data_t;

      using rule_t = ranges;
      using subs_t = empty_list;

      template< std::size_t... Is >
      [[nodiscard]] static constexpr bool test_impl( std::index_sequence< Is... > /*unused*/, const data_t c ) noexcept
      {
         constexpr const data_t cs[] = { Cs... };
         if constexpr( sizeof...( Cs ) % 2 == 0 ) {
            return ( validate_range< data_t, cs[ 2 * Is ], cs[ 2 * Is + 1 ] >( c ) || ... );
         }
         else {
            return ( validate_range< data_t, cs[ 2 * Is ], cs[ 2 * Is + 1 ] >( c ) || ... ) || ( c == cs[ sizeof...( Cs ) - 1 ] );
         }
      }

      [[nodiscard]] static constexpr bool test_one( const data_t c ) noexcept
      {
         return test_impl( std::make_index_sequence< sizeof...( Cs ) / 2 >(), c );
      }

      [[nodiscard]] static constexpr bool test_any( const data_t c ) noexcept
      {
         return test_impl( std::make_index_sequence< sizeof...( Cs ) / 2 >(), c );
      }

      template< typename ParseInput >
      [[nodiscard]] static bool match( ParseInput& in ) noexcept( noexcept( Peek::peek( in ) ) )
      {
         if( const auto t = Peek::peek( in ) ) {
            if( test_one( t.data ) ) {
               bump_help< ranges >( in, t.size );
               return true;
            }
         }
         return false;
      }
   };

   template< typename Peek, typename Peek::data_t Lo, typename Peek::data_t Hi >
   struct ranges< Peek, Lo, Hi >
      : range< result_on_found::success, Peek, Lo, Hi >
   {};

   template< typename Peek, typename Peek::data_t C >
   struct ranges< Peek, C >
      : one< result_on_found::success, Peek, C >
   {};

   template< typename Peek >
   struct ranges< Peek >
      : failure
   {};

   template< typename Peek, typename Peek::data_t... Cs >
   inline constexpr bool enable_control< ranges< Peek, Cs... > > = false;

}  // namespace tao::pegtl::internal

#endif

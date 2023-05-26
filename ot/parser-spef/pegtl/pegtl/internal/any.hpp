// Copyright (c) 2014-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_INTERNAL_ANY_HPP
#define TAO_PEGTL_INTERNAL_ANY_HPP

#include "enable_control.hpp"
#include "peek_char.hpp"

#include "../type_list.hpp"

namespace tao::pegtl::internal
{
   template< typename Peek >
   struct any;

   template<>
   struct any< peek_char >
   {
      using peek_t = peek_char;
      using data_t = char;

      using rule_t = any;
      using subs_t = empty_list;

      [[nodiscard]] static bool test_one( const char /*unused*/ ) noexcept
      {
         return true;
      }

      [[nodiscard]] static bool test_any( const char /*unused*/ ) noexcept
      {
         return true;
      }

      template< typename ParseInput >
      [[nodiscard]] static bool match( ParseInput& in ) noexcept( noexcept( in.empty() ) )
      {
         if( !in.empty() ) {
            in.bump();
            return true;
         }
         return false;
      }
   };

   template< typename Peek >
   struct any
   {
      using peek_t = Peek;
      using data_t = typename Peek::data_t;

      using rule_t = any;
      using subs_t = empty_list;

      [[nodiscard]] static bool test_one( const data_t /*unused*/ ) noexcept
      {
         return true;
      }

      [[nodiscard]] static bool test_any( const data_t /*unused*/ ) noexcept
      {
         return true;
      }

      template< typename ParseInput >
      [[nodiscard]] static bool match( ParseInput& in ) noexcept( noexcept( Peek::peek( in ) ) )
      {
         if( const auto t = Peek::peek( in ) ) {
            in.bump( t.size );
            return true;
         }
         return false;
      }
   };

   template< typename Peek >
   inline constexpr bool enable_control< any< Peek > > = false;

}  // namespace tao::pegtl::internal

#endif

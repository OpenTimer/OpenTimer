// Copyright (c) 2014-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_CONTRIB_INTERNAL_PEEK_UTF16_HPP
#define TAO_PEGTL_CONTRIB_INTERNAL_PEEK_UTF16_HPP

#include <type_traits>

#include "../../internal/data_and_size.hpp"

#include "read_uint.hpp"

namespace tao::pegtl::internal
{
   template< typename R >
   struct peek_utf16_impl
   {
      using data_t = char32_t;
      using pair_t = data_and_size< char32_t >;

      using short_t = std::make_unsigned< char16_t >::type;

      static_assert( sizeof( short_t ) == 2 );
      static_assert( sizeof( char16_t ) == 2 );

      template< typename ParseInput >
      [[nodiscard]] static pair_t peek( ParseInput& in ) noexcept( noexcept( in.size( 4 ) ) )
      {
         if( in.size( 2 ) < 2 ) {
            return { 0, 0 };
         }
         const char32_t t = R::read( in.current() );
         if( ( t < 0xd800 ) || ( t > 0xdfff ) ) {
            return { t, 2 };
         }
         if( ( t >= 0xdc00 ) || ( in.size( 4 ) < 4 ) ) {
            return { 0, 0 };
         }
         const char32_t u = R::read( in.current() + 2 );
         if( ( u >= 0xdc00 ) && ( u <= 0xdfff ) ) {
            const auto cp = ( ( ( t & 0x03ff ) << 10 ) | ( u & 0x03ff ) ) + 0x10000;
            return { cp, 4 };
         }
         return { 0, 0 };
      }
   };

   using peek_utf16_be = peek_utf16_impl< read_uint16_be >;
   using peek_utf16_le = peek_utf16_impl< read_uint16_le >;

}  // namespace tao::pegtl::internal

#endif

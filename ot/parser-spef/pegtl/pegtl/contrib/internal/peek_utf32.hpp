// Copyright (c) 2014-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_CONTRIB_INTERNAL_PEEK_UTF32_HPP
#define TAO_PEGTL_CONTRIB_INTERNAL_PEEK_UTF32_HPP

#include <cstddef>

#include "../../internal/data_and_size.hpp"

#include "read_uint.hpp"

namespace tao::pegtl::internal
{
   template< typename R >
   struct peek_utf32_impl
   {
      using data_t = char32_t;
      using pair_t = data_and_size< char32_t >;

      static_assert( sizeof( char32_t ) == 4 );

      template< typename ParseInput >
      [[nodiscard]] static pair_t peek( ParseInput& in ) noexcept( noexcept( in.size( 4 ) ) )
      {
         if( in.size( 4 ) < 4 ) {
            return { 0, 0 };
         }
         const char32_t t = R::read( in.current() );
         if( ( t <= 0x10ffff ) && !( t >= 0xd800 && t <= 0xdfff ) ) {
            return { t, 4 };
         }
         return { 0, 0 };
      }
   };

   using peek_utf32_be = peek_utf32_impl< read_uint32_be >;
   using peek_utf32_le = peek_utf32_impl< read_uint32_le >;

}  // namespace tao::pegtl::internal

#endif

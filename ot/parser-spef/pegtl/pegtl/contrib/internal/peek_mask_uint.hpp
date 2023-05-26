// Copyright (c) 2018-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_CONTRIB_INTERNAL_PEEK_MASK_UINT_HPP
#define TAO_PEGTL_CONTRIB_INTERNAL_PEEK_MASK_UINT_HPP

#include <cstddef>
#include <cstdint>

#include "../../internal/data_and_size.hpp"

#include "read_uint.hpp"

namespace tao::pegtl::internal
{
   template< typename R, typename R::type M >
   struct peek_mask_uint_impl
   {
      using data_t = typename R::type;
      using pair_t = data_and_size< data_t >;

      template< typename ParseInput >
      [[nodiscard]] static pair_t peek( ParseInput& in ) noexcept( noexcept( in.size( sizeof( data_t ) ) ) )
      {
         if( in.size( sizeof( data_t ) ) < sizeof( data_t ) ) {
            return { 0, 0 };
         }
         const data_t data = R::read( in.current() ) & M;
         return { data, sizeof( data_t ) };
      }
   };

   template< std::uint16_t M >
   using peek_mask_uint16_be = peek_mask_uint_impl< read_uint16_be, M >;

   template< std::uint16_t M >
   using peek_mask_uint16_le = peek_mask_uint_impl< read_uint16_le, M >;

   template< std::uint32_t M >
   using peek_mask_uint32_be = peek_mask_uint_impl< read_uint32_be, M >;

   template< std::uint32_t M >
   using peek_mask_uint32_le = peek_mask_uint_impl< read_uint32_le, M >;

   template< std::uint64_t M >
   using peek_mask_uint64_be = peek_mask_uint_impl< read_uint64_be, M >;

   template< std::uint64_t M >
   using peek_mask_uint64_le = peek_mask_uint_impl< read_uint64_le, M >;

}  // namespace tao::pegtl::internal

#endif

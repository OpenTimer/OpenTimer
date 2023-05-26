// Copyright (c) 2018-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_CONTRIB_UINT8_HPP
#define TAO_PEGTL_CONTRIB_UINT8_HPP

#include "../internal/result_on_found.hpp"
#include "../internal/rules.hpp"

#include "internal/peek_mask_uint8.hpp"
#include "internal/peek_uint8.hpp"

namespace tao::pegtl::uint8
{
   // clang-format off
   struct any : internal::any< internal::peek_uint8 > {};

   template< std::uint8_t... Cs > struct not_one : internal::one< internal::result_on_found::failure, internal::peek_uint8, Cs... > {};
   template< std::uint8_t Lo, std::uint8_t Hi > struct not_range : internal::range< internal::result_on_found::failure, internal::peek_uint8, Lo, Hi > {};
   template< std::uint8_t... Cs > struct one : internal::one< internal::result_on_found::success, internal::peek_uint8, Cs... > {};
   template< std::uint8_t Lo, std::uint8_t Hi > struct range : internal::range< internal::result_on_found::success, internal::peek_uint8, Lo, Hi > {};
   template< std::uint8_t... Cs > struct ranges : internal::ranges< internal::peek_uint8, Cs... > {};
   template< std::uint8_t... Cs > struct string : internal::seq< internal::one< internal::result_on_found::success, internal::peek_uint8, Cs >... > {};

   template< std::uint8_t M, std::uint8_t... Cs > struct mask_not_one : internal::one< internal::result_on_found::failure, internal::peek_mask_uint8< M >, Cs... > {};
   template< std::uint8_t M, std::uint8_t Lo, std::uint8_t Hi > struct mask_not_range : internal::range< internal::result_on_found::failure, internal::peek_mask_uint8< M >, Lo, Hi > {};
   template< std::uint8_t M, std::uint8_t... Cs > struct mask_one : internal::one< internal::result_on_found::success, internal::peek_mask_uint8< M >, Cs... > {};
   template< std::uint8_t M, std::uint8_t Lo, std::uint8_t Hi > struct mask_range : internal::range< internal::result_on_found::success, internal::peek_mask_uint8< M >, Lo, Hi > {};
   template< std::uint8_t M, std::uint8_t... Cs > struct mask_ranges : internal::ranges< internal::peek_mask_uint8< M >, Cs... > {};
   template< std::uint8_t M, std::uint8_t... Cs > struct mask_string : internal::seq< internal::one< internal::result_on_found::success, internal::peek_mask_uint8< M >, Cs >... > {};
   // clang-format on

}  // namespace tao::pegtl::uint8

#endif

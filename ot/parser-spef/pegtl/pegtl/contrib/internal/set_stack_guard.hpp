// Copyright (c) 2020-2022 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_CONTRIB_INTERNAL_SET_STACK_GUARD_HPP
#define TAO_PEGTL_CONTRIB_INTERNAL_SET_STACK_GUARD_HPP

#include <set>
#include <utility>

namespace tao::pegtl::internal
{
   template< typename... Cs >
   class [[nodiscard]] set_stack_guard
   {
   public:
      template< typename... Ts >
      set_stack_guard( std::set< Cs... >& set, Ts&&... ts )
         : m_i( set.emplace( std::forward< Ts >( ts )... ) ),
           m_s( set )
      {}

      set_stack_guard( set_stack_guard&& ) = delete;
      set_stack_guard( const set_stack_guard& ) = delete;

      set_stack_guard& operator=( set_stack_guard&& ) = delete;
      set_stack_guard& operator=( const set_stack_guard& ) = delete;

      ~set_stack_guard()
      {
         if( m_i.second ) {
            m_s.erase( m_i.first );
         }
      }

      [[nodiscard]] explicit operator bool() const noexcept
      {
         return m_i.second;
      }

   private:
      const std::pair< typename std::set< Cs... >::iterator, bool > m_i;
      std::set< Cs... >& m_s;
   };

   template< typename... Cs >
   set_stack_guard( std::set< Cs... >&, const typename std::set< Cs... >::value_type& ) -> set_stack_guard< Cs... >;

}  // namespace tao::pegtl::internal

#endif

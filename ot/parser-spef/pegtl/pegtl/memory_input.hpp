// Copyright (c) 2014-2023 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PEGTL_MEMORY_INPUT_HPP
#define TAO_PEGTL_MEMORY_INPUT_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "config.hpp"
#include "eol.hpp"
#include "normal.hpp"
#include "nothing.hpp"
#include "position.hpp"
#include "tracking_mode.hpp"

#include "internal/action_input.hpp"
#include "internal/at.hpp"
#include "internal/bump.hpp"
#include "internal/eolf.hpp"
#include "internal/inputerator.hpp"
#include "internal/rewind_guard.hpp"
#include "internal/until.hpp"

namespace TAO_PEGTL_NAMESPACE
{
   namespace internal
   {
      template< tracking_mode, typename Eol, typename Source >
      class memory_input_base;

      template< typename Eol, typename Source >
      class memory_input_base< tracking_mode::eager, Eol, Source >
      {
      public:
         using inputerator_t = internal::inputerator;

         template< typename T >
         memory_input_base( const inputerator_t& in_begin, const char* in_end, T&& in_source ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
            : m_begin( in_begin.data ),
              m_current( in_begin ),
              m_end( in_end ),
              m_source( std::forward< T >( in_source ) )
         {}

         template< typename T >
         memory_input_base( const char* in_begin, const char* in_end, T&& in_source ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
            : m_begin( in_begin ),
              m_current( in_begin ),
              m_end( in_end ),
              m_source( std::forward< T >( in_source ) )
         {}

         memory_input_base( const memory_input_base& ) = delete;
         memory_input_base( memory_input_base&& ) = delete;

         ~memory_input_base() = default;

         memory_input_base& operator=( const memory_input_base& ) = delete;
         memory_input_base& operator=( memory_input_base&& ) = delete;

         [[nodiscard]] const char* current() const noexcept
         {
            return m_current.data;
         }

         [[nodiscard]] const char* begin() const noexcept
         {
            return m_begin;
         }

         [[nodiscard]] const char* end( const std::size_t /*unused*/ = 0 ) const noexcept
         {
            return m_end;
         }

         [[nodiscard]] std::size_t byte() const noexcept
         {
            return m_current.byte;
         }

         [[nodiscard]] std::size_t line() const noexcept
         {
            return m_current.line;
         }

         [[nodiscard]] std::size_t column() const noexcept
         {
            return m_current.column;
         }

         void bump( const std::size_t in_count = 1 ) noexcept
         {
            internal::bump( m_current, in_count, Eol::ch );
         }

         void bump_in_this_line( const std::size_t in_count = 1 ) noexcept
         {
            internal::bump_in_this_line( m_current, in_count );
         }

         void bump_to_next_line( const std::size_t in_count = 1 ) noexcept
         {
            internal::bump_to_next_line( m_current, in_count );
         }

         [[nodiscard]] TAO_PEGTL_NAMESPACE::position position( const inputerator_t& it ) const
         {
            return TAO_PEGTL_NAMESPACE::position( it, m_source );
         }

         void restart( const std::size_t in_byte = 0, const std::size_t in_line = 1, const std::size_t in_column = 1 )
         {
            assert( in_line != 0 );
            assert( in_column != 0 );

            m_current.data = m_begin;
            m_current.byte = in_byte;
            m_current.line = in_line;
            m_current.column = in_column;
            private_depth = 0;
         }

      protected:
         const char* const m_begin;
         inputerator_t m_current;
         const char* m_end;
         const Source m_source;

      public:
         std::size_t private_depth = 0;
      };

      template< typename Eol, typename Source >
      class memory_input_base< tracking_mode::lazy, Eol, Source >
      {
      public:
         using inputerator_t = const char*;

         template< typename T >
         memory_input_base( const internal::inputerator& in_begin, const char* in_end, T&& in_source ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
            : m_begin( in_begin ),
              m_current( in_begin.data ),
              m_end( in_end ),
              m_source( std::forward< T >( in_source ) )
         {}

         template< typename T >
         memory_input_base( const char* in_begin, const char* in_end, T&& in_source ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
            : m_begin( in_begin ),
              m_current( in_begin ),
              m_end( in_end ),
              m_source( std::forward< T >( in_source ) )
         {}

         memory_input_base( const memory_input_base& ) = delete;
         memory_input_base( memory_input_base&& ) = delete;

         ~memory_input_base() = default;

         memory_input_base& operator=( const memory_input_base& ) = delete;
         memory_input_base& operator=( memory_input_base&& ) = delete;

         [[nodiscard]] const char* current() const noexcept
         {
            return m_current;
         }

         [[nodiscard]] const char* begin() const noexcept
         {
            return m_begin.data;
         }

         [[nodiscard]] const char* end( const std::size_t /*unused*/ = 0 ) const noexcept
         {
            return m_end;
         }

         [[nodiscard]] std::size_t byte() const noexcept
         {
            return std::size_t( current() - m_begin.data );
         }

         void bump( const std::size_t in_count = 1 ) noexcept
         {
            m_current += in_count;
         }

         void bump_in_this_line( const std::size_t in_count = 1 ) noexcept
         {
            m_current += in_count;
         }

         void bump_to_next_line( const std::size_t in_count = 1 ) noexcept
         {
            m_current += in_count;
         }

         [[nodiscard]] TAO_PEGTL_NAMESPACE::position position( const inputerator_t it ) const
         {
            internal::inputerator c( m_begin );
            internal::bump( c, static_cast< std::size_t >( it - m_begin.data ), Eol::ch );
            return TAO_PEGTL_NAMESPACE::position( c, m_source );
         }

         void restart()
         {
            m_current = m_begin.data;
            private_depth = 0;
         }

      protected:
         const internal::inputerator m_begin;
         inputerator_t m_current;
         const char* m_end;
         const Source m_source;

      public:
         std::size_t private_depth = 0;
      };

   }  // namespace internal

   template< tracking_mode P = tracking_mode::eager, typename Eol = eol::lf_crlf, typename Source = std::string >
   class memory_input
      : public internal::memory_input_base< P, Eol, Source >
   {
   public:
      static constexpr tracking_mode tracking_mode_v = P;

      using eol_t = Eol;
      using source_t = Source;

      using typename internal::memory_input_base< P, Eol, Source >::inputerator_t;

      using action_t = internal::action_input< memory_input >;

      using internal::memory_input_base< P, Eol, Source >::memory_input_base;

      template< typename T >
      memory_input( const char* in_begin, const std::size_t in_size, T&& in_source ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
         : memory_input( in_begin, in_begin + in_size, std::forward< T >( in_source ) )
      {}

      template< typename T >
      memory_input( const std::string& in_string, T&& in_source ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
         : memory_input( in_string.data(), in_string.size(), std::forward< T >( in_source ) )
      {}

      template< typename T >
      memory_input( const std::string_view in_string, T&& in_source ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
         : memory_input( in_string.data(), in_string.size(), std::forward< T >( in_source ) )
      {}

      template< typename T >
      memory_input( std::string&&, T&& ) = delete;

      template< typename T >
      memory_input( const char* in_begin, T&& in_source ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
         : memory_input( in_begin, std::strlen( in_begin ), std::forward< T >( in_source ) )
      {}

      template< typename T >
      memory_input( const char* in_begin, const char* in_end, T&& in_source, const std::size_t in_byte, const std::size_t in_line, const std::size_t in_column ) noexcept( std::is_nothrow_constructible_v< Source, T&& > )
         : memory_input( { in_begin, in_byte, in_line, in_column }, in_end, std::forward< T >( in_source ) )
      {}

      memory_input( const memory_input& ) = delete;
      memory_input( memory_input&& ) = delete;

      ~memory_input() = default;

      memory_input& operator=( const memory_input& ) = delete;
      memory_input& operator=( memory_input&& ) = delete;

      [[nodiscard]] const Source& source() const noexcept
      {
         return this->m_source;
      }

      [[nodiscard]] bool empty() const noexcept
      {
         return this->current() == this->end();
      }

      [[nodiscard]] std::size_t size( const std::size_t /*unused*/ = 0 ) const noexcept
      {
         return std::size_t( this->end() - this->current() );
      }

      [[nodiscard]] char peek_char( const std::size_t offset = 0 ) const noexcept
      {
         return this->current()[ offset ];
      }

      [[nodiscard]] std::uint8_t peek_uint8( const std::size_t offset = 0 ) const noexcept
      {
         return static_cast< std::uint8_t >( peek_char( offset ) );
      }

      [[nodiscard]] inputerator_t& inputerator() noexcept
      {
         return this->m_current;
      }

      [[nodiscard]] const inputerator_t& inputerator() const noexcept
      {
         return this->m_current;
      }

      using internal::memory_input_base< P, Eol, Source >::restart;

      template< rewind_mode M, typename ParseInput >
      void restart( const internal::rewind_guard< M, ParseInput >& m ) noexcept
      {
         this->m_current = m.inputerator();
      }

      using internal::memory_input_base< P, Eol, Source >::position;

      [[nodiscard]] TAO_PEGTL_NAMESPACE::position position() const
      {
         return position( inputerator() );
      }

      [[nodiscard]] TAO_PEGTL_NAMESPACE::position current_position() const
      {
         return position( inputerator() );
      }

      void discard() const noexcept {}

      void require( const std::size_t /*unused*/ ) const noexcept {}

      template< rewind_mode M >
      [[nodiscard]] internal::rewind_guard< M, memory_input > auto_rewind() noexcept
      {
         return internal::rewind_guard< M, memory_input >( this );
      }

      [[nodiscard]] const inputerator_t& rewind_save() noexcept
      {
         return this->m_current;
      }

      void rewind_restore( const inputerator_t& data ) noexcept
      {
         this->m_current = data;
      }

      [[nodiscard]] const char* at( const TAO_PEGTL_NAMESPACE::position& p ) const noexcept
      {
         return this->begin() + p.byte;
      }

      [[nodiscard]] const char* begin_of_line( const TAO_PEGTL_NAMESPACE::position& p ) const noexcept
      {
         return at( p ) - ( p.column - 1 );
      }

      [[nodiscard]] const char* end_of_line( const TAO_PEGTL_NAMESPACE::position& p ) const noexcept
      {
         using input_t = memory_input< tracking_mode::lazy, Eol, const char* >;
         input_t in( at( p ), this->end(), "" );
         using grammar = internal::until< internal::at< internal::eolf > >;
         (void)normal< grammar >::match< apply_mode::nothing, rewind_mode::optional, nothing, normal >( in );
         return in.current();
      }

      [[nodiscard]] std::string_view line_at( const TAO_PEGTL_NAMESPACE::position& p ) const noexcept
      {
         const char* b = begin_of_line( p );
         return { b, static_cast< std::size_t >( end_of_line( p ) - b ) };
      }

      void private_set_end( const char* new_end ) noexcept
      {
         this->m_end = new_end;
      }
   };

   template< typename... Ts >
   memory_input( Ts&&... ) -> memory_input<>;

}  // namespace TAO_PEGTL_NAMESPACE

#endif

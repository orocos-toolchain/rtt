/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  parser-types.hpp

                        parser-types.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#ifndef PARSER_TYPES_HPP
#define PARSER_TYPES_HPP

#include <boost/spirit.hpp>
#include "corelib/CommandInterface.hpp"

namespace ORO_Execution
{
  class ProgramGraph;
  class VertexNode;
  class EdgeCondition;
}

namespace ORO_CoreLib
{
  template< class T>
  class Property;
  class PropertyBag;
  class PropertyBase;
  class ConditionInterface;
}

namespace ORO_Execution
{
    class TaskContext;

    namespace detail {
        class ExpressionParser;
        class ArgumentsParser;
    }

  using ORO_CoreLib::PropertyBag;
  using ORO_CoreLib::PropertyBase;
  using ORO_CoreLib::ConditionInterface;
  using ORO_CoreLib::CommandInterface;

  using namespace boost::spirit;

  typedef std::string our_buffer_t;
  typedef our_buffer_t::iterator our_iterator_t;
  typedef position_iterator<our_iterator_t> our_pos_iter_t;
  // this is the iterator type that the parsers have to work with.  It
  // might change some time in the future..
  typedef our_pos_iter_t iter_t;

  // a macro using GCC's C++ extension typeof that is used to not have
  // to specify impossibly long type names..  See the Boost.Spirit
  // documentation for more details, as that's where I got it from..
  // we use __typeof__ instead of typeof because it is not disabled by
  // using gcc -ansi
#   define RULE( name, def ) \
      __typeof__( (def) ) name = (def)


#if 1
    /**
     * Parser used in skip parser. Set skipeol to 'true' to
     * skip newlines, set skipeol to false to not skip newlines.
     */
    struct eol_skip_functor
    {
        /**
         * By default, eol are skipped.
         */
        static bool skipeol;
        typedef nil_t result_t;

        template <typename ScannerT>
        std::ptrdiff_t
        operator()(ScannerT const& scan, result_t& result) const {
            if (scan.at_end() || skipeol == false )
                return -1;

            std::size_t len = 0;

            if ( *scan == '\r') {
                ++scan;
                ++len;
            }

            if ( !scan.at_end() && *scan == '\n') {
                ++scan;
                ++len;
            }
            if ( len > 0 ) {
                return len;
            }

            return -1;
        }
    };

    /**
     * Use this global parser to skip newlines inside statements.
     */
    extern functor_parser<eol_skip_functor> eol_skip_p;

#   define SKIP_PARSER \
      ( comment_p( "#" ) | comment_p( "//" ) | \
        comment_p( "/*", "*/" ) | (space_p - eol_p) | eol_skip_p  )

  // here are the typedef's for the scanner, and the rule types..
  typedef __typeof__( SKIP_PARSER ) skip_parser_t;
#else
  // we need to know what type the skip parser will be in order to be
  // able to know what types the scanner and rule will be exactly.
  // So we have to put a typedef here, and in order to not put the skip
  // parser definition in two places, we put it here as a macro, and use
  // the macro at the appropriate places..

  // we support shell/perl ( "# comment\n" ), C ( non-nested "/*
  // comment */" ) and C++ ( "// comment\n" ) style comments.
  // These are skipped at the scanner level, by using the standard
  // Boost.Spirit skip_iteration_policy.

#   define SKIP_PARSER \
      ( comment_p( "#" ) | comment_p( "//" ) | \
        comment_p( "/*", "*/" ) | (space_p - eol_p) )

  // here are the typedef's for the scanner, and the rule types..
  typedef __typeof__( SKIP_PARSER ) skip_parser_t;
#endif
  typedef skip_parser_iteration_policy<skip_parser_t> iter_pol_t;
  typedef scanner_policies<iter_pol_t> scanner_pol_t;
  typedef scanner<iter_t, scanner_pol_t> scanner_t;
  typedef rule<scanner_t> rule_t;
  typedef rule<lexeme_scanner<scanner_t>::type > lexeme_rule_t;

}

#endif

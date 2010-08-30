/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  parser-types.hpp

                        parser-types.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#ifndef PARSER_TYPES_HPP
#define PARSER_TYPES_HPP

#include "rtt-scripting-config.h"

#include <boost/version.hpp>

#if BOOST_VERSION >= 103800
#include <boost/spirit/include/classic.hpp>
namespace boost_spirit = boost::spirit::classic;
#else
#include <boost/spirit.hpp>
namespace boost_spirit = boost::spirit;
#endif
#include "../base/ActionInterface.hpp"
#include "rtt-scripting-fwd.hpp"

#undef interface	// To avoid name clash with namespace interface and Windows SDK objbase.h included through boost/spirit

namespace RTT
{ namespace scripting {

       using namespace boost_spirit;

  typedef std::string our_buffer_t;
  typedef our_buffer_t::iterator our_iterator_t;
  typedef position_iterator<our_iterator_t> our_pos_iter_t;
  // this is the iterator type that the parsers have to work with.  It
  // might change some time in the future..
  typedef our_pos_iter_t iter_t;

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
        eol_skip_functor(bool& skipref) : skipeol(skipref) {}
        eol_skip_functor(eol_skip_functor const& orig) : skipeol( orig.skipeol ) {}
        bool& skipeol;
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

    //! When using this macro, you must have a boolean variable skipref defined.
    //! which is initialized to true.
#   define SKIP_PARSER \
      ( comment_p( "#" ) | comment_p( "//" ) | \
        comment_p( "/*", "*/" ) | (space_p - eol_p) | functor_parser<eol_skip_functor>( eol_skip_functor(skipref) ) )

  // here are the typedef's for the scanner, and the rule types..
  //typedef __typeof__ ( SKIP_PARSER ) skip_parser_t;
	
	//register SKIP_PARSER with typeof system 
	//struct X {};
	//BOOST_TYPEOF_REGISTER_TYPE(X);
	//TODO:
	//typedef alternative<chlit<>, alternative<chlit<>, alternative<chlit<>, alternative<chlit<>, chlit<> > > > > skip_parser_t;
	typedef boost_spirit::alternative<boost_spirit::alternative<boost_spirit::alternative<boost_spirit::alternative<boost_spirit::confix_parser<boost_spirit::impl::string_as_parser::type,boost_spirit::kleene_star<boost_spirit::anychar_parser>,boost_spirit::alternative<boost_spirit::eol_parser,boost_spirit::end_parser>,boost_spirit::unary_parser_category,boost_spirit::non_nested,boost_spirit::is_lexeme>,boost_spirit::confix_parser<boost_spirit::impl::string_as_parser::type,boost_spirit::kleene_star<boost_spirit::anychar_parser>,boost_spirit::alternative<boost_spirit::eol_parser,boost_spirit::end_parser>,boost_spirit::unary_parser_category,boost_spirit::non_nested,boost_spirit::is_lexeme> >,boost_spirit::confix_parser<boost_spirit::impl::string_as_parser::type,boost_spirit::kleene_star<boost_spirit::anychar_parser>,boost_spirit::impl::string_as_parser::type,boost_spirit::unary_parser_category,boost_spirit::non_nested,boost_spirit::is_lexeme> >,boost_spirit::difference<boost_spirit::space_parser,boost_spirit::eol_parser> >,boost_spirit::functor_parser<eol_skip_functor> > skip_parser_t;

	//typedef BOOST_TYPEOF( SKIP_PARSER ) skip_parser_t;
	/*typedef
		alternative<alternative<space_parser, sequence<sequence<
               strlit<const char*>, kleene_star<difference<anychar_parser,
               chlit<char> > > >, chlit<char> > >, sequence<sequence<
               strlit<const char*>, kleene_star<difference<anychar_parser,
               strlit<const char*> > > >, strlit<const char*> > >
	skip_parser_t;*/

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

}}

#endif

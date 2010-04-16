/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:07 CEST 2004  CommonParser.cxx

                        CommonParser.cxx -  description
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
#include <boost/bind.hpp>

#include "parse_exception.hpp"
#include "parser-debug.hpp"
#include "CommonParser.hpp"

namespace RTT {
    using boost::bind;
    using namespace detail;

    namespace {
        assertion<std::string> expect_eos("End of statement expected. Use a newline or ';' to separate statements.");
    }

    CommonParser::CommonParser()
        : identchar( "a-zA-Z_0-9" ), skipeol(true),
          skipper( eol_skip_functor(skipeol) )
    {
        // we reserve a few words
        keywordstable =
            "do",
            "until",
            "done",
            "or",
            "and",
            "not",
            "include",
            "if",
            "define",
            "then",
            "else",
            "for",
            "foreach",
            "while",
            "true",
            "false",
            "async",
            "time",
            "const",
            "nothing", // do not exclude 'do nothing' !
            "yield",
            "var",
            "set",
            "alias",
            "sync",
            "send",
            "call",
            "collect",
            "collectIfDone",
            "return",
            "call",
            "try",
            "catch";

        BOOST_SPIRIT_DEBUG_RULE( idr );
        BOOST_SPIRIT_DEBUG_RULE( idlr );
        BOOST_SPIRIT_DEBUG_RULE( eos );
        BOOST_SPIRIT_DEBUG_RULE( notassertingeos );
        BOOST_SPIRIT_DEBUG_RULE( leos );
        BOOST_SPIRIT_DEBUG_RULE( endofkeyword );
        BOOST_SPIRIT_DEBUG_RULE( keywords );
        BOOST_SPIRIT_DEBUG_RULE( keyword );
        BOOST_SPIRIT_DEBUG_RULE( identifier );
        BOOST_SPIRIT_DEBUG_RULE( identchar );
        BOOST_SPIRIT_DEBUG_RULE( notassertingidentifier );
        BOOST_SPIRIT_DEBUG_RULE( lexeme_identifier );
        BOOST_SPIRIT_DEBUG_RULE( lexeme_notassertingidentifier );

        // an identifier is a word which can be used to identify a
        // label, or be the name of an object or method.  it is required
        // to start with a letter, followed by any number of letters,
        // numbers, dashes, underscores or letters.  The keywords we
        // reserved above are excluded..
        keywords = keywordstable;
        endofkeyword = (~identchar) | eol_p | end_p;
        keyword = lexeme_d[keywords >> eps_p(endofkeyword)];

        // if a rule is going to be used inside a lexeme_d, then it
        // needs to be of a different type..  Since identifier is used
        // both inside and outside of lexeme_d, we need two versions of
        // it.  Those are provided here: lexeme_identifier and
        // identifier..
        idr  = lexeme_d[ alpha_p >> *identchar ][assign( lastparsedident )] - keyword;
        idlr = lexeme_d[ alpha_p >> *identchar ][assign( lastparsedident )] - keyword;
        // #warning " Rule on stack  ?? "
        //RULE( identifier_base, lexeme_d[ alpha_p >> *identchar ][assign( lastparsedident )] - as_lower_d[keywords] );
        //BOOST_SPIRIT_DEBUG_RULE( identifier_base );
        lexeme_identifier = idlr | keyword[bind( &CommonParser::seenillegalidentifier, this )];
        lexeme_notassertingidentifier = idlr;

        notassertingidentifier = idr;
        identifier = idr | keyword[bind( &CommonParser::seenillegalidentifier, this )];

        // end of statement is on a newline or a ';'
        //eos = lexeme_d[ *(space_p - eol_p) >> (eol_p | ch_p(';')) ];
        eos = expect_eos( notassertingeos ); // detect } as eos, but do not consume.
        notassertingeos = eol_p | ch_p(';') | eps_p(ch_p('}')); // detect } as eos, but do not consume.
        leos = *(space_p - eol_p) >> (eol_p | ch_p(';') | eps_p(ch_p('}')));
    }

    void CommonParser::seenillegalidentifier()
    {
        throw parse_exception_illegal_identifier( lastparsedident );
    }
}

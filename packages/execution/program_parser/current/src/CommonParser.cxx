/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:07 CEST 2004  CommonParser.cxx

                        CommonParser.cxx -  description
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
#include <boost/bind.hpp>

#include "execution/parse_exception.hpp"
#include "execution/parser-debug.hpp"
#include "execution/CommonParser.hpp"

namespace ORO_Execution {
    using boost::bind;

    CommonParser::CommonParser()
    {
        // we reserve a few words
        keywords =
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
            "frame",
            "double",
            "int",
            "bool",
            "char",
            "string",
            "vector",
            "double6d",
            "rotation",
            "twist",
            "wrench",
            "nothing", // do not exclude 'do nothing' !
            "var",
            "set",
            "let",
            "alias",
            "to",
            "sync",
            "return",
            "call",
            "inState",
            "SubContext",
            "RootContext",
            "StateContext",
            "EventHandle",
            "initial",
            "final",
            "state",
            "preconditions",
            "entry",
            "exit",
            "handle",
            "transitions",
            "emit",
            "select",
            "connect",
            "disconnect",
            "try",
            "catch";

        chset<> identchar( "a-zA-Z_0-9" );

        // an identifier is a word which can be used to identify a
        // label, or be the name of an object or method.  it is required
        // to start with a letter, followed by any number of letters,
        // numbers, dashes, underscores or letters.  The keywords we
        // reserved above are excluded..

        // if a rule is going to be used inside a lexeme_d, then it
        // needs to be of a different type..  Since identifier is used
        // both inside and outside of lexeme_d, we need two versions of
        // it.  Those are provided here: lexeme_identifier and
        // identifier..
        RULE( identifier_base, lexeme_d[ alpha_p >> *identchar ][assign( lastparsedident )] - as_lower_d[keywords]);
        lexeme_identifier = identifier_base | keywords[bind( &CommonParser::seenillegalidentifier, this )];
        notassertingidentifier = identifier_base;
        identifier = identifier_base | keywords[bind( &CommonParser::seenillegalidentifier, this )];
        lexeme_notassertingidentifier = identifier_base;

        BOOST_SPIRIT_DEBUG_RULE( identifier );
        BOOST_SPIRIT_DEBUG_RULE( notassertingidentifier );
        BOOST_SPIRIT_DEBUG_RULE( lexeme_identifier );
        BOOST_SPIRIT_DEBUG_RULE( lexeme_notassertingidentifier );
    }

    void CommonParser::seenillegalidentifier()
    {
        throw parse_exception_illegal_identifier( lastparsedident );
    }
}

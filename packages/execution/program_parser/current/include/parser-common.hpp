/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  parser-common.hpp 

                        parser-common.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef PARSER_COMMON_HPP
#define PARSER_COMMON_HPP

//#define BOOST_SPIRIT_DEBUG 
#define BOOST_SPIRIT_DEBUG_OUT std::cerr

#include <boost/spirit.hpp>

namespace ORO_Execution
{
  class ProgramGraph;
  class VertexNode;
  class EdgeCondition;
  class Processor;
}

namespace ORO_CoreLib
{
  class PropertyBag;
  class PropertyBase;
  class CommandInterface;
  class ConditionInterface;
}

namespace ORO_Execution
{
  class ParseContext;
  class ExpressionParser;
  class ParsedValueBase;
  class ArgumentsParser;
  class TypeInfo;
  class TypeInfoRepository;
  class GlobalFactory;
  class DataSourceBase;

  using ORO_CoreLib::PropertyBag;
  using ORO_CoreLib::PropertyBase;
  using ORO_CoreLib::CommandInterface;
  using ORO_CoreLib::ConditionInterface;

  using namespace boost::spirit;

  typedef std::string our_buffer_t;
  typedef our_buffer_t::iterator our_iterator_t;
  typedef position_iterator<our_iterator_t> our_pos_iter_t;
  // this is the iterator type that the parsers have to work with.  It
  // might change some time in the future..
  typedef our_pos_iter_t iter_t;

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
        comment_p( "/*", "*/" ) | ( space_p - ch_p( '\n' ) ) )

  // a macro using GCC's C++ extension typeof that is used to not have
  // to specify impossibly long type names..  See the Boost.Spirit
  // documentation for more details, as that's where I got it from..
#   define RULE( name, def ) \
      typeof( (def) ) name = (def)

  // here are the typedef's for the scanner, and the rule types..
  typedef typeof( SKIP_PARSER ) skip_parser_t;
  typedef skip_parser_iteration_policy<skip_parser_t> iter_pol_t;
  typedef scanner_policies<iter_pol_t> scanner_pol_t;
  typedef scanner<iter_t, scanner_pol_t> scanner_t;
  typedef rule<scanner_t> rule_t;
  typedef rule<lexeme_scanner<scanner_t>::type > lexeme_rule_t;

  /**
   * This is the exception class that is thrown by all of the parsers
   * to indicate a parse error.  It holds a string describing what
   * went wrong.
   */
  class parse_exception
  {
    const std::string mwhat;
  public:
    parse_exception( const std::string& what )
      : mwhat( what ) {};
    const char* what() const throw () { return mwhat.c_str(); };
  };

  /**
   * @brief This class contains some very common parser definitions.
   */
  class CommonParser
  {
  public:
    symbols<nil_t> keywords;
    rule_t identifier;
    lexeme_rule_t lexeme_identifier;

    CommonParser()
      {
        // we reserve a few words
        keywords =
          "do", "until", "next", "done", "or", "and", "not", "include", "if",
          "define", "end", "then", "else", "for", "foreach", "while", "true",
          "false", "stop", "async", "time", "const", "frame", "double",
          "int", "bool", "char", "string", "vector", "rotation", "twist",
          "wrench", "nothing", "var", "set", "let", "alias", "to", "sync",
          "return", "call";

        chset<> identchar( "a-zA-Z-_0-9" );

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
        RULE( identifier_base, lexeme_d[ alpha_p >> *identchar ] - as_lower_d[keywords] );
        lexeme_identifier = identifier_base;
        identifier = identifier_base;

        BOOST_SPIRIT_DEBUG_RULE( identifier );
        BOOST_SPIRIT_DEBUG_RULE( lexeme_identifier );
      };
  };
}

#endif

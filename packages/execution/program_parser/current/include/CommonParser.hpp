/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  CommonParser.hpp

                        CommonParser.hpp -  description
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
#ifndef COMMONPARSER_HPP
#define COMMONPARSER_HPP

#include "parser-types.hpp"

namespace RTT { namespace detail
{

  /**
   * @brief This class contains some very common parser definitions.
   * @todo check the identifier parser implementation of asserting when 
   * a keyword is wrongly used as an identifier. In current implementation
   * it almost never throws.
   */
  class CommonParser
  {
      std::string lastparsedident;
  public:
      // The characters of an identifier after the first char.
      chset<> identchar;

      symbols<nil_t> keywords;
      // parses any reserved keyword
      rule_t keyword;

      // the normal identifier throws an error if it fails to match
      // because a keyword was used.  Normally this is no problem, but
      // if it really is, use notassertingidentifier.
      rule_t idr;
      lexeme_rule_t idlr;
      rule_t identifier;
      rule_t notassertingidentifier;
      lexeme_rule_t lexeme_identifier;
      lexeme_rule_t lexeme_notassertingidentifier;
      
      //@{
      /**
       * End Of Statement Parser. Identifies the end of a statement
       * in Orocos scripting.
       */
      rule_t eos;
      lexeme_rule_t leos;
      //@}

      CommonParser();

      void seenillegalidentifier();
  };
}}

#endif

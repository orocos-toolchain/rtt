/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ArgumentsParser.hpp 

                        ArgumentsParser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
#ifndef ARGUMENTSPARSER_HPP
#define ARGUMENTSPARSER_HPP

#include "parser-types.hpp"

#include <corelib/PropertyBag.hpp>
#include "CommonParser.hpp"
#include "TaskContext.hpp"

#pragma interface

namespace ORO_Execution
{
  using ORO_CoreLib::PropertyBagOwner;
  using ORO_CoreLib::PropertyBag;

  /**
   * This is a parser that you construct to parse a set of arguments.
   * It is meant to only be constructed after the objectname and
   * methodname have been parsed.  You should then assign its parser
   * to a rule ( it is one of boost.spirit coolest features that it
   * allows to do this during parsing ), and get the results back
   * after the rule is done..
   * You are supposed to construct a new ArgumentsParser for every
   * method call you see.
   */
  class ArgumentsParser
  {
    // the arguments we have already parsed.  We take a reference to
    // them when they reach us, and release it in the destructor
    std::vector<DataSourceBase*> margs;
    bool mparsed;

    rule_t arguments, argument;

    ExpressionParser& expressionparser;

    void seen_arg();
    void seenarguments();

    std::string mobject;
    std::string mmethod;
      TaskContext* _peer;
  public:
    /**
     * Construct a new ArgumentsParser.  The object and method
     * parameters are for use in its error messages, and will
     * otherwise not be used..
     */
      ArgumentsParser( ExpressionParser& p, TaskContext* peer,
                       const std::string& object, const std::string& method );

      ~ArgumentsParser();

      TaskContext* peer()
      {
          return _peer;
      }

    rule_t& parser()
      {
        return arguments;
      };

    /**
     * Get the parsed DataSource's..
     * ArgumentsParser holds a reference to the returned DataSource's,
     * but releases it in its destructor..  You should take a
     * reference yourself if you're going to be using them after
     * that..
     */
    std::vector<DataSourceBase*> result()
      {
        return margs;
      };
    /**
     * Some syntaxes allow an argument list to be left out, and then
     * this variable is useful to find out whether an argument list
     * was actually parsed or not..
     */
    bool parsed() const
      {
        return mparsed;
      };

    std::string objectname()
      {
        return mobject;
      };
    std::string methodname()
      {
        return mmethod;
      };
  };
}

#endif

/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  ConditionParser.hpp 

                        ConditionParser.hpp -  description
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
 
 
#ifndef CONDITIONPARSER_HPP
#define CONDITIONPARSER_HPP

#include "parser-common.hpp"
#include "ExpressionParser.hpp"
#include "DataSource.hpp"

#include <memory>
#include <stack>

namespace ORO_Execution
{
  /**
   * This is a class containing a parse function for conditions.  It
   * is used by ProgramParser, and probably other parser's too in
   * the future...
   */
  class ConditionParser
  {
    ConditionInterface* ret;

    void seendonecondition();
    void seenexpression();

    rule_t condition;

    ParseContext& context;
    CommonParser commonparser;
    ExpressionParser expressionparser;

  public:
    ConditionParser( ParseContext& );
    ~ConditionParser();

    rule_t& parser()
      {
        return condition;
      };

    /**
     * Call this to get the parsed condition.  If you use it, you
     * should subsequently call reset(), otherwise it will be deleted
     * in the ConditionParser destructor..
     */
    ConditionInterface* getParseResult()
      {
        return ret;
      };

    void reset();
  };
}

#endif

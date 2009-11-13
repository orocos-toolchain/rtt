/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  ConditionParser.hpp

                        ConditionParser.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef CONDITIONPARSER_HPP
#define CONDITIONPARSER_HPP

#include "parser-types.hpp"
#include "CommonParser.hpp"
#include "ExpressionParser.hpp"
#include "../internal/DataSource.hpp"

#include <memory>
#include <stack>
#include <utility>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT { namespace scripting
{
  /**
   * This is a class containing a parse function for conditions.  It
   * is used by ProgramParser, and probably other parser's too in
   * the future...
   */
  class ConditionParser
  {
      internal::DataSource<bool>::shared_ptr ds_bool;

    void seendonecondition();
    void seenexpression();

    rule_t condition;

    TaskContext* context;
    CommonParser commonparser;
    ExpressionParser expressionparser;

  public:
    ConditionParser( TaskContext* );
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
      base::ConditionInterface* getParseResult();

      /**
       * Retrieve the result as a command, condition pair.
       */
      std::pair<base::ActionInterface*,base::ConditionInterface*> getParseResultAsCommand();

    void reset();
  };
}}

#endif

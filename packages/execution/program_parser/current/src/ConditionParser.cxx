/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ConditionParser.cxx

                        ConditionParser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"
#pragma implementation
#include "execution/ConditionParser.hpp"

#include "corelib/ConditionDuration.hpp"
#include "corelib/ConditionTrue.hpp"
#include "corelib/ConditionFalse.hpp"
#include "execution/ConditionBoolDataSource.hpp"
#include "execution/ConditionComposite.hpp"
#include "execution/ConditionCompare.hpp"

#include <boost/bind.hpp>

#include "execution/ParseContext.hpp"

#include "execution/DataSourceFactoryInterface.hpp"
#include "execution/TemplateFactory.hpp"
#include "execution/TryCommand.hpp"

namespace ORO_Execution
{
  using boost::bind;
  using ORO_CoreLib::ConditionDuration;
  using ORO_CoreLib::PropertyBag;

  ConditionParser::ConditionParser( TaskContext* c )
    : ds_bool( 0 ), context( c ), expressionparser( c )
  {
    BOOST_SPIRIT_DEBUG_RULE( condition );

    /**
     * conditions used to be more complex, but nowadays, they're just
     * boolean expressions..
     */
    condition =
      expressionparser.parser() [
        bind( &ConditionParser::seenexpression, this ) ];
  }

    TaskContext* ConditionParser::setStack(TaskContext* tc){
        return expressionparser.setStack( tc );
    }

  void ConditionParser::reset()
  {
      // not strictly needed because its a smart_ptr
      ds_bool = 0;
  };

  ConditionParser::~ConditionParser()
  {
  };

  void ConditionParser::seenexpression()
  {
    // get the datasource parsed by the ExpressionParser..
    DataSourceBase::shared_ptr mcurdata =
      expressionparser.getResult();
    expressionparser.dropResult();

    // The reference count is stored in the DataSource itself !
    // so the ref cnt information is not lost in this cast
    ds_bool =
      dynamic_cast<DataSource<bool>*>( mcurdata.get() );
    if ( ds_bool )
    {
      mcurdata = 0;
    }
    else
    {
      // we only want boolean expressions..
      throw parse_exception_semantic_error(
        "Attempt to use a non-boolean value as a condition." );
    };
  };

    ConditionInterface* ConditionParser::getParseResult()
      {
          // wrap the datasource in a ConditionBoolDataSource..
          return new ConditionBoolDataSource( ds_bool.get() );
      };

      /**
       * Retrieve the result as a command, condition pair.
       */
    std::pair<CommandInterface*,ConditionInterface*> ConditionParser::getParseResultAsCommand()
      {
          EvalCommand* ec = new EvalCommand( ds_bool );
          EvalCommandResult* ecr = new EvalCommandResult( ec->cache() );
          return std::make_pair( ec ,ecr );
          //return std::pair<CommandInterface*,ConditionInterface*>(0,0);
      }
}

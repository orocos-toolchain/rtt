/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ConditionParser.cxx

                        ConditionParser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "rtt/scripting/parser-debug.hpp"
#include "rtt/scripting/parse_exception.hpp"
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "rtt/scripting/ConditionParser.hpp"

#include "rtt/scripting/ConditionDuration.hpp"
#include "rtt/scripting/ConditionTrue.hpp"
#include "rtt/scripting/ConditionFalse.hpp"
#include "rtt/scripting/ConditionBoolDataSource.hpp"
#include "rtt/scripting/ConditionComposite.hpp"

#include <boost/bind.hpp>

#include "rtt/scripting/TryCommand.hpp"

namespace RTT
{
    using namespace detail;
    using boost::bind;



    ConditionParser::ConditionParser( TaskContext* c, ExecutionEngine* caller, CommonParser& cp )
        : ds_bool( 0 ), context( c ), commonparser(cp), expressionparser( c, caller, cp )
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

    void ConditionParser::reset()
    {
        // not strictly needed because its a smart_ptr
        ds_bool = 0;
    }

    ConditionParser::~ConditionParser()
    {
    }

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
            }
    }

    ConditionInterface* ConditionParser::getParseResult()
    {
        // wrap the datasource in a ConditionBoolDataSource..
        return new ConditionBoolDataSource( ds_bool.get() );
    }

    /**
     * Retrieve the result as a command, condition pair.
     */
    std::pair<ActionInterface*,ConditionInterface*> ConditionParser::getParseResultAsCommand()
    {
        EvalCommand* ec = new EvalCommand( ds_bool );
        EvalCommandResult* ecr = new EvalCommandResult( ec->cache() );
        return std::make_pair( ec, ecr );
        //return std::pair<ActionInterface*,ConditionInterface*>(0,0);
    }
}

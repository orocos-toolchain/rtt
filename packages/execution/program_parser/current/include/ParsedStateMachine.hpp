/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:33:09 CEST 2004  ParsedStateMachine.hpp

                        ParsedStateMachine.hpp -  description
                           -------------------
    begin                : Tue July 20 2004
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


#ifndef CURRENT_INCLUDE_PARSEDSTATECONTEXT_HPP
#define CURRENT_INCLUDE_PARSEDSTATECONTEXT_HPP

#include "execution/StateMachine.hpp"
#include "execution/DataSource.hpp"
#include <boost/shared_ptr.hpp>

namespace ORO_Execution {

    class StateDescription;
    class Processor;
    class TaskAttributeBase;
    class TaskContext;
    class StateMachineTask;

    namespace detail {
        class StateMachineCommands;
    }

    class ParsedStateMachine;
    typedef boost::shared_ptr<ParsedStateMachine> ParsedStateMachinePtr;

    class ParsedStateMachine
        : public StateMachine
    {
        typedef std::map<std::string, TaskAttributeBase*> VisibleWritableValuesMap;
    protected:
        virtual void handleUnload();
    public:
        ParsedStateMachine();
        virtual ~ParsedStateMachine();

        /**
         * Create a copy, set instantiate to 'true' if instantiating a RootMachine.
         */
        ParsedStateMachinePtr copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate = false ) const;

        void addParameter( const std::string& name, TaskAttributeBase* var );

        TaskAttributeBase* getParameter( const std::string& name ) const;
        VisibleWritableValuesMap getParameters() const;
        std::vector<std::string> getParameterNames() const;

        /**
         * Set the name of this context. If \a recurisive == true, this also sets subMachines'
         * names, to the given name + "." + the name they have been
         * instantiated by in this context.
         */
        void setName( const std::string& name, bool recursive );

        std::string getText() const;

        void setText( std::string text);

        TaskContext* getTaskContext() const;

        void setTaskContext(StateMachineTask* tc);

        bool inState( const std::string& name );
        /**
         * Call this function if the state context is parsed.
         */
        void finish();
    private:
        VisibleWritableValuesMap parametervalues;

        boost::shared_ptr<std::string> _text;

        StateMachineTask* context;
    };
}

#endif

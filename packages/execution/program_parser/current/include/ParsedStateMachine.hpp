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

    namespace detail {
        class StateMachineCommands;
    }

    class ParsedStateMachine
        : public StateMachine
    {
        typedef std::map<std::string, TaskAttributeBase*> VisibleWritableValuesMap;

    public:
        ParsedStateMachine();
        virtual ~ParsedStateMachine();

        /**
         * Create a copy, set instantiate to 'true' if instantiating a RootMachine.
         */
        ParsedStateMachine* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate = false ) const;

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

        std::string getText() const { return *_text; }
        void setText( std::string text) { *_text = text; }

        TaskContext* getTaskContext() const {
            return context;
        }
        void setTaskContext(TaskContext* tc) {
            context = tc;
        }

        bool inState( const std::string& name ) {
            StateInterface* copy = this->currentState();
            if (copy == 0)
                return false;
            return copy->getName() == name;
        }
        /**
         * Call this function if the state context is parsed.
         */
        void finish();
    private:
        VisibleWritableValuesMap parametervalues;

        boost::shared_ptr<std::string> _text;

        TaskContext* context;
        detail::StateMachineCommands* sc_coms;
    };
}

#endif

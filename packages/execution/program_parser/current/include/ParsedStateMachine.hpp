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

namespace ORO_Execution {

    class StateDescription;
    class StateMachineCommands;
    class Processor;
    class TaskAttributeBase;
    class TaskContext;

    class ParsedStateMachine
        : public StateMachine
    {
        typedef std::map<std::string, StateDescription*> StateNameMap;
        // We need to keep the pointers to subcontexts in a
        // datasource, so that commands in the state methods can refer
        // to it using a single datasource.  On copying, we first copy
        // the subcontexts, adding the datasources containing their
        // pointers to the replacement map, so that the new commands
        // refer to the correct subcontexts...  We store this as a
        // DataSource refering to a StateMachine, even though we know
        // that it will only contain ParsedStateMachine's.  This is
        // because it will be used to call functions that accept
        // arguments of type StateMachine, and the DataSource's
        // unfortunately don't know about inheritance or casting...
        typedef std::map<std::string, DataSource<StateMachine*>::shared_ptr> SubContextNameMap;

        typedef std::map<std::string, DataSourceBase::shared_ptr> VisibleReadOnlyValuesMap;
        typedef std::map<std::string, TaskAttributeBase*> VisibleWritableValuesMap;

    public:
        ParsedStateMachine();
        virtual ~ParsedStateMachine();

        /**
         * @brief Create a ParsedStateMachine which is parsed from \a text.
         */
        ParsedStateMachine(const std::string& text);

        std::vector<std::string> getSubContextList() const;
        ParsedStateMachine* getSubContext( const std::string& name ) const;
        /**
         * Add a new SubContext to this context.  This function
         * returns the DataSource by which the program can refer to
         * the given StateMachine...
         */
        DataSource<StateMachine*>* addSubContext( const std::string& name, ParsedStateMachine* sc );

        ParsedStateMachine* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const;

        const std::map<std::string, StateDescription*>& getStates() const {
            return states;
        }
        std::vector<std::string> getStateList() const;

        StateDescription* getState( const std::string& name ) const;

        // is this context currently in the state by the given name ?
        bool inState( const std::string& name ) const;

        void addState( const std::string& name, StateDescription* state );

        void addReadOnlyVar( const std::string& name, DataSourceBase* var );
        void addParameter( const std::string& name, TaskAttributeBase* var );
        DataSourceBase* getReadOnlyVar( const std::string& name ) const;
        TaskAttributeBase* getParameter( const std::string& name ) const;
        VisibleWritableValuesMap getParameters() const;
        VisibleReadOnlyValuesMap getReadOnlyValues() const;
        std::vector<std::string> getParameterNames() const;
        std::vector<std::string> getReadOnlyValuesNames() const;

        /**
         * Return a DataSource which contains the scoped name
         * of this StateMachine.
         */
        DataSource<std::string>* getNameDS() const;

        /**
         * Set the name of this context. If \a recurisive == true, this also sets subcontexts'
         * names, to the given name + "." + the name they have been
         * instantiated by in this context.
         */
        void setName( const std::string& name, bool recursive );

        const std::string& getText() const { return _text; }
        void setText(const std::string& text) { _text = text; }

        TaskContext* getTaskContext() const {
            return context;
        }
        void setTaskContext(TaskContext* tc) {
            context = tc;
        }
        /**
         * Call this function if the state context is parsed.
         */
        void finish();
    private:
        VariableDataSource<std::string>::shared_ptr nameds;

        SubContextNameMap subcontexts;

        StateNameMap states;

        VisibleWritableValuesMap parametervalues;

        VisibleReadOnlyValuesMap visiblereadonlyvalues;

        std::string _text;

        TaskContext* context;
        StateMachineCommands* sc_coms;
    };
}

#endif

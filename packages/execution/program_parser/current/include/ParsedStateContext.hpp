/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:33:09 CEST 2004  ParsedStateContext.hpp

                        ParsedStateContext.hpp -  description
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

#include "execution/StateContextTree.hpp"
#include "execution/DataSource.hpp"

namespace ORO_Execution {
    // using ORO_CoreLib::StateContext;
    using ORO_CoreLib::StateInterface;

    class StateDescription;
    class Processor;
    class ParsedValueBase;

    class ParsedStateContext
        : public StateContextTree
    {
        typedef std::map<std::string, StateDescription*> StateNameMap;
        // We need to keep the pointers to subcontexts in a
        // datasource, so that commands in the state methods can refer
        // to it using a single datasource.  On copying, we first copy
        // the subcontexts, adding the datasources containing their
        // pointers to the replacement map, so that the new commands
        // refer to the correct subcontexts...  We store this as a
        // DataSource refering to a StateContext, even though we know
        // that it will only contain ParsedStateContext's.  This is
        // because it will be used to call functions that accept
        // arguments of type StateContext, and the DataSource's
        // unfortunately don't know about inheritance or casting...
        typedef std::map<std::string, DataSource<StateContextTree*>::shared_ptr> SubContextNameMap;

        typedef std::map<std::string, DataSourceBase::shared_ptr> VisibleReadOnlyValuesMap;
        typedef std::map<std::string, ParsedValueBase*> VisibleWritableValuesMap;

    public:
        ParsedStateContext();
        virtual ~ParsedStateContext();

        std::vector<std::string> getSubContextList() const;
        ParsedStateContext* getSubContext( const std::string& name ) const;
        /**
         * Add a new SubContext to this context.  This function
         * returns the DataSource by which the program can refer to
         * the given StateContext...
         */
        DataSource<StateContextTree*>* addSubContext( const std::string& name, ParsedStateContext* sc );

        ParsedStateContext* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const;

        const std::map<std::string, StateDescription*>& getStates() const {
            return states;
        }
        std::vector<std::string> getStateList() const;

        StateDescription* getState( const std::string& name ) const;

        // is this context currently in the state by the given name ?
        bool inState( const std::string& name ) const;

        void addState( const std::string& name, StateDescription* state );

        void addReadOnlyVar( const std::string& name, DataSourceBase* var );
        void addParameter( const std::string& name, ParsedValueBase* var );
        DataSourceBase* getReadOnlyVar( const std::string& name ) const;
        ParsedValueBase* getParameter( const std::string& name ) const;
        VisibleWritableValuesMap getParameters() const;
        VisibleReadOnlyValuesMap getReadOnlyValues() const;
        std::vector<std::string> getParameterNames() const;
        std::vector<std::string> getReadOnlyValuesNames() const;

        DataSource<std::string>* getNameDS() const;

        // Set the name of this context.  This also sets subcontexts'
        // names, to the given name + "." + the name they have been
        // instantiated by in this context.
        void setName( const std::string& name );

    private:
        VariableDataSource<std::string>::shared_ptr nameds;

        SubContextNameMap subcontexts;

        StateNameMap states;

        VisibleWritableValuesMap parametervalues;

        VisibleReadOnlyValuesMap visiblereadonlyvalues;
    };
}

#endif

/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:32:42 CEST 2004  ParsedStateContext.cxx

                        ParsedStateContext.cxx -  description
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

#include "execution/ParsedStateContext.hpp"
#include "execution/DataSource.hpp"
#include "execution/Processor.hpp"
#include "execution/StateDescription.hpp"
#include "execution/ParsedValue.hpp"
#include "execution/mystd.hpp"

namespace ORO_Execution {

    using ORO_CoreLib::ConditionInterface;

    std::vector<std::string> ParsedStateContext::getSubContextList() const {
        return mystd::keys( subcontexts );
    }

    ParsedStateContext* ParsedStateContext::getSubContext( const std::string& name ) const {
        SubContextNameMap::const_iterator i = subcontexts.find( name );
        if ( i == subcontexts.end() )
            return 0;
        assert( dynamic_cast<ParsedStateContext*>( i->second->get() ) );
        return static_cast<ParsedStateContext*>( i->second->get() );
    }

    ParsedStateContext* ParsedStateContext::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const {
        std::map<const StateInterface*, StateDescription*> statemapping;
        ParsedStateContext* ret = new ParsedStateContext( this->getText() );

        ret->nameds = nameds->copy( replacements );

        for ( VisibleWritableValuesMap::const_iterator i = parametervalues.begin();
              i != parametervalues.end(); ++i )
        {
          ParsedValueBase* npvb = i->second->copy( replacements );
          ret->parametervalues[i->first] = npvb;
          replacements[i->second->toDataSource()] = npvb->toDataSource();
        }

        for ( SubContextNameMap::const_iterator i = subcontexts.begin(); i != subcontexts.end(); ++i )
        {
            // we first copy the subcontexts, and add the datasources
            // containing their pointers to the replacements map, so
            // that the new commands will work on the correct
            // contexts...
            assert( dynamic_cast<ParsedStateContext*>(  i->second->get() ) );
            ParsedStateContext* oldcontext = static_cast<ParsedStateContext*>(  i->second->get() );
            ParsedStateContext* newcontext = oldcontext->copy( replacements );
            DataSource<StateContextTree*>::shared_ptr ncds = new VariableDataSource<StateContextTree*>( newcontext );
            ret->subcontexts[i->first] = ncds;
            ret->addChild( newcontext ); // also copy tree info to StateContextTree !
            newcontext->setParent( ret );
            replacements[i->second.get()] = ncds.get();
        }

        // First make a copy of all states.  All states are either
        // known by their name or by a transition from or to them...
        for ( TransitionMap::const_iterator i = stateMap.begin(); i != stateMap.end(); ++i )
        {
            assert( dynamic_cast<StateDescription*>( i->first ) );
            StateDescription* fromState = static_cast<StateDescription*>( i->first );
            if( statemapping.find( fromState ) == statemapping.end() )
                statemapping[fromState] = fromState->copy( replacements );
            for ( TransList::const_iterator j = i->second.begin(); j != i->second.end(); ++j )
            {
                assert( dynamic_cast<StateDescription*>( j->get<1>() ) );
                StateDescription* toState = static_cast<StateDescription*>( j->get<1>() );
                if( statemapping.find( toState ) == statemapping.end() )
                    statemapping[toState] = toState->copy( replacements );
            }
        }
        for ( StateNameMap::const_iterator i = states.begin(); i != states.end(); ++i )
            if ( statemapping.find( i->second ) == statemapping.end() )
                statemapping[i->second] = i->second->copy( replacements );

        // now link the names to the new states
        for ( StateNameMap::const_iterator i = states.begin(); i != states.end(); ++i )
        {
            assert( statemapping.find( i->second ) != statemapping.end() );
            ret->states[i->first] = statemapping[i->second];
        }

        // next, copy the transitions
        for ( TransitionMap::const_iterator i = stateMap.begin(); i != stateMap.end(); ++i )
        {
            assert( statemapping.find( i->first ) != statemapping.end() );
            StateInterface* fromState = statemapping[i->first];
            for ( TransList::const_iterator j = i->second.begin(); j != i->second.end(); ++j )
            {
                ConditionInterface* condition = j->get<0>()->copy( replacements );
                assert( statemapping.find( j->get<1>() ) != statemapping.end() );
                StateInterface* toState = statemapping[j->get<1>()];
                int rank = j->get<2>();
                ret->stateMap[fromState].push_back( boost::make_tuple( condition, toState, rank ) );
            }
        }

        for ( VisibleReadOnlyValuesMap::const_iterator i = visiblereadonlyvalues.begin();
              i != visiblereadonlyvalues.end(); ++i )
        {
          ret->visiblereadonlyvalues[i->first] = i->second->copy( replacements );
        }

        ret->finistate = statemapping[finistate];
        ret->initstate = statemapping[initstate];
        ret->current = statemapping[current];

        return ret;
    }

    ParsedStateContext::~ParsedStateContext() {
        // we own our states...
        for ( StateNameMap::iterator i = states.begin();
              i != states.end(); ++i )
            delete i->second;
        // we own our subcontexts...
        for ( SubContextNameMap::iterator i = subcontexts.begin();
              i != subcontexts.end(); ++i )
            delete i->second->get();
    }

    std::vector<std::string> ParsedStateContext::getStateList() const {
        return mystd::keys( states );
    }

    StateDescription* ParsedStateContext::getState( const std::string& name ) const{
        StateNameMap::const_iterator i = states.find( name );
        if ( i == states.end() )
            return 0;
        else return i->second;
    }

    ParsedStateContext::ParsedStateContext()
        : StateContextTree( 0 ) // no parent
    {
        nameds = new VariableDataSource<std::string>( "" );
    };

    ParsedStateContext::ParsedStateContext(const std::string& text)
        : StateContextTree( 0 ) // no parent
          ,_text(text)
    {
        nameds = new VariableDataSource<std::string>( "" );
    };

    void ParsedStateContext::addState( const std::string& name, StateDescription* state ) {
        assert( states.find( name ) == states.end() );
        states[name] = state;
    }

    DataSource<StateContextTree*>* ParsedStateContext::addSubContext( const std::string& name, ParsedStateContext* sc ) {
        assert( subcontexts.find( name ) == subcontexts.end() );
        DataSource<StateContextTree*>* newds = new VariableDataSource<StateContextTree*>( sc );
        subcontexts[name] = newds;
        this->addChild( sc );
        sc->setParent( this );
        return newds;
    }

    void ParsedStateContext::addReadOnlyVar( const std::string& name, DataSourceBase* var )
    {
        assert( visiblereadonlyvalues.find( name ) == visiblereadonlyvalues.end() );
        visiblereadonlyvalues[name] = var;
    }
    void ParsedStateContext::addParameter( const std::string& name, ParsedValueBase* var )
    {
        assert( parametervalues.find( name ) == parametervalues.end() );
        parametervalues[name] = var;
        // every parameter is also a readonly var...
        visiblereadonlyvalues[name] = var->toDataSource();
    }
    DataSourceBase* ParsedStateContext::getReadOnlyVar( const std::string& name ) const
    {
        if( visiblereadonlyvalues.find( name ) == visiblereadonlyvalues.end() )
            return 0;
        return visiblereadonlyvalues.find(name)->second.get();
    }
    ParsedValueBase* ParsedStateContext::getParameter( const std::string& name ) const
    {
        if( parametervalues.find( name ) == parametervalues.end() )
            return 0;
        return parametervalues.find(name)->second;
    }

    ParsedStateContext::VisibleWritableValuesMap ParsedStateContext::getParameters() const
    {
        return parametervalues;
    }

    ParsedStateContext::VisibleReadOnlyValuesMap ParsedStateContext::getReadOnlyValues() const
    {
        return visiblereadonlyvalues;
    }

    std::vector<std::string> ParsedStateContext::getParameterNames() const
    {
        return mystd::keys( parametervalues );
    }

    std::vector<std::string> ParsedStateContext::getReadOnlyValuesNames() const
    {
        return mystd::keys( visiblereadonlyvalues );
    }

    bool ParsedStateContext::inState( const std::string& name ) const
    {
        assert( getState( name ) != 0 );
        return currentState() == getState( name );
    }

    DataSource<std::string>* ParsedStateContext::getNameDS() const
    {
        return nameds.get();
    }

    void ParsedStateContext::setName( const std::string& name )
    {
        this->_name = name;
        nameds->set( name );
        for ( SubContextNameMap::iterator i = subcontexts.begin(); i != subcontexts.end(); ++i )
        {
            std::string subname = name + "." + i->first;
            ParsedStateContext* psc = static_cast<ParsedStateContext*>( i->second->get() );
            psc->setName( subname );
        }
    }
}

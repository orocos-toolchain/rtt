/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  EventService.cxx 

                        EventService.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#include "rtt/EventService.hpp"

#include "rtt/ExecutionEngine.hpp"
#include <rtt/CompletionProcessor.hpp>



namespace RTT
{
    //EventService EventService::Global(CompletionProcessor::Instance()->getEventProcessor());

    EventService::EventService( ExecutionEngine* ee ) : eeproc(ee), eproc( 0 ) {}
    EventService::EventService( EventProcessor* ep ) : eeproc(0), eproc( ep ) {} // ep may be null.

    EventProcessor* EventService::getEventProcessor() {
        return eeproc ? eeproc->getEventProcessor()
            : (eproc ? eproc : CompletionProcessor::Instance()->getEventProcessor());
    }

    bool EventService::hasEvent(const std::string& ename) const
    {
        if ( fact.count(ename) == 0 )
            return false;
        return true;
    }

    std::vector<std::string> EventService::getEvents() const
    {
        std::vector<std::string> elist;
        Factories::const_iterator fit = fact.begin();
        while ( fit != fact.end() )
            {
                elist.push_back( fit->first );
                ++fit;
            }
        return elist;
    }

    int EventService::arity(const std::string& name) const {
        if ( this->hasEvent(name) == false )
            return -1;
        return fact.find(name)->second->arity();
    }
            
    bool EventService::removeEvent( const std::string& ename ) {
        Factories::iterator it = fact.find(ename);
        if ( it == fact.end()  )
            return false;
        delete it->second;
        fact.erase(it);
        return true;
    }
        
    EventService::~EventService() {
        for (Factories::iterator it = fact.begin(); it !=fact.end(); ++it )
            delete it->second;
    }

    EventC EventService::setupEmit(const std::string& ename) const {
        return EventC(this, ename);
    }

    ConnectionC EventService::setupConnection(const std::string& ename) const {
        return ConnectionC(this, ename );
    }

    Handle EventService::setupSyn(const std::string& ename,
                                               boost::function<void(void)> func,          
                                               std::vector<DataSourceBase::shared_ptr> args ) const {
        if ( fact.count(ename) != 1 )
            return Handle(); // empty handle.
        detail::EventHookBase* ehi = fact.find(ename)->second->createReceptor( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupSyn( func ); 
    }
        
    Handle EventService::setupAsyn(const std::string& ename,
                                                boost::function<void(void)> afunc,          
                                                const std::vector<DataSourceBase::shared_ptr>& args,
                                                ActivityInterface* t,
                                                EventProcessor::AsynStorageType s_type) const {
        return this->setupAsyn(ename, afunc, args, t->getEventProcessor(), s_type );
    }
        
    Handle EventService::setupAsyn(const std::string& ename,
                                                boost::function<void(void)> afunc,          
                                                const std::vector<DataSourceBase::shared_ptr>& args,
                                                EventProcessor* ep /* = CompletionProcessor::Instance()->getEventProcessor()*/,
                                                EventProcessor::AsynStorageType s_type) const {
        if ( fact.count(ename) != 1 )
            return Handle(); // empty handle.
        detail::EventHookBase* ehi = fact.find(ename)->second->createReceptor( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupAsyn( afunc, ep, s_type ); 
    }

    Handle EventService::setupSynAsyn(const std::string& ename,
                                                   boost::function<void(void)> sfunc,
                                                   boost::function<void(void)> afunc,
                                                   const std::vector<DataSourceBase::shared_ptr>& args,
                                                   ActivityInterface* t,
                                                   EventProcessor::AsynStorageType s_type) const {
        return this->setupSynAsyn(ename, sfunc, afunc, args, t->getEventProcessor(), s_type );
    }

    Handle EventService::setupSynAsyn(const std::string& ename,
                                                   boost::function<void(void)> sfunc,
                                                   boost::function<void(void)> afunc,
                                                   const std::vector<DataSourceBase::shared_ptr>& args,
                                                   EventProcessor* ep /*= CompletionProcessor::Instance()->getEventProcessor()*/,
                                                   EventProcessor::AsynStorageType s_type) const {
        if ( fact.count(ename) != 1 )
            return Handle(); // empty handle.
        detail::EventHookBase* ehi = fact.find(ename)->second->createReceptor( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupSynAsyn( sfunc, afunc, ep, s_type ); 
    }

    DataSourceBase::shared_ptr EventService::setupEmit(const std::string& ename,const std::vector<DataSourceBase::shared_ptr>& args) const
    {
        if ( fact.count(ename) != 1 )
            return DataSourceBase::shared_ptr();

        DataSourceBase::shared_ptr result( fact.find(ename)->second->createEmittor( args ) );

        return result;
    }


}

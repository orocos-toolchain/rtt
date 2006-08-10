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
    using namespace boost;

    EventService::EventService( ExecutionEngine* ee ) : eeproc(ee), eproc( 0 ) {}
    EventService::EventService( EventProcessor* ep ) : eeproc(0), eproc( ep ) {} // ep may be null.

    EventProcessor* EventService::getEventProcessor() {
        return eeproc ? eeproc->getEventProcessor()
            : (eproc ? eproc : CompletionProcessor::Instance());
    }

    bool EventService::hasEvent(const std::string& ename) const
    {
        return this->hasMember(ename);
    }

    std::vector<std::string> EventService::getEvents() const
    {
        return this->getNames();
    }

    int EventService::arity(const std::string& name) const
    {
        return this->getArity(name);
    }
            
    bool EventService::removeEvent( const std::string& ename ) {
        if ( mevents.count(ename) == 0 )
            return false;
        // first erase the hook.
        for (Hooks::iterator it = mhooks.begin(); it !=mhooks.end(); ++it ) {
            if ( it->first == ename ) {
                mhooks.erase( it );
                break;
            }
        }
        // next erase the emittor.
        mevents.erase( mevents.find( ename ) );
        // next erase the factory, if any:
        this->remove( ename );
        return true;
    }
        
    EventService::~EventService() {
        for (Hooks::iterator it = mhooks.begin(); it !=mhooks.end(); ++it )
            delete it->second;
    }

    EventC EventService::setupEmit(const std::string& ename) const {
        if ( !this->hasMember(ename) ) {
            log(Error) << "Can not initialize EventC for '"<<ename<<"': no such Event."<<endlog();
            return EventC(); // empty handle.
        }
        return EventC(this, ename);
    }

    ConnectionC EventService::setupConnection(const std::string& ename) const {
        if ( mhooks.count(ename) != 1 ) {
            log(Error) << "Can not initialize ConnectionC for '"<<ename<<"': no such Event."<<endlog();
            return ConnectionC(); // empty handle.
        }
        return ConnectionC(this, ename );
    }

    Handle EventService::setupSyn(const std::string& ename,
                                               boost::function<void(void)> func,          
                                               std::vector<DataSourceBase::shared_ptr> args ) const {
        if ( mhooks.count(ename) != 1 ) {
            log(Error) << "Can not create connection to '"<<ename<<"': no such Event."<<endlog();
            return Handle(); // empty handle.
        }
        detail::EventHookBase* ehi = mhooks.find(ename)->second->produce( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupSyn( func ); 
    }
        
    Handle EventService::setupAsyn(const std::string& ename,
                                                boost::function<void(void)> afunc,          
                                                const std::vector<DataSourceBase::shared_ptr>& args,
                                                EventProcessor* ep /* = CompletionProcessor::Instance()->getEventProcessor()*/,
                                                EventProcessor::AsynStorageType s_type) const {
        if ( mhooks.count(ename) != 1 ) {
            log(Error) << "Can not create connection to '"<<ename<<"': no such Event."<<endlog();
            return Handle(); // empty handle.
        }
        detail::EventHookBase* ehi = mhooks.find(ename)->second->produce( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupAsyn( afunc, ep, s_type ); 
    }

    ActionInterface* EventService::getEvent(const std::string& ename,const std::vector<DataSourceBase::shared_ptr>& args) const
    {
        return this->produce(ename, args);
    }


}

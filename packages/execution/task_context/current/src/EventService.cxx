#include "execution/EventService.hpp"

#include "execution/ExecutionEngine.hpp"
#include <corelib/CompletionProcessor.hpp>

using namespace ORO_CoreLib;

namespace ORO_Execution
{
    //EventService EventService::Global(CompletionProcessor::Instance()->getEventProcessor());

    EventService::EventService( ExecutionEngine* ee ) : eeproc(ee), eproc( 0 ) {}
    EventService::EventService( EventProcessor* ep ) : eeproc(0), eproc( ep ) {} // ep may be null.

    ORO_CoreLib::EventProcessor* EventService::getEventProcessor() {
        return eeproc ? eeproc->getEventProcessor()
            : (eproc ? eproc : CompletionProcessor::Instance()->getEventProcessor());
    }

    bool EventService::hasEvent(const std::string& ename) const
    {
        if ( fact.count(ename) == 0 )
            return false;
        return true;
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

    ConnectionC EventService::setupSynConnection(const std::string& ename,
                                                  boost::function<void(void)> func) const {
        return ConnectionC(this, ename, func, 0 );
    }

    ConnectionC EventService::setupAsynConnection(const std::string& ename,
                                                   boost::function<void(void)> func,
                                                   ORO_CoreLib::EventProcessor* ep) const {
        return ConnectionC(this, ename, func, ep ); 
    }

    ORO_CoreLib::Handle EventService::setupSyn(const std::string& ename,
                                               boost::function<void(void)> func,          
                                               std::vector<DataSourceBase::shared_ptr> args ) const {
        if ( fact.count(ename) != 1 )
            return ORO_CoreLib::Handle(); // empty handle.
        detail::EventHookBase* ehi = fact.find(ename)->second->createReceptor( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupSyn( func ); 
    }
        
    ORO_CoreLib::Handle EventService::setupAsyn(const std::string& ename,
                                                boost::function<void(void)> afunc,          
                                                const std::vector<DataSourceBase::shared_ptr>& args,
                                                ORO_CoreLib::TaskInterface* t) const {
        return this->setupAsyn(ename, afunc, args, t->getEventProcessor() );
    }
        
    ORO_CoreLib::Handle EventService::setupAsyn(const std::string& ename,
                                                boost::function<void(void)> afunc,          
                                                const std::vector<DataSourceBase::shared_ptr>& args,
                                                ORO_CoreLib::EventProcessor* ep /* = ORO_CoreLib::CompletionProcessor::Instance()->getEventProcessor()*/ ) const {
        if ( fact.count(ename) != 1 )
            return ORO_CoreLib::Handle(); // empty handle.
        detail::EventHookBase* ehi = fact.find(ename)->second->createReceptor( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupAsyn( afunc, ep ); 
    }

    ORO_CoreLib::Handle EventService::setupSynAsyn(const std::string& ename,
                                                   boost::function<void(void)> sfunc,
                                                   boost::function<void(void)> afunc,
                                                   const std::vector<DataSourceBase::shared_ptr>& args,
                                                   ORO_CoreLib::TaskInterface* t) const {
        return this->setupSynAsyn(ename, sfunc, afunc, args, t->getEventProcessor() );
    }

    ORO_CoreLib::Handle EventService::setupSynAsyn(const std::string& ename,
                                                   boost::function<void(void)> sfunc,
                                                   boost::function<void(void)> afunc,
                                                   const std::vector<DataSourceBase::shared_ptr>& args,
                                                   ORO_CoreLib::EventProcessor* ep /*= ORO_CoreLib::CompletionProcessor::Instance()->getEventProcessor()*/ ) const {
        if ( fact.count(ename) != 1 )
            return ORO_CoreLib::Handle(); // empty handle.
        detail::EventHookBase* ehi = fact.find(ename)->second->createReceptor( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupSynAsyn( sfunc, afunc, ep ); 
    }

    DataSourceBase::shared_ptr EventService::setupEmit(const std::string& ename,const std::vector<DataSourceBase::shared_ptr>& args) const
    {
        if ( fact.count(ename) != 1 )
            return DataSourceBase::shared_ptr();

        DataSourceBase::shared_ptr result( fact.find(ename)->second->createEmittor( args ) );

        return result;
    }


}

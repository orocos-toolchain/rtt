#ifndef EVENT_SERVICE_HPP
#define EVENT_SERVICE_HPP

#include "TemplateEventFactory.hpp"

namespace ORO_Execution
{

    namespace detail {

        /**
         * Factory interface creating intermediate EventHookBase objects.
         */
        struct EventStubInterface
        {
            virtual ~EventStubInterface() {}
            virtual EventHookBase* createReceptor(const std::vector<DataSourceBase::shared_ptr>& args ) = 0;
            virtual DataSourceBase* createEmittor(const std::vector<DataSourceBase::shared_ptr>& args ) = 0;
        };

    
        template< class EventT>
        struct EventStub :
            public EventStubInterface
        {
            EventT* me;
            detail::TemplateFactoryPart<EventT, EventHookBase*>*  mrfact;
            detail::TemplateFactoryPart<EventT, DataSourceBase*>* mefact;
            EventStub( EventT* e,
                       detail::TemplateFactoryPart<EventT, EventHookBase*>* rfact,
                       detail::TemplateFactoryPart<EventT, DataSourceBase*>* efact)
                : me(e), mrfact(rfact), mefact(efact) {}
            virtual ~EventStub() {
                delete mrfact;
                delete mefact;
            }

            virtual EventHookBase* createReceptor(const std::vector<DataSourceBase::shared_ptr>& args ) {
                return mrfact->produce( me, args );
            }

            virtual DataSourceBase* createEmittor(const std::vector<DataSourceBase::shared_ptr>& args ) {
                return mefact->produce( me, args );
            }
        };

    }


    /**
     * Stores Events of any type and connects SYN/ASYN Event handlers to them.
     * 
     */
    class EventService
    {
        typedef std::map<std::string, detail::EventStubInterface* > Factories;
        Factories fact;
        
    public:
        /**
         * Add an arbitrary Event to this Service.
         */
        template< class EventT>
        bool addEvent( const std::string& ename, EventT* e ) {
            if ( fact.count(ename) != 0 )
                return false;
            fact[ename] = new detail::EventStub<EventT>( e,
                                                         detail::EventHookFactoryGenerator<EventT>().receptor(),
                                                         detail::EventHookFactoryGenerator<EventT>().emittor() );
            return true;
        }

        /**
         * Query for the existence of an Event in this Service.
         */
        bool hasEvent(const std::string& ename)
        {
            if ( fact.count(ename) == 0 )
                return false;
            return true;
        }

        /**
         * Add an added Event from this Service.
         */
        bool removeEvent( const std::string& ename ) {
            Factories::iterator it = fact.find(ename);
            if ( it == fact.end()  )
                return false;
            delete it->second;
            fact.erase(it);
            return true;
        }
        
        ~EventService() {
            for (Factories::iterator it = fact.begin(); it !=fact.end(); ++it )
                delete it->second;
        }

        /**
         * Setup a synchronous Event handler which will set \a args and
         * then call \a func synchronously when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param func  A function object which will be called.
         * @param args  The arguments which will be set before \a func is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         */
        ORO_CoreLib::Handle setupSyn(const std::string& ename,
                                boost::function<void(void)> func,          
                                std::vector<DataSourceBase::shared_ptr> args ) {
            if ( fact.count(ename) != 1 )
                return ORO_CoreLib::Handle(); // empty handle.
            detail::EventHookBase* ehi = fact[ename]->createReceptor( args );

            // ehi is stored _inside_ the connection object !
            return ehi->setupSyn( func ); 
        }
        
        /**
         * Setup a asynchronous Event handler which will set \a args and
         * call \a afunc asynchronously (in task \a t) when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param afunc  A function object which will be called.
         * @param args  The arguments which will be set before \a afunc is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         * @param t The task in which the \a args will be set and \a afunc will be called.
         * @param ep The EventProcessor in which the \a args will be set and \a afunc will be called.
         * @{
         */
        ORO_CoreLib::Handle setupAsyn(const std::string& ename,
                                      boost::function<void(void)> afunc,          
                                      const std::vector<DataSourceBase::shared_ptr>& args,
                                      ORO_CoreLib::TaskInterface* t) {
            return this->setupAsyn(ename, afunc, args, t->getEventProcessor() );
        }
        
        ORO_CoreLib::Handle setupAsyn(const std::string& ename,
                                      boost::function<void(void)> afunc,          
                                      const std::vector<DataSourceBase::shared_ptr>& args,
                                      ORO_CoreLib::EventProcessor* ep = ORO_CoreLib::CompletionProcessor::Instance()->getEventProcessor() ) {
            if ( fact.count(ename) != 1 )
                return ORO_CoreLib::Handle(); // empty handle.
            detail::EventHookBase* ehi = fact[ename]->createReceptor( args );

            // ehi is stored _inside_ the connection object !
            return ehi->setupAsyn( afunc, ep ); 
        }
        //!@}
        
        /**
         * Setup a synchronous and asynchronous Event handler which will set \a args and
         * call \a sfunc synchronously, and call \a afunc asynchronously (in task \a t)
         * when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param sfunc  A function object which will be called synchronously
         * @param afunc  A function object which will be called asynchronously
         * @param args  The arguments which will be set before \a sfunc
         * \em and before \a afunc is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         * @param t The task in which the \a args will be set and \a afunc will be called.
         * @param ep The EventProcessor in which the \a args will be set and \a afunc will be called.
         * @{
         */
        ORO_CoreLib::Handle setupSynAsyn(const std::string& ename,
                                    boost::function<void(void)> sfunc,
                                    boost::function<void(void)> afunc,
                                    const std::vector<DataSourceBase::shared_ptr>& args,
                                    ORO_CoreLib::TaskInterface* t) {
            return this->setupSynAsyn(ename, sfunc, afunc, args, t->getEventProcessor() );
        }

        ORO_CoreLib::Handle setupSynAsyn(const std::string& ename,
                                    boost::function<void(void)> sfunc,
                                    boost::function<void(void)> afunc,
                                    const std::vector<DataSourceBase::shared_ptr>& args,
                                    ORO_CoreLib::EventProcessor* ep = ORO_CoreLib::CompletionProcessor::Instance()->getEventProcessor() ) {
            if ( fact.count(ename) != 1 )
                return ORO_CoreLib::Handle(); // empty handle.
            detail::EventHookBase* ehi = fact[ename]->createReceptor( args );

            // ehi is stored _inside_ the connection object !
            return ehi->setupSynAsyn( sfunc, afunc, ep ); 
        }
        //! @}

        /**
         * Setup an Event::emit() invocation wrapped in a DataSource.
         * Call \a result.evaluate() to emit the event with the given \a args.
         *
         * @param ename The name of the previously added Event.
         * @param args  DataSources holding the values for each parameter of the event's emit().
         * They be read at the moment of emit().
         * @see DataSourceGenerator for creating the arguments from variables or plain literals.
         */
        DataSourceBase::shared_ptr setupEmit(const std::string& ename,const std::vector<DataSourceBase::shared_ptr>& args)
        {
            if ( fact.count(ename) != 1 )
                return DataSourceBase::shared_ptr();

            DataSourceBase::shared_ptr result( fact[ename]->createEmittor( args ) );

            return result;
        }

        /**
         * The Global EventService, in which global Events are stored.
         */
        static EventService Global;
    };


}

#endif

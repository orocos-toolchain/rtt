#ifndef EVENT_SERVICE_HPP
#define EVENT_SERVICE_HPP

#include "Event.hpp"

namespace ORO_Execution
{

    namespace detail {
        using ORO_CoreLib::Handle;

        /**
         * Create end-user event handles.
         * An EventHook is dedicated to \b 1 Event with \b 1 SYN and/or ASYN
         * event receptors. When the connection object is destroyed,
         * this hook is destroyed. A smart pointer trick makes sure
         * that connection will clean us up during its destruction.
         */
        struct EventHookBase
        {
            virtual ~EventHookBase() {}

            Handle setupSyn(boost::function<void(void)> sfunc ) {
                msfunc = sfunc;
                return msetupSyn();
            }

            Handle setupAsyn(boost::function<void(void)> afunc ) {
                mafunc = afunc;
                return msetypAsyn();
            }

            Handle setupSynAsyn(boost::function<void(void)> sfunc, boost::function<void(void)> afunc ) {
                msfunc = sfunc;
                mafunc = afunc;
                return msetupSynAsyn();
            }
        protected:
            boost::function<void(void)> msfunc;
            boost::function<void(void)> mafunc;

            virtual Handle msetupSyn( ) = 0;

            virtual Handle msetupAsyn( ) = 0;

            virtual Handle msetupSynAsyn( ) = 0;
        };

        template<typename EventT>
        struct EventHook0 : public EventHookBase
        {
        protected:
            // The default copy constructor is ok for this class,
            // a copy is equivalent to the original.
            typedef EventT::SlotFunction::result_type Ret;
            typedef EventHook0<EventT> This;
            EventT* msource;

            //boost::shared_ptr<EventHookBase> seh;
            
        public:
            EventHook0( EventT* source )
                : msource(source) //, seh(this)
            {}
        protected:
            Handle msetupSyn( ) {
                Handle h = msource->setup( boost::bind(This::synop,boost::shared_ptr<This>(this)) );
                //seh = 0;
                return h;
            }

            Handle msetupAsyn( ) {
                Handle h = msource->setup( boost::bind(This::asynop,boost::shared_ptr<This>(this)), t );
                //seh = 0;
                return h;
            }

            Handle msetupSynAsyn( ) {
                boost::shared_ptr<This> seh(this);
                Handle h = msource->setup( boost::bind(This::synop,seh), boost::bind(This::asynop,seh), t );
                seh = 0;
                return h;
            }

            Ret synop() 
            {
                msfunc();
                return Ret();
            }

            Ret asynop() 
            {
                mafunc();
                return Ret();
            }
        };

        template<typename EventT>
        struct EventHook1 : public EventHookBase
        {
        protected:
            // The default copy constructor is ok for this class,
            // a copy is equivalent to the original.
            typedef EventT::SlotFunction::arg1_type A1;
            typedef EventT::SlotFunction::result_type Ret;
            typedef EventHook1<EventT> This;
            EventT* msource;
            AssignableDataSource<A1>::shared_ptr ma1;

            //boost::shared_ptr<EventHookBase> seh;
            
        public:
            EventHook1( EventT* source, AssignableDataSource<A1>::shared_ptr a1 )
                : msource(source), ma1(a1) //, seh(this)
            {
            }
        protected:
            Handle msetupSyn( ) {
                Handle h = msource->setup( boost::bind(This::synop,boost::shared_ptr<This>(this),_1) );
                //seh = 0;
                return h;
            }

            Handle msetupAsyn( ) {
                Handle h = msource->setup( boost::bind(This::asynop,boost::shared_ptr<This>(this),_1), t );
                //seh = 0;
                return h;
            }

            Handle msetupSynAsyn( ) {
                boost::shared_ptr<This> seh(this);
                Handle h = msource->setup( boost::bind(This::synop,seh,_1), boost::bind(This::asynop,seh,_1), t );
                seh = 0;
                return h;
            }

            Ret synop(A1 arg1) 
            {
                // set the received args.
                ma1->set(arg1);
                msfunc();
                return Ret();
            }

            Ret asynop(A1 arg1) 
            {
                // set the received args.
                ma1->set(arg1);
                mafunc();
                return Ret();
            }
        };

        template<typename EventT>
        struct EventHook2 : public EventHookBase
        {
        protected:
            // The default copy constructor is ok for this class,
            // a copy is equivalent to the original.
            typedef EventT::SlotFunction::arg1_type A1;
            typedef EventT::SlotFunction::arg2_type A2;
            typedef EventT::SlotFunction::result_type Ret;
            typedef EventHook2<EventT> This;
            EventT* msource;
            AssignableDataSource<A1>::shared_ptr ma1;
            AssignableDataSource<A2>::shared_ptr ma2;

            //boost::shared_ptr<EventHookBase> seh;
            
        public:
            EventHook2( EventT* source,
                        AssignableDataSource<A1>::shared_ptr a1,
                        AssignableDataSource<A2>::shared_ptr a2)
                : msource(source), ma1(a1), ma2(a2) //, seh(this)
            {
            }
        protected:
            Handle msetupSyn( ) {
                Handle h = msource->setup( boost::bind(This::synop,boost::shared_ptr<This>(this),_1,_2) );
                //seh = 0;
                return h;
            }

            Handle msetupAsyn( ) {
                Handle h = msource->setup( boost::bind(This::asynop,boost::shared_ptr<This>(this),_1,_2), t );
                //seh = 0;
                return h;
            }

            Handle msetupSynAsyn( ) {
                boost::shared_ptr<This> seh(this);
                Handle h = msource->setup( boost::bind(This::synop,seh,_1,_2), boost::bind(This::asynop,seh,_1,_2), t );
                seh = 0;
                return h;
            }

            Ret synop(A1 arg1, A2 arg2) 
            {
                // set the received args.
                ma1->set(arg1);
                ma2->set(arg2);
                msfunc();
                return Ret();
            }
            Ret asynop(A1 arg1, A2 arg2) 
            {
                // set the received args.
                ma1->set(arg1);
                ma2->set(arg2);
                mafunc();
                return Ret();
            }
        };

        template<typename EventT>
        struct EventHook3 : public EventHookBase
        {
        protected:
            // The default copy constructor is ok for this class,
            // a copy is equivalent to the original.
            typedef EventT::SlotFunction::arg1_type A1;
            typedef EventT::SlotFunction::arg2_type A2;
            typedef EventT::SlotFunction::arg3_type A3;
            typedef EventT::SlotFunction::result_type Ret;
            typedef EventHook3<EventT> This;

            EventT* msource;
            AssignableDataSource<A1>::shared_ptr ma1;
            AssignableDataSource<A2>::shared_ptr ma2
            AssignableDataSource<A3>::shared_ptr ma3;

            //boost::shared_ptr<EventHookBase> seh;
            
        public:
            EventHook3( EventT* source,
                        AssignableDataSource<A1>::shared_ptr a1,
                        AssignableDataSource<A2>::shared_ptr a2,
                        AssignableDataSource<A3>::shared_ptr a3)
                : msource(source), ma1(a1), ma2(a2), ma3(a3) //, seh(this)
            {
            }
        protected:
            Handle msetupSyn( ) {
                Handle h = msource->setup( boost::bind(This::synop,boost::shared_ptr<This>(this),_1,_2,_3) );
                //seh = 0;
                return h;
            }

            Handle msetupAsyn( ) {
                Handle h = msource->setup( boost::bind(This::asynop,boost::shared_ptr<This>(this),_1,_2,_3), t );
                //seh = 0;
                return h;
            }

            Handle msetupSynAsyn( ) {
                boost::shared_ptr<This> seh(this);
                Handle h = msource->setup( boost::bind(This::synop,seh,_1,_2,_3), boost::bind(This::asynop,seh,_1,_2,_3), t );
                seh = 0;
                return h;
            }

            Ret synop(A1 arg1, A2 arg2, A3 arg3) 
            {
                // set the received args.
                ma1->set(arg1);
                ma2->set(arg2);
                ma3->set(arg3);
                msfunc();
                return Ret();
            }
            Ret asynop(A1 arg1, A2 arg2, A3 arg3) 
            {
                // set the received args.
                ma1->set(arg1);
                ma2->set(arg2);
                ma3->set(arg3);
                mafunc();
                return Ret();
            }
        };

        /**
         * Factory interface creating intermediate EventHookBase objects.
         */
        struct EventStubInterface
        {
            virtual ~EventStubInterface() {}
            virtual EventHookBase* createReceptor(const vector<DataSourceBase::shared_ptr>& args ) = 0;
        };

    
        template< class EventT>
        struct EventStub :
            public EventStubInterface
        {
            EventT* me;
            detail::TemplateFactoryPart<EventT*, EventHookBase*> mfact;
            // the factory is copy-constructed !
            EventStub( EventT* e, detail::TemplateFactoryPart<EventT*, EventHookBase*> fact)
                : me(e), mfact(fact) {}

            virtual EventHookBase* createReceptor(const vector<DataSourceBase::shared_ptr>& args ) {
                return mfact->produce( me, args );
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
            fact[ename] = new detail::EventStub( e, detail::EventHookFactoryGenerator<EventT>()() );
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

        /**
         * Setup a synchronous Event handler which will set \a args and
         * then call \a func synchronously when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param func  A function object which will be called.
         * @param args  The arguments which will be set before \a func is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         */
        virtual Handle setupSyn(const std::string& ename,
                                boost::function<void(void)> func,          
                                vector<DataSourceBase::shared_ptr> args ) {
            if ( fact.count(ename) != 1 )
                return Handle(); // empty handle.
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
         */
        virtual Handle setupASyn(const std::string& ename,
                                 boost::function<void(void)> afunc,          
                                 vector<DataSourceBase::shared_ptr> args,
                                 TaskInterface* t) {
            if ( fact.count(ename) != 1 )
                return Handle(); // empty handle.
            detail::EventHookBase* ehi = fact[ename]->createReceptor( args );

            // ehi is stored _inside_ the connection object !
            return ehi->setupASyn( afunc, t ); 
        }
        
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
         */
        virtual Handle setupSynASyn(const std::string& ename,
                                    boost::function<void(void)> sfunc,
                                    boost::function<void(void)> afunc,
                                    vector<DataSourceBase::shared_ptr> args,
                                    TaskInterface* t) {
            if ( fact.count(ename) != 1 )
                return Handle(); // empty handle.
            detail::EventHookBase* ehi = fact[ename]->createReceptor( args );

            // ehi is stored _inside_ the connection object !
            return ehi->setupSynASyn( sfunc, afunc, t ); 
        }
        
    };


}

#endif

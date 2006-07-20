/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  TemplateEventFactory.hpp 

                        TemplateEventFactory.hpp -  description
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
 
 
#ifndef TEMPLATE_EVENT_FACTORY_HPP
#define TEMPLATE_EVENT_FACTORY_HPP
 

#include "Event.hpp"
#include "ActivityInterface.hpp"
#include "TemplateFactory.hpp"
#include "TemplateMethodFactory.hpp"

#include <boost/bind.hpp>
#include <boost/type_traits.hpp>


/**
 * @file This file contains the Template factories for Event objects
 *
 * This is an internal file, not to be used directly by users.
 */
namespace RTT{

    namespace detail {

        
        

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

            Handle setupAsyn(boost::function<void(void)> afunc, EventProcessor* t,EventProcessor::AsynStorageType s_type ) {
                mafunc = afunc;
                return msetupAsyn(t, s_type);
            }

            Handle setupSynAsyn(boost::function<void(void)> sfunc, boost::function<void(void)> afunc, EventProcessor* t,EventProcessor::AsynStorageType s_type ) {
                msfunc = sfunc;
                mafunc = afunc;
                return msetupSynAsyn(t, s_type);
            }
        protected:
            boost::function<void(void)> msfunc;
            boost::function<void(void)> mafunc;

            virtual Handle msetupSyn( ) = 0;

            virtual Handle msetupAsyn( EventProcessor* t,EventProcessor::AsynStorageType s_type ) = 0;

            virtual Handle msetupSynAsyn( EventProcessor* t,EventProcessor::AsynStorageType s_type ) = 0;
        };

        template<typename EventT>
        struct EventHook0 : public EventHookBase
        {
        protected:
            // The default copy constructor is ok for this class,
            // a copy is equivalent to the original.
            typedef typename EventT::SlotFunction::result_type Ret;
            typedef EventHook0<EventT> This;
            EventT* msource;

            //boost::shared_ptr<EventHookBase> seh;
            
        public:
            EventHook0( EventT* source )
                : msource(source) //, seh(this)
            {}
        protected:
            Handle msetupSyn( ) {
                Handle h = msource->setup( boost::bind(&This::synop,boost::shared_ptr<This>(this)) );
                //seh = 0;
                return h;
            }

            Handle msetupAsyn( EventProcessor* t,EventProcessor::AsynStorageType s_type ) {
                Handle h = msource->setup( boost::bind(&This::asynop,boost::shared_ptr<This>(this)), t, s_type );
                //seh = 0;
                return h;
            }

            Handle msetupSynAsyn( EventProcessor* t,EventProcessor::AsynStorageType s_type ) {
                boost::shared_ptr<This> seh(this);
                Handle h = msource->setup( boost::bind(&This::synop,seh), boost::bind(&This::asynop,seh), t, s_type );
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
            typedef typename EventT::SlotFunction::arg1_type A1;
            typedef typename EventT::SlotFunction::result_type Ret;
            typedef EventHook1<EventT> This;
            EventT* msource;
            typename AssignableDataSource<A1>::shared_ptr ma1;

            //boost::shared_ptr<EventHookBase> seh;
            
        public:
            EventHook1( EventT* source, typename AssignableDataSource<A1>::shared_ptr a1 )
                : msource(source), ma1(a1) //, seh(this)
            {
            }
        protected:
            Handle msetupSyn( ) {
                Handle h = msource->setup( boost::bind(&This::synop,boost::shared_ptr<This>(this),_1) );
                //seh = 0;
                return h;
            }

            Handle msetupAsyn( EventProcessor* t,EventProcessor::AsynStorageType s_type ) {
                Handle h = msource->setup( boost::bind(&This::asynop,boost::shared_ptr<This>(this),_1), t, s_type );
                //seh = 0;
                return h;
            }

            Handle msetupSynAsyn( EventProcessor* t, EventProcessor::AsynStorageType s_type ) {
                boost::shared_ptr<This> seh(this);
                Handle h = msource->setup( boost::bind(&This::synop,seh,_1), boost::bind(&This::asynop,seh,_1), t, s_type );
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
            typedef typename EventT::SlotFunction::arg1_type A1;
            typedef typename EventT::SlotFunction::arg2_type A2;
            typedef typename EventT::SlotFunction::result_type Ret;
            typedef EventHook2<EventT> This;
            EventT* msource;
            typename AssignableDataSource<A1>::shared_ptr ma1;
            typename AssignableDataSource<A2>::shared_ptr ma2;

            //boost::shared_ptr<EventHookBase> seh;
            
        public:
            EventHook2( EventT* source,
                        typename AssignableDataSource<A1>::shared_ptr a1,
                        typename AssignableDataSource<A2>::shared_ptr a2)
                : msource(source), ma1(a1), ma2(a2) //, seh(this)
            {
            }
        protected:
            Handle msetupSyn( ) {
                Handle h = msource->setup( boost::bind(&This::synop,boost::shared_ptr<This>(this),_1,_2) );
                return h;
            }

            Handle msetupAsyn( EventProcessor* t, EventProcessor::AsynStorageType s_type ) {
                Handle h = msource->setup( boost::bind(&This::asynop,boost::shared_ptr<This>(this),_1,_2), t, s_type );
                return h;
            }

            Handle msetupSynAsyn( EventProcessor* t,EventProcessor::AsynStorageType s_type ) {
                boost::shared_ptr<This> seh(this);
                Handle h = msource->setup( boost::bind(&This::synop,seh,_1,_2), boost::bind(&This::asynop,seh,_1,_2), t, s_type );
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
            typedef typename EventT::SlotFunction::arg1_type A1;
            typedef typename EventT::SlotFunction::arg2_type A2;
            typedef typename EventT::SlotFunction::arg3_type A3;
            typedef typename EventT::SlotFunction::result_type Ret;
            typedef EventHook3<EventT> This;

            EventT* msource;
            typename AssignableDataSource<A1>::shared_ptr ma1;
            typename AssignableDataSource<A2>::shared_ptr ma2;
            typename AssignableDataSource<A3>::shared_ptr ma3;

            //boost::shared_ptr<EventHookBase> seh;
            
        public:
            EventHook3( EventT* source,
                        typename AssignableDataSource<A1>::shared_ptr a1,
                        typename AssignableDataSource<A2>::shared_ptr a2,
                        typename AssignableDataSource<A3>::shared_ptr a3)
                : msource(source), ma1(a1), ma2(a2), ma3(a3) //, seh(this)
            {
            }
        protected:
            Handle msetupSyn( ) {
                Handle h = msource->setup( boost::bind(&This::synop,boost::shared_ptr<This>(this),_1,_2,_3) );
                return h;
            }

            Handle msetupAsyn( EventProcessor* t, EventProcessor::AsynStorageType s_type ) {
                Handle h = msource->setup( boost::bind(&This::asynop,boost::shared_ptr<This>(this),_1,_2,_3), t, s_type );
                return h;
            }

            Handle msetupSynAsyn( EventProcessor* t, EventProcessor::AsynStorageType s_type ) {
                boost::shared_ptr<This> seh(this);
                Handle h = msource->setup( boost::bind(&This::synop,seh,_1,_2,_3), boost::bind(&This::asynop,seh,_1,_2,_3), t, s_type );
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
         * Generate EventHook depending on number of arguments.
         */
        class EventHookGenerator
        {
        public:
            typedef EventHookBase* result_type;

            template<class EventT>
            EventHookBase* operator()( EventT* e ) const {
                return new EventHook0<EventT>(e); // called by TemplateFactoryPart0
            }

            template<class EventT, class Arg1T>
            EventHookBase* operator()( EventT* , Arg1T ) const
            {
                throw non_lvalue_args_exception( 1, DataSource<Arg1T>::GetType() );
            }

            template<class EventT, class Arg1T, class Arg2T>
            EventHookBase* operator()( EventT* , Arg1T, Arg2T ) const
            {
                throw non_lvalue_args_exception( 1, DataSource<Arg1T>::GetType() );
            }

            template<class EventT, class Arg1T, class Arg2T, class Arg3T>
            EventHookBase* operator()( EventT* , Arg1T, Arg2T, Arg3T ) const
            {
                throw non_lvalue_args_exception( 1, DataSource<Arg1T>::GetType() );
            }

            template<class EventT, class Arg1T>
            EventHookBase* operator()( EventT* e, DataSource<Arg1T>* arg1 ) const
            {
                // check if we can make an AssignableDS from arg1...
                typename AssignableDataSource<Arg1T>::shared_ptr ma1 = AdaptAssignableDataSource<Arg1T>()( arg1 );
                if ( !ma1 )
                    throw non_lvalue_args_exception( 1, arg1->getType() );

                // ok, deliver part to 'user'.
                return new EventHook1<EventT>(e, ma1); // called by TemplateFactoryPart1
            }

            template<class EventT, class Arg1T, class Arg2T>
            EventHookBase* operator()( EventT* e,
                                            DataSource<Arg1T>* arg1,
                                            DataSource<Arg2T>* arg2) const
            {
                // check if we can make an AssignableDS from arg1...
                typename AssignableDataSource<Arg1T>::shared_ptr ma1 = AdaptAssignableDataSource<Arg1T>()( arg1 );
                if ( !ma1 )
                    throw non_lvalue_args_exception( 1, arg1->getType() );
                typename AssignableDataSource<Arg2T>::shared_ptr ma2 = AdaptAssignableDataSource<Arg2T>()( arg2 );
                if ( !ma2 )
                    throw non_lvalue_args_exception( 2, arg2->getType() );

                // ok, deliver part to 'user'.
                return new EventHook2<EventT>(e, ma1, ma2); // called by TemplateFactoryPart2
            }

            template<class EventT, class Arg1T, class Arg2T, class Arg3T>
            EventHookBase* operator()( EventT* e,
                                            DataSource<Arg1T>* arg1,
                                            DataSource<Arg2T>* arg2,
                                            DataSource<Arg3T>* arg3) const
            {
                // check if we can make an AssignableDS from arg1...
                typename AssignableDataSource<Arg1T>::shared_ptr ma1 = AdaptAssignableDataSource<Arg1T>()( arg1 );
                if ( !ma1 )
                    throw non_lvalue_args_exception( 1, arg1->getType() );
                typename AssignableDataSource<Arg2T>::shared_ptr ma2 = AdaptAssignableDataSource<Arg2T>()( arg2 );
                if ( !ma2 )
                    throw non_lvalue_args_exception( 2, arg2->getType() );
                typename AssignableDataSource<Arg3T>::shared_ptr ma3 = AdaptAssignableDataSource<Arg3T>()( arg3 );
                if ( !ma3 )
                    throw non_lvalue_args_exception( 3, arg3->getType() );

                // ok, deliver part to 'user'.
                return new EventHook3<EventT>(e, ma1, ma2,ma3); // called by TemplateFactoryPart3
            }
        };

        template<class EventT, class Enable = void>
        struct EventHookFactoryGenerator
        {
            TemplateFactoryPart<EventT,EventHookBase*>* receptor() const {
                return new TemplateFactoryFunctorPart0<EventT,EventHookBase*,EventHookGenerator>( EventHookGenerator(), "Event Hook" );
            }
            TemplateFactoryPart<EventT,DataSourceBase*>* emittor() const {
                return method<EventT,typename EventT::emit_type>(&EventT::emit, "Event Emittor");
            }
        };
        
        
        template<class EventT>
        struct EventHookFactoryGenerator<EventT, typename boost::enable_if_c< EventT::SlotFunction::arity == 1 >::type >
        {
                TemplateFactoryPart<EventT,EventHookBase*>* receptor() const {
                    return new TemplateFactoryFunctorPart1<
                        EventT,
                        EventHookBase*,
                        EventHookGenerator,
                        typename EventT::SlotFunction::arg1_type>( EventHookGenerator(), "Event Hook", "arg1", "description" );
                }

                TemplateFactoryPart<EventT,DataSourceBase*>* emittor() const {
                    return method<EventT,typename EventT::emit_type, typename EventT::arg1_type>(&EventT::emit, "Event Emittor",
                                  "arg1", "description");
                }
        };

        template<class EventT>
        struct EventHookFactoryGenerator<EventT, typename boost::enable_if_c< EventT::SlotFunction::arity == 2 >::type >
        {
                TemplateFactoryPart<EventT,EventHookBase*>* receptor() const {
                    return new TemplateFactoryFunctorPart2<
                        EventT,
                        EventHookBase*,
                        EventHookGenerator,
                        typename EventT::SlotFunction::arg1_type,
                        typename EventT::SlotFunction::arg2_type>( EventHookGenerator(), "Event Hook",
                                                          "arg1", "description", "arg2", "description" );
                }

                TemplateFactoryPart<EventT,DataSourceBase*>* emittor() const {
                    return method<EventT,typename EventT::emit_type, typename EventT::arg1_type, typename EventT::arg2_type>(&EventT::emit, "Event Emittor",
                                  "arg1", "description",
                                  "arg2", "description");
                }
        };

        template<class EventT>
        struct EventHookFactoryGenerator<EventT, typename boost::enable_if_c< EventT::SlotFunction::arity == 3 >::type >
        {
                TemplateFactoryPart<EventT,EventHookBase*>* receptor() const {
                    return new TemplateFactoryFunctorPart3<
                        EventT,
                        EventHookBase*,
                        EventHookGenerator,
                        typename EventT::SlotFunction::arg1_type,
                        typename EventT::SlotFunction::arg2_type,
                        typename EventT::SlotFunction::arg3_type>( EventHookGenerator(), "Event Hook",
                                                          "arg1", "description", "arg2", "description",
                                                          "arg3", "description");
                }

                TemplateFactoryPart<EventT,DataSourceBase*>* emittor() const {
                    return method<EventT,typename EventT::emit_type, typename EventT::arg1_type, typename EventT::arg2_type, 
                        typename EventT::arg3_type>(&EventT::emit, "Event Emittor",
                                  "arg1", "description",
                                  "arg2", "description",
                                  "arg3", "description");
                }
        };
        // repeat for each additional argument... N=10 ?
    }
}
#endif

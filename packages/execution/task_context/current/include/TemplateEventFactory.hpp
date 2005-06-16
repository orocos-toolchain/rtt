#ifndef TEMPLATE_EVENT_FACTORY_HPP
#define TEMPLATE_EVENT_FACTORY_HPP
 

#include "Event.hpp"
#include "TemplateFactory.hpp"

#include <boost/bind.hpp>
#include <boost/type_traits.hpp>


/**
 * @file This file contains the Template factories for Event objects
 *
 * This is an internal file, not to be used directly by users.
 */
namespace ORO_Execution{

    namespace detail {

        /**
         * Generate EventHook depending on number of arguments.
         */
        class EventHookGenerator
        {
        public:
            template<class EventT>
            EventHookInterface* operator()( EventT* e ) {
                return new EventHook0(e); // called by TemplateFactoryPart0
            }

            template<class EventT, class Arg1T>
            EventHookInterface* operator()( EventT* e, DataSource<Arg1T>* arg1 )
            {
                // check if we can make an AssignableDS from arg1...
                AssignableDataSource<Arg1T>::shared_ptr ma1 = AdaptAssignableDataSource<Arg1T>()( arg1 );
                if ( !ma1 )
                    throw non_lvalue_args_exception( 1, arg1->getType() );

                // ok, deliver part to 'user'.
                return new EventHook1(e, ma1); // called by TemplateFactoryPart1
            }

            template<class EventT, class Arg1T, class Arg2T>
            EventHookInterface* operator()( EventT* e,
                                            DataSource<Arg1T>* arg1,
                                            DataSource<Arg2T>* arg2)
            {
                // check if we can make an AssignableDS from arg1...
                AssignableDataSource<Arg1T>::shared_ptr ma1 = AdaptAssignableDataSource<Arg1T>()( arg1 );
                if ( !ma1 )
                    throw non_lvalue_args_exception( 1, arg1->getType() );
                AssignableDataSource<Arg2T>::shared_ptr ma2 = AdaptAssignableDataSource<Arg2T>()( arg2 );
                if ( !ma2 )
                    throw non_lvalue_args_exception( 2, arg2->getType() );

                // ok, deliver part to 'user'.
                return new EventHook2(e, ma1, ma2); // called by TemplateFactoryPart2
            }

            template<class EventT, class Arg1T, class Arg2T, class Arg3T>
            EventHookInterface* operator()( EventT* e,
                                            DataSource<Arg1T>* arg1,
                                            DataSource<Arg2T>* arg2,
                                            DataSource<Arg3T>* arg3)
            {
                // check if we can make an AssignableDS from arg1...
                AssignableDataSource<Arg1T>::shared_ptr ma1 = AdaptAssignableDataSource<Arg1T>()( arg1 );
                if ( !ma1 )
                    throw non_lvalue_args_exception( 1, arg1->getType() );
                AssignableDataSource<Arg2T>::shared_ptr ma2 = AdaptAssignableDataSource<Arg2T>()( arg2 );
                if ( !ma2 )
                    throw non_lvalue_args_exception( 2, arg2->getType() );
                AssignableDataSource<Arg3T>::shared_ptr ma3 = AdaptAssignableDataSource<Arg3T>()( arg3 );
                if ( !ma3 )
                    throw non_lvalue_args_exception( 3, arg3->getType() );

                // ok, deliver part to 'user'.
                return new EventHook3(e, ma1, ma2,ma3); // called by TemplateFactoryPart3
            }
        };

        template<class EventT, class Enable = void>
        struct EventHookFactoryGenerator
        {
            TemplateFactoryPart<EventT*,EventHookInterface*> operator()() {
                return new TemplateFactoryPart0<EventT*,EventHookInterface*,EventHookGenerator>( EventHookGenerator(), "Event Hook" );
            }
        };
        
        
        template<class EventT>
        struct EventHookFactoryGenerator<EventT, typename boost::enable_if< EventT::SlotFunction::Arity == 1 >::type >
        {
                TemplateFactoryPart<EventT*,EventHookInterface*> operator()() {
                    return new TemplateFactoryPart1<
                        EventT*,
                        EventHookInterface*,
                        EventHookGenerator,
                        EventT::SlotFunction::arg1_type>( EventHookGenerator(), "Event Hook", "arg1", "description" );
                }
        };

        template<class EventT>
        struct EventHookFactoryGenerator<EventT, typename boost::enable_if< EventT::SlotFunction::Arity == 2 >::type >
        {
                TemplateFactoryPart<EventT*,EventHookInterface*> operator()() {
                    return new TemplateFactoryPart2<
                        EventT*,
                        EventHookInterface*,
                        EventHookGenerator,
                        EventT::SlotFunction::arg1_type,
                        EventT::SlotFunction::arg2_type>( EventHookGenerator(), "Event Hook",
                                                          "arg1", "description", "arg2", "description" );
                }
        };

        template<class EventT>
        struct EventHookFactoryGenerator<EventT, typename boost::enable_if< EventT::SlotFunction::Arity == 3 >::type >
        {
                TemplateFactoryPart<EventT*,EventHookInterface*> operator()() {
                    return new TemplateFactoryPart3<
                        EventT*,
                        EventHookInterface*,
                        EventHookGenerator,
                        EventT::SlotFunction::arg1_type,
                        EventT::SlotFunction::arg2_type,
                        EventT::SlotFunction::arg3_type>( EventHookGenerator(), "Event Hook",
                                                          "arg1", "description", "arg2", "description",
                                                          "arg3", "description");
                }
        };
        // repeat for each additional argument... N=10 ?
    }
}
#endif

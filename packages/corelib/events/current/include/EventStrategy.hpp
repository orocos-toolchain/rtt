/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventStrategy.hpp 

                        EventStrategy.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef EVENTSTRATEGY_HPP
#define EVENTSTRATEGY_HPP

namespace ORO_CoreLib
{
    namespace detail
    {
        struct EventStrategyMethodsInterface
        {
            /**
             * Synchronously notifies all the Listeners that this event occured
             * @post all registered Listeners were called for handling this Event
             */
            virtual void notifySyn()=0;

            /**
             * Asynchronously notifies all the Listeners that this event occured
             * @post All registered Listeners will be notified within undetermined time
             */
            virtual void notifyAsyn()=0;

            /**
             * Synchronously notifies a completer that the event should be completed
             *
             * @param eli 
             *        The Listener requesting completion
             * @post  The completer associated with <eli> has been called
             */
            virtual void completeSyn(EventListenerInterface* eli)=0;

            /**
             * Asynchronously completes an event
             *
             * @param eli 
             *        The Listener requesting completion
             * @post  The Event will be completed within undetermined time
             */
            virtual void completeAsyn(EventListenerInterface* eli)=0;
        };

        struct EventStrategy
        {
            virtual void notify( EventStrategyMethodsInterface* e ) = 0;
            virtual void complete( EventStrategyMethodsInterface* e, EventListenerInterface* eli ) = 0;
        };

        struct SynSyn : public EventStrategy
        {
            void notify( EventStrategyMethodsInterface* e )
            {
                e->notifySyn();
            }
            void complete( EventStrategyMethodsInterface* e, EventListenerInterface* eli )
            {
                e->completeSyn( eli );
            }
        };

        struct SynAsyn : public EventStrategy
        {

            void notify( EventStrategyMethodsInterface* e )
            {
                e->notifySyn();
            }

            void complete( EventStrategyMethodsInterface* e, EventListenerInterface* eli )
            {
                e->completeAsyn( eli );
            }
        };

        struct AsynSyn : public EventStrategy
        {

            void notify( EventStrategyMethodsInterface* e )
            {
                e->notifyAsyn();
            }

            void complete( EventStrategyMethodsInterface* e, EventListenerInterface* eli )
            {
                e->completeSyn( eli );
            }
        };

        struct AsynAsyn : public EventStrategy
        {

            void notify( EventStrategyMethodsInterface* e )
            {
                e->notifyAsyn();
            }


            void complete( EventStrategyMethodsInterface* e, EventListenerInterface* eli )
            {
                e->completeAsyn( eli );
            }

        };


        struct EventStrategyMethods
            : public EventStrategyMethodsInterface
        {
        /**
         * The four kind of event handling practices
         *
         * SYNSYN   Synchronous fire method, synchronous complete method
         * SYNASYN  Synchronous fire method, asynchronous complete method
         * ASYNSYN  Asynchronous fire method, synchronous complete method
         * ASYNASYN Asynchronous fire method, asynchronous complete method
         */
        enum EventType { SYNSYN, SYNASYN, ASYNSYN, ASYNASYN };


            EventStrategyMethods(EventType t)
            {
        switch ( t )
            {

            case SYNSYN:
                strategy = &ss;
                break;

            case SYNASYN:
                strategy = &sa;
                break;

            case ASYNSYN:
                strategy = &as;
                break;
            case ASYNASYN:
                strategy = &aa;
                break;
            default:
                strategy = &ss;
            }
            }
        protected:
            friend class SynSyn;
            friend class SynAsyn;
            friend class AsynSyn;
            friend class AsynAsyn;

        protected:
            //friend class SynSyn;
            SynSyn ss;
            //friend class SynAsyn;
            SynAsyn sa;
            //friend class AsynSyn;
            AsynSyn as;
            //friend class AsynAsyn;
            AsynAsyn aa;

            /**
             * The Strategy holds the way in which handling/completing is done.
             * This is decided on Event construction
             */
            EventStrategy* strategy;

        };

    }
}

#endif

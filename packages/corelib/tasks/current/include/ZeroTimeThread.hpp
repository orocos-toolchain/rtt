/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:36 CEST 2002  ZeroTimeThread.h 

                       ZeroTimeThread.h -  description
                          -------------------
   begin                : Wed April 17 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

   ***************************************************************************
   *                                                                         *
   *   This program is free software; you can redistribute it and/or modify  *
   *   it under the terms of the GNU General Public License as published by  *
   *   the Free Software Foundation; either version 2 of the License, or     *
   *   (at your option) any later version.                                   *
   *                                                                         *
   ***************************************************************************/



#ifndef ZEROTIMETHREAD_HPP
#define ZEROTIMETHREAD_HPP

#include "TaskExecution.hpp"


namespace ORO_CoreLib
{

    class TaskNonPreemptible;
    /**
     * This thread is the fastest, non preemptible thread in the
     * Orocos realtime system
     * Uses the Singleton pattern, since there will be only one EVER.
     *
     * ABOUT THREADSAFETY: map does not invalidate its iterators when an element is removed
     * or inserted in the map (except for the removed elements iterator itself).
     * This could mean we don't need mutexes. mutexes don't work in init_module/cleanup_module
     * (yet)
     * 
     * @see ComponentThreaded
     */
    class ZeroTimeThread
        : public TaskExecution
    {
    public:
        static ZeroTimeThread* Instance();

        /**
         * Releases the ZeroTimeThread
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release();

        /**
         * Add a TaskNonPreemptible which is handled each n nanoseconds
         * 
         * @param t The task to handle each n nanoseconds
         * @param n handle every n nanoseconds
         */
        bool taskAdd( TaskNonPreemptible* t, const nsecs n );

        /**
         * Remove a TaskNonPreemptible from handleing
         *
         * @post <t> is no longer handled by this thread
         */
        void taskRemove( TaskNonPreemptible* t );
    protected:

        /**
         * Constructor
         */
        ZeroTimeThread();

        /**
         * Destructor
         */
        virtual ~ZeroTimeThread();

    private:

        /**
         * Our only instance of the ZeroTimeThread
         */
        static ZeroTimeThread* _instance;

    };
} // namespace ORO_CoreLib

#endif

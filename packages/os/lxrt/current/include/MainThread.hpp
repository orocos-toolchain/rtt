/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  MainThread.hpp 

                        MainThread.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
 
 
#ifndef MAINTHREAD_HPP
#define MAINTHREAD_HPP

#include "PeriodicThread.hpp"

//#include <os/Time.hpp>

#include <pkgconf/os_lxrt.h>

extern "C"
int lock_all(int stk, int heap);

namespace ORO_OS
{

    /**
     * This class initializes the scheduler on construction
     */
    class SchedPolicy
    {
        static const int SELECT_THIS_PID=0;
    public:
        SchedPolicy();
            
        virtual ~SchedPolicy();
    protected:
        void* rt_task;
    };

    /**
     * Allocates all memory resources for the main thread
     */
    struct MemPolicy
    {
        MemPolicy(int hp, int stk);

        virtual ~MemPolicy() {}
    protected:
        int alloc_heap;
        int alloc_stack;
    };

    /**
     * This class simulates a main() realtime thread so that ipc is 
     * possible for realtime primitives
     *
     * This is RTAI/LXRT specific
     */
    class MainThread
        : protected MemPolicy, protected SchedPolicy
    {
        static MainThread* mt;

    public:
        /**
         * Returns the single instance of MainThread.
         * @param heap_sz
         *        The heap to be reserved for this process. Defaults to 2*1024*1024 bytes
         * @param stack_sz
         *        The stack to be reserved for this process. Defaults to 2*1024 bytes
         * @return The MainThread instance.
         */
        static MainThread* Instance(int heap_sz = 512*1024, int stack_sz = 8*1024)
        {
            if ( mt == 0 )
                mt = new MainThread(heap_sz, stack_sz);

            return mt;
        }

        static void Release()
        {
            if ( mt != 0 )
                delete mt;

            mt = 0;
        }

        bool start() { return true; }
        bool stop() { return true; }

        /**
         * The function to be queued
         * Use this as a base class if you want a function
         * to be execute()'d
         */
        struct Functor
        {
            virtual void operator() () const = 0;
        };

        /**
         * Starts a task and returns when the
         * task effectively runs
         */
        void startTask( PeriodicThread* t )
        {
            t->start();
        }

        /**
         * Stops a task and returns when the
         * task has effectively stopped
         */
        void stopTask ( PeriodicThread* t )
        {
            t->stop();
        }

        /**
         * Executes a function object in the main thread
         */
        void execute( void (*funcObj)(void) )
        {
            (*funcObj)();
        }

        /**
         * Complete all pending requests
         */
        void complete()
        {
            // nothing to do since all is processed immediately
        }

    protected:

        MainThread(int heap_sz, int stack_sz)
            : MemPolicy(heap_sz, stack_sz), SchedPolicy()
        {
        }

        MainThread()
            :MemPolicy(1*1024*1024, 512*1024)
        {
        }

        virtual ~MainThread()
        {
        }

    private:

    };

}

#endif

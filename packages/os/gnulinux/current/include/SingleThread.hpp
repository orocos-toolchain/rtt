/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:53 CEST 2004  SingleThread.hpp 

                        SingleThread.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 

#ifndef SINGLE_THREAD_HPP
#define SINGLE_THREAD_HPP

// Our own package config headers.
#include "pkgconf/os.h"
#include "pkgconf/os_gnulinux.h"

#include <os/fosi.h>

#include <os/RunnableInterface.hpp>

#include <string>

namespace ORO_OS
{
    /**
     * @brief This Thread abstraction class represents
     * a thread which can be started many times
     * and stops each time the step() function returns.
     *
     */
    class SingleThread 
    {
        friend void* singleThread_f( void* t );

    public:

        SingleThread(int priority, const std::string& name, RunnableInterface* r=0);
    
        virtual ~SingleThread();

        /**
         * Start the thread
         *
         * @return true if successfull.
         */
        virtual bool start();

        /**
         * Returns whether the thread is running
         */
        virtual bool isRunning() const;

        /**
         * Set the name of this task
         */
        virtual void taskNameSet(const char*);

        /**
         * Read the name of this task
         */
        virtual const char* taskNameGet() const;

        virtual void step();
    
        virtual bool initialize();

        virtual void finalize();

        bool makeHardRealtime() 
        { 
            return false;
        }

        bool makeSoftRealtime()
        { 
            return true;
        }

        bool isHardRealtime()   { return false; }

    protected:
        /**
         * When set to 1, the thread will run, when set to 0
         * the thread will stop
         */
        bool running;

        /**
         * The userspace thread carying the rt_task.
         */
        pthread_t thread;

        /**
         * The priority of the created thread.
         */
        int priority;

        /**
         * Signal the thread that it should exit.
         */
        bool prepareForExit;

        /**
         * The maximum lenght of the task name.
         * (in RTAI, only the 6 first chars are used.
         */
        static const int TASKNAMESIZE = 64;

        /**
         * The unique name of this task.
         */
        char taskName[TASKNAMESIZE];

        /**
         * The semaphore used for starting the thread.
         */
        sem_t sem;

        /**
         * The semaphore used for communicating between
         * the thread and the constructor/destructor.
         */
        sem_t confDone;

        /**
         * The possible Runnable to run in this Component
         */
        RunnableInterface* runComp;
    };

}

#endif

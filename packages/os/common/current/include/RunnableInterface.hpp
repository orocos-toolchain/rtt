/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  RunnableInterface.hpp 

                        RunnableInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 



#ifndef OS_RUNNABLEINTERFACE_HPP
#define OS_RUNNABLEINTERFACE_HPP

namespace RTT
{ namespace OS {
  class ThreadInterface;

    /**
     * @brief A class for running a certain piece of code in a thread.
     *
     * It defines three methods for executing functionality. It allows
     * to execute code in a PeriodicThreadInterface or SingleThreadInterface, which define the
     * control methods as starting and stopping (periodic) threads.
     * 
     * In a start-run-stop cycle, before step() is called the first time,
     * initialize() will be called in the thread that started this RunnableInterface.
     * When step() is ran the last time in this cycle, finalize() will be 
     * called, after it finishes, in the threaad that stopped this RunnableInterface.
     * 
     * A non periodic thread will call \a loop(), which indicates that the
     * RunnableInterface is allowed to block ( step() is not allowed to block ).
     * By default, loop() calls step(), but a subclass may override the loop() method
     * to put its own blocking functionality in. To break out of the loop() method,
     * reimplement \a breakLoop() such that loop() returns when breakLoop() is called.
     */
    class RunnableInterface
    {
      /**
       * The thread which runs this RunnableInterface.
       * Zero if no thread assigned.
       */
      ThreadInterface* rthread;
    public:
      /**
       * Create a runnable object. The optional constructor parameter
       * allows the object to attach directly to a thread. Otherwise,
       * ThreadInterface::run(RunnableInterface*) must be used to
       * attach this object to a thread. A thread can only run one
       * RunnableInterface object, use CoreLib tasks otherwise.
       * @param t The thread this object must attach to.
       */
      RunnableInterface(ThreadInterface* t=0);

      virtual ~RunnableInterface();

        /**
         * The method that will be called once each time before the periodical
         * execution of \a step() ( or non periodical execution of \a loop() ) is started.
         */
        virtual bool initialize() = 0;

        /**
         * The method that will be periodically executed when this
         * class is run in a periodic thread.
         */
        virtual void step() = 0;

        /**
         * The method that will be executed once when this
         * class is run in a non periodic thread. The default 
	 * implementation calls step() once.
         */
      virtual void loop();

        /**
         * This method is called to break out of the \a loop() method.
         * Reimplement this method to let \a loop() return and return
         * true on success. When this method is not reimplemented by you, it
         * will always return \a false, denoting that the loop can not
         * be breaked. The safest implementation of breakLoop only returns
         * if \a loop() returns, and may thus itself be blocking.
         * @return true if the loop could be notified to return.
         */
      virtual bool breakLoop();

        /**
         * The method that will be called once each time after the periodical
         * execution of \a step() ( or non periodical execution of \a loop() ) is stopped.
         */
        virtual void finalize() = 0;

      /**
       * Get the thread this object is run in.
       * @return a pointer to the thread or 0 if not run by a thread.
       */
      ThreadInterface* getThread() const;

      /**
       * Set the thread this object will be run in.
       */
      virtual void setThread(ThreadInterface* t);
    };

}}

#endif

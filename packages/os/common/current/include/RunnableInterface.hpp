/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  RunnableInterface.hpp 

                        RunnableInterface.hpp -  description
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
 
 



#ifndef RUNNABLEINTERFACE_HPP
#define RUNNABLEINTERFACE_HPP

namespace ORO_OS
{

    /**
     * @brief A class for running a certain piece of code in a thread.
     *
     * It defines three methods for executing functionality. It is 
     * complementary to the RunnableTaskInterface, which defines the
     * control methods as starting and stopping.
     * 
     * In a start-run-stop cycle, before step() is called the first time,
     * initialize() will be called.
     * When step() is ran the last time in this cycle, finalize() will be 
     * called, after it finishes.
     */

    class RunnableInterface
    {
        public:
            virtual ~RunnableInterface()
            {}

            /**
             * The method that will be periodically executed when this
             * class is run.
             */
            virtual void step() = 0;

            /**
             * The method that will be called once each time before the periodical
             * execution of step() is started. This method is executed in the
             * context of the step.
             */
            virtual bool initialize() = 0;

            /**
             * The method that will be called once each time after the periodical
             * execution of step() is stopped. This method is exectuted in the
             * context of the step.
             */
            virtual void finalize() = 0;
    };

}

#endif

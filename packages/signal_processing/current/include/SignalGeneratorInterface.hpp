/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:41:00 CEST 2004  SignalGeneratorInterface.hpp 

                        SignalGeneratorInterface.hpp -  description
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
 
 
#ifndef ORO_SIGNAL_PROC_SIGNALGENERATORINTERFACE_HPP
#define ORO_SIGNAL_PROC_SIGNALGENERATORINTERFACE_HPP


namespace ORO_SignalProcessing
{

    /**
     * Generates a signal.
     */
    class SignalGeneratorInterface
    {
    public:
        /**
         * Return the value of the signal on time t.
         */
        virtual double result( double t ) = 0;

        /**
         * Reset the generator.
         */
        virtual void reset() = 0;
    };
}

#endif

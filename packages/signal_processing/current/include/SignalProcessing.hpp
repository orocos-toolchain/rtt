/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:41:00 CEST 2004  SignalProcessing.hpp 

                        SignalProcessing.hpp -  description
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
 
 

#ifndef SIGNAL_PROCESSING_HPP
#define SIGNAL_PROCESSING_HPP

/**
 * @brief This is the main namespace of the signal_processing package. 
 *
 * It groups all classes that are meant for signal generation and processing.
 * A signal is represented by a double, and signals are proccessed in a
 * channel.
 * It is handy for testing hardware setups, an Orocos Control Kernel
 * can be used for complexer control tasks. The FeedForwardController,
 * GenericSensor, GenericEffector and SignalGenerator Components use
 * this library.
 *
 * Orocos is in the need of attracking an external signal processing library.
 *
 * @see ORO_ControlKernel
 *
 * This package is not yet documented inb an Orocos Manual.
 */
namespace ORO_SignalProcessing
{
}

#endif

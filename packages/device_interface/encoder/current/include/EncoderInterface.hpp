/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  EncoderInterface.hpp 

                        EncoderInterface.hpp -  description
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
 
 
#ifndef ENCODERINTERFACE_HPP
#define ENCODERINTERFACE_HPP


namespace ORO_DeviceInterface
{
    /**
     * @brief The most generic interface to a position/turn counter,
     * for encoding positions.
     *
     * Within one turn, there are
     * resolution() position increments. After a turn-increment,
     * the position will be exactly zero or resolution(). An
     * upcounting() == true encoder will count from zero to
     * resolution() between two turn increments. An upcounting() == false
     * encoder will count from resolution() to zero between two turn
     * increments.
     */
    class EncoderInterface
    {
    public:
        virtual ~EncoderInterface() {}

        /**
         * Get the position within the current turn.
         */
        virtual int positionGet() const = 0;

        /**
         * Get the current turn.
         */
        virtual int turnGet() const = 0;
           
        /**
         * Set the position within the current turn.
         */
        virtual void positionSet( int p) = 0;

        /**
         * Set the current turn.
         */
        virtual void turnSet( int t ) = 0;

        /**
         * Return the position resolution. This number
         * can be negative or positive and denotes the
         * the maximal or minimal value positionGet().
         */
        virtual int resolution() const = 0;

        /**
         * Returns true if after a positive turn increment,
         * position increments positively.
         *
         * Meaning from 0 to |resolution()| or from
         * resolution() to zero if resolution() < 0
         */
        virtual bool upcounting() const = 0;
    };
}

#endif

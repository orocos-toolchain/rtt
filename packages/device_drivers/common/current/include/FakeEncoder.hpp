/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  FakeEncoder.hpp 

                        FakeEncoder.hpp -  description
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
 
 
#ifndef FAKEENCODER_HPP
#define FAKEENCODER_HPP


namespace ORO_DeviceDriver
{
    /**
     * @brief A Dummy Encoder implementation
     */
    class FakeEncoder
        : public ORO_DeviceInterface::EncoderInterface
    {
        int turn, pos;
    public:
        FakeEncoder() : turn(0), pos(0) {}
        virtual ~FakeEncoder() {}

        /**
         * Get the position within the current turn.
         */
        virtual int positionGet() const { return pos; }

        /**
         * Get the current turn.
         */
        virtual int turnGet() const { return turn; }
           
        /**
         * Set the position within the current turn.
         */
        virtual void positionSet( int p) { pos = p; }

        /**
         * Set the current turn.
         */
        virtual void turnSet( int t ) { turn = t; }

        /**
         * Return the position resolution. This number
         * can be negative or positive and denotes the
         * the maximal or minimal value positionGet().
         */
        virtual int resolution() const { return 4096; }

        /**
         * Returns true if after a positive turn increment,
         * position increments positively.
         * Meaning from 0 to |resolution()| or from
         * resolution() to zero if resolution() < 0
         */
        virtual bool upcounting() const { return true; }
    };
}

#endif

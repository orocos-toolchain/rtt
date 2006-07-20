/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  FakeEncoder.hpp 

                        FakeEncoder.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef FAKEENCODER_HPP
#define FAKEENCODER_HPP

#include "EncoderInterface.hpp"

namespace RTT
{
    /**
     * @brief A Dummy Encoder implementation
     */
    class FakeEncoder
        : public EncoderInterface
    {
        int turn, pos;
        unsigned int mresolution;
        bool mupcounting;
    public:
        FakeEncoder(unsigned int resolution = 4096, bool upcounting = true)
            : turn(0), pos(0),
              mresolution(resolution), mupcounting(upcounting)
        {}
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
        virtual int resolution() const { return mresolution; }

        /**
         * Returns true if after a positive turn increment,
         * position increments positively.
         * Meaning from 0 to |resolution()| or from
         * resolution() to zero if resolution() < 0
         */
        virtual bool upcounting() const { return mupcounting; }
    };
}

#endif

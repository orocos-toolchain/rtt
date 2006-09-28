/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  Encoder.hpp 

                        Encoder.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef CANOPENENCODER_HPP
#define CANOPENENCODER_HPP

#include "BeckhoffSSI.hpp"
#include <rtt/dev/EncoderInterface.hpp>


namespace RTT
{namespace CAN
{
    using CAN::BeckhoffSSI;

    class Encoder 
        : public EncoderInterface
    {
        BeckhoffSSI* bhssi;
        unsigned int chan;
    public:
        Encoder( BeckhoffSSI* ssi, unsigned int channel ) : bhssi(ssi), chan(channel) {}

        int positionGet() const
        {
            int p;
            bhssi->positionGet(chan, p);
            return p;
        }

        int turnGet() const
        {
            int t;
            bhssi->turnGet(chan, t);
            return t;
        }
            
        void positionSet( int p)
        {
            bhssi->positionSet(chan, p);
        }

        void turnSet( int t )
        {
            bhssi->turnSet(chan, t);
        }

        int resolution() const
        {
            return bhssi->resolution();
        }

        bool upcounting() const
        {
            return bhssi->upcounting();
        }
    };
}}

#endif

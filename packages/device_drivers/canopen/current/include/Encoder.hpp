/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  Encoder.hpp 

                        Encoder.hpp -  description
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
 
 
#ifndef CANOPENENCODER_HPP
#define CANOPENENCODER_HPP

#include "BeckhoffSSI.hpp"
#include <device_interface/EncoderInterface.hpp>


namespace CAN
{
    using Beckhoff::BeckhoffSSI;

    class Encoder 
        : public ORO_DeviceInterface::EncoderInterface
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
}

#endif

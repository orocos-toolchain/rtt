/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:22:43 CEST 2002  Axis.cpp 

                       Axis.cpp -  description
                          -------------------
   begin                : Mon June 10 2002
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

#include "device_drivers/Axis.hpp"
#include <os/rtstreams.hpp>
#include <corelib/EventInterfaces.hpp>



namespace CBDeviceDriver
{

    using namespace ORO_CoreLib;

    Axis::~Axis()
    {
        act->disableDrive();
    }

    void Axis::move( double vel )
    {
        act->driveSet( vel * v_to_u );
    }

    void Axis::enable()
    {
        act->enableDrive();
    }

    void Axis::disable()
    {
        act->disableDrive();
    }

    double Axis::positionGet()
    {
        return double( encoder->turnGet()*encoder->resolution() +  encoder->positionGet() ) / mm_to_inc + posOffset;
    }

    void Axis::positionSet( double newpos )
    {
        posOffset = newpos -  positionGet();
    }

    double Axis::calibrate( double calpos, double sign )
    {
        double currentPos;
        if ( sign < 0 )
            {
                if ( mm_to_inc > 0)
                    currentPos = calpos - ( encoder->resolution() - encoder->positionGet() ) / mm_to_inc;
                else
                    currentPos = calpos - encoder->positionGet() / mm_to_inc;
            }
        else
            {
                if ( mm_to_inc > 0)
                    currentPos =  calpos + encoder->positionGet() / mm_to_inc;
                else
                    currentPos = calpos + (encoder->resolution() - encoder->positionGet() ) / mm_to_inc;
            }
        double oldposition = positionGet();
        positionSet( currentPos );
        return positionGet() - oldposition;
    }

    void Axis::turnSet( int t )
    {
        encoder->turnSet( t );
    }

    double Axis::velocity()
    {
        return act->driveGet() / v_to_u;
    }

    void Axis::reset()
    { 
        act->disableDrive();
        act->stop();
    }

    Drive* Axis::actuatorGet()
    {
        return act;
    }

    EncoderInterface* Axis::encoderGet()
    {
        return encoder;
    }

    DigitalInput* Axis::switchGet()
    {
        return swt;
    }


};

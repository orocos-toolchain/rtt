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

#include "corelib/Axis.hpp"
#include <rtstl/rtstreams.hpp>
#include <corelib/EventInterfaces.hpp>



namespace CBDeviceDriver
{

    using namespace ORO_CoreLib;

    Axis::~Axis()
    {
        calTask.stop();
        homeTask.stop();
        act->stop();
    }

    void Axis::callibrate()
    {
        calTask.start();
    }

    void Axis::reset()
    {
        act->offsetSet( 0 );
        act->stop();
    }

    void Axis::home()
    {
        homeTask.start();
    }

    ActuatorInterface<double>* Axis::actuatorGet()
    {
        return act;
    }

    EncoderIncrementalInterface* Axis::encoderGet()
    {
        return encincr;
    }

    SwitchHomingInterface* Axis::switchGet()
    {
        return swt;
    }


};

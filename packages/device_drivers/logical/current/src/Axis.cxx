/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:22:43 CEST 2002  Axis.cpp 

                       Axis.cpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
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

#include "device_drivers/Axis.hpp"
#include "device_drivers/HomePositionDetector.hpp"
#include "device_drivers/DigitalInput.hpp"
#include "device_drivers/DigitalOutput.hpp"
#include "device_drivers/AnalogDrive.hpp"


namespace ORO_DeviceDriver
{
    using namespace ORO_CoreLib;

    Axis::Axis( AnalogDrive* a ) 
        : act( a ), homeswitch(0), brakeswitch(0),
          hpd(0)
    {
        lock();
    }

    Axis::~Axis()
    {
        lock();
        delete act;
        delete brakeswitch;
        delete homeswitch;
        for (SensList::iterator it = sens.begin();
             it != sens.end();
             ++it)
            {
                delete it->second.sensor;
                delete it->second.low;
                delete it->second.high;
            }
        delete hpd;
    }

    void Axis::drive( double vel )
    {
        // All sensors may limit the drive
#if 0
        for (SensList::iterator it = sens.begin();
             it != sens.end();
             ++it)
            {
                if ( it->second.high->isOn() && vel > 0 ||
                     it->second.low->isOn()  && vel < 0 )
                    {
                        vel = 0.0;
                        break;
                    }
            }
#endif
        act->driveSet( vel );
    }

    void Axis::stop()
    {
        act->enableDrive();
        act->driveSet( 0 );
        if ( brakeswitch )
            brakeswitch->switchOff();
    }

    bool Axis::isEnabled() const
    {
        return act->enableGet()->isOn() && ( brakeswitch ? ! brakeswitch->isOn() : true );
    }

    void Axis::lock()
    {
        act->driveSet( 0 );
        if (brakeswitch)
            brakeswitch->switchOn();
        act->disableDrive();
    }

    void Axis::sensorSet(const std::string& name,  SensorInterface<double>* _sens,
                         DigitalInput* lowlimit, DigitalInput* highlimit )
    {
        if (sens.count(name) != 0)
            return;
        sens.insert(make_pair(name, SensorInfo( _sens, lowlimit, highlimit)) );

        // special treatment for "Position" sensor.
        if ( name == "Position" )
            {
                if ( homeswitch == 0 )
                    homeswitch =  new DigitalInput( hpd = new HomePositionDetector( _sens ), 0 );
            }
    }

    SensorInterface<double>* Axis::sensorGet(const std::string& name) const
    {
        if (sens.count(name) == 0)
            return 0;
        else
            return sens.find(name)->second.sensor;
    }

    std::vector<std::string> Axis::sensorList() const
    {
        std::vector<std::string> result;
        for (SensList::const_iterator it = sens.begin();  it != sens.end(); ++it)
            result.push_back( it->first );
        return result;
    }

    AnalogDrive* Axis::driveGet() const
    {
        return act;
    }

    void Axis::brakeSet( DigitalOutput* brk )
        { brakeswitch = brk; }

    DigitalOutput* Axis::brakeGet() const
        { return brakeswitch; }

    void Axis::homeswitchSet( DigitalInput* swtch )
        { delete homeswitch; homeswitch = swtch; }

    const DigitalInput* Axis::homeswitchGet() const
        { return homeswitch; }

};

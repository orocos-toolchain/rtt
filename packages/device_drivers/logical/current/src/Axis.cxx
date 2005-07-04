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
#include "device_drivers/DigitalInput.hpp"
#include "device_drivers/DigitalOutput.hpp"
#include "device_drivers/AnalogDrive.hpp"


namespace ORO_DeviceDriver
{
    using namespace ORO_CoreLib;

    Axis::Axis( AnalogDrive* a ) 
      : _drive_value(0), act( a ), brakeswitch(0),
         _is_locked(false), _is_stopped(false), _is_driven(true), 
         _max_drive( std::numeric_limits<double>::max()), _max_drive_event(NULL) 
    {
      stop();
      lock();
    }

    Axis::~Axis()
    {
      stop();
      lock();

      delete act;
      delete brakeswitch;
      for (SensList::iterator it = sens.begin(); it != sens.end(); ++it)
        delete it->second;
    }


    bool Axis::drive( double vel )
    {
      if (_is_stopped || _is_driven)
      {
          if ( (vel < _max_drive) && (vel > -_max_drive) )
          {
              act->driveSet( vel );
	      _drive_value = vel;
              _is_stopped = false;
              _is_driven  = true;
              return true;
          }
          else
          {
	    if (_max_drive_event == NULL){
              stop();
              lock();
	    }
	    else{
	      _max_drive_event->fire();
	      Logger::log() << Logger::Error << "(Axis) Maximum velocity treshold exceeded!" << Logger::endl;	      
	    }
	    return false;
          }
      }
      else
        return false;
    }


    bool Axis::stop()
    {
      if (_is_driven){
        act->driveSet( 0 );
	_drive_value = 0;
        _is_driven  = false;
        _is_stopped = true;
        return true;
      }
      else if (_is_stopped)
        return true;
      else
        return false;
    }
  

    bool Axis::lock()
    {
      if (_is_stopped){
        if ( brakeswitch )
            brakeswitch->switchOn();
        act->disableDrive();
        _is_locked  = true;
        _is_stopped = false;
        return true;
      }
      else if (_is_locked)
        return true;
      else
        return false;
    }
  
      
    bool Axis::unlock()
    {
      if (_is_locked){
        if ( brakeswitch )
            brakeswitch->switchOff();
        act->enableDrive();
        act->driveSet( 0 );
	_drive_value = 0;
        _is_locked  = false;
        _is_stopped = true;
        return true;
      }
      else if (_is_stopped)
        return true;
      else
        return false;
    }


  
    bool Axis::isLocked() const
    {
        return _is_locked;
    }


    bool Axis::isStopped() const
    {
        return _is_stopped;
    }


    bool Axis::isDriven() const
    {
        return _is_driven;
    }

    
    void Axis::limitDrive( double max )
    {
        _max_drive = max;
    }


    void Axis::setLimitDriveEvent(ORO_CoreLib::Event<void(void)>& maximumDrive)
    {
      _max_drive_event = &maximumDrive;
    }
  
    
    void Axis::setSensor(const std::string& name,  SensorInterface<double>* _sens)
    {
        if (sens.count(name) != 0)
            return;
        sens.insert(make_pair(name, _sens) );
    }

    SensorInterface<double>* Axis::getSensor(const std::string& name) const
    {
        if (sens.count(name) == 0)
            return 0;
        else
            return sens.find(name)->second;
    }

    std::vector<std::string> Axis::sensorList() const
    {
        std::vector<std::string> result;
        for (SensList::const_iterator it = sens.begin();  it != sens.end(); ++it)
            result.push_back( it->first );
        return result;
    }

    void Axis::setSwitch(const std::string& name, DigitalInput* _swtch)
    {
        if (swtch.count(name) != 0)
            return;
        swtch.insert(make_pair(name, _swtch) );
    }

    DigitalInput* Axis::getSwitch(const std::string& name) const
    {
        if (swtch.count(name) == 0)
            return 0;
        else
            return swtch.find(name)->second;
    }

    std::vector<std::string> Axis::switchList() const
    {
        std::vector<std::string> result;
        for (SwitchList::const_iterator it = swtch.begin();  it != swtch.end(); ++it)
            result.push_back( it->first );
        return result;
    }

    void Axis::setCounter(const std::string& name,  SensorInterface<int>* _count)
    {
        if (count.count(name) != 0)
            return;
        count.insert(make_pair(name, _count) );
    }

    SensorInterface<int>* Axis::getCounter(const std::string& name) const
    {
        if (count.count(name) == 0)
            return 0;
        else
            return count.find(name)->second;
    }

    std::vector<std::string> Axis::counterList() const
    {
        std::vector<std::string> result;
        for (CountList::const_iterator it = count.begin();  it != count.end(); ++it)
            result.push_back( it->first );
        return result;
    }

    AnalogDrive* Axis::getDrive() const
    {
        return act;
    }

    double Axis::getDriveValue() const
    {
      return _drive_value;
    }

    void Axis::setDrive(AnalogDrive* a)
    {
      delete act;
      act = a;
    }

    void Axis::setBrake( DigitalOutput* brk )
    {
      delete brakeswitch;
      brakeswitch = brk;
    }

    DigitalOutput* Axis::getBrake() const
        { return brakeswitch; }
};

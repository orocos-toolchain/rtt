// $Id: nAxisGeneratorCartesianVel.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
// Copyright (C) 2003 Klaas Gadeyne <klaas.gadeyne@mech.kuleuven.ac.be>
//                    Wim Meeussen  <wim.meeussen@mech.kuleuven.ac.be>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

#include "control_kernel/nAxesGeneratorVel.hpp"
#include <assert.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Geometry;
  using namespace ORO_Execution;
  using namespace ORO_OS;
  

  nAxesGeneratorVel::nAxesGeneratorVel(unsigned int num_axes, std::string name)
    : nAxesGeneratorVel_typedef(name),
      _num_axes(num_axes), 
      _duration(num_axes),      
      _velocity_local(num_axes),
      _velocity_desired(num_axes),
      _time_begin(num_axes),
      _time_passed(num_axes),
      _is_moving(num_axes),
      _is_accel(num_axes),
      _vel_profile(num_axes),
      _properties_read(false),
      _max_acc("max_acc", "Maximum Acceleration in Trajectory"),
      _max_jerk("max_jerk", "Maximum Jerk in Trajectory")


  {
    for(unsigned int i = 0 ; i < _num_axes ; i++)
      _duration[i]=0;
  }
  


  nAxesGeneratorVel::~nAxesGeneratorVel()
  {
    for( unsigned int i=0; i<_num_axes; i++)
      delete _vel_profile[i];
  }
  
  

  void nAxesGeneratorVel::pull()
  {
    for(unsigned int i = 0 ; i < _num_axes ; i++)
      {
	_time_passed[i] = TimeService::Instance()->secondsSince(_time_begin[i]);
      }

  }


  void nAxesGeneratorVel::calculate()
  {
    for(unsigned int i = 0 ; i < _num_axes ; i++)
      {
	//stop accelerating
	if (_is_accel[i])
	  {
	    _velocity_local[i]=_vel_profile[i]->Pos(_time_passed[i]);	      
	      if ( _velocity_desired[i] == _velocity_local[i] )
		{
		  _is_accel[i] = false;
		}
	  }
	// stop moving
	if (_is_moving[i] && _time_passed[i] >= _duration[i] && _duration[i] != 0) 
	  {
	    _is_moving[i] = false;
	    _velocity_local[i]=0;
	  }
      }
  }

  
  void nAxesGeneratorVel::push()      
  {
    _velocity_DOI->Set(_velocity_local);
  }



  bool nAxesGeneratorVel::componentLoaded()
  {
    // get interface to Setpoint data types
    if ( !nAxesGeneratorVel_typedef::SetPoint::dObj()->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesGeneratorVel::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    // set empty values
    vector<double> _temp_vector(_num_axes);
    _velocity_DOI->Set(_temp_vector);

    return true;
  }
  


  bool nAxesGeneratorVel::componentStartup()
  {
    // check if updateProperties has been called
    if (!_properties_read){
      cerr << "nAxesGeneratorPos::componentStartup() Properties have not been read." << endl;
      return false;
    }

    // initial velocity is zero
    for(unsigned int i = 0 ; i < _num_axes ; i++) {
      _is_moving[i] = false;
      _is_accel[i]=false;
      _velocity_local[i]=0;
      _velocity_desired[i]=0;
    }
    return true;
  }


  
  bool nAxesGeneratorVel::updateProperties(const ORO_ControlKernel::PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if ( !composeProperty(bag, _max_acc) ||
	 !composeProperty(bag, _max_jerk) ){
      cerr << "nAxesGeneratorVel::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_max_acc.value().size() == _num_axes);
    assert(_max_jerk.value().size() == _num_axes);

    // Instantiate Motion Profiles
    for( unsigned int i=0; i<_num_axes; i++)
      _vel_profile[i] = new VelocityProfile_Trap( _max_acc.value()[i], _max_jerk.value()[i]);

    return true;
  }

  
  void nAxesGeneratorVel::exportProperties(ORO_CoreLib::PropertyBag& bag)
  {
    bag.add(&_max_acc);
    bag.add(&_max_jerk); 
  }
  

  CommandFactoryInterface* nAxesGeneratorVel::createCommandFactory()
  {
    TemplateCommandFactory<nAxesGeneratorVel>* my_commandFactory = newCommandFactory( this );
    my_commandFactory->add( "drive", command( &nAxesGeneratorVel::drive,
					      &nAxesGeneratorVel::driveFinished,
					      "Set the velocity",
					      "velocity", "joint velocity for all axes",
					      "duration", "duration of movement") );
    my_commandFactory->add( "driveAxis", command( &nAxesGeneratorVel::driveAxis,
					      &nAxesGeneratorVel::driveAxisFinished,
					      "Set the velocity for one axis",
                                              "axis", "selected axis",
					      "velocity", "joint velocity for axis",
					      "duration", "duration of movement") );

    return my_commandFactory;
  }

  bool nAxesGeneratorVel::drive(const std::vector<double>& velocity,std::vector<double>& duration)
  {
    _duration = duration;
    _velocity_desired=velocity; 

    bool returnValue=false;
    _get_time=TimeService::Instance()->getTicks();
    for(unsigned int i = 0 ; i < _num_axes ; i++)
      {
	_time_begin[i] = _get_time;
	_time_passed[i] = 0;
	_is_moving[i] = true;
	_is_accel[i]=true;
	if(!_vel_profile[i]) std::cout << "_vel_profile " << i << "NULL!" << std::endl;
	_vel_profile[i]->SetProfile(_velocity_local[i],_velocity_desired[i]);
	if(_velocity_local.size()!=_num_axes || _duration[i] < 0)   
	  returnValue=false;
	else
	  returnValue=true;
      }
    return returnValue;
  }

  bool nAxesGeneratorVel::driveFinished() const
  {
    bool returnValue=true;
    for(unsigned int i = 0 ; i < _num_axes ; i++)
      if (returnValue)
	returnValue = (!_is_moving[i] || _duration[i] == 0);
    return returnValue;
  }

  bool nAxesGeneratorVel::driveAxis(const int axis,const double velocity,double duration)
  {
  
    _get_time = TimeService::Instance()->getTicks();
    _time_begin[axis]=_get_time;
    _time_passed[axis] = 0;
    _duration[axis] = duration;
    _velocity_desired[axis]=velocity; 
    _is_moving[axis] = true;
    _is_accel[axis]=true;
    /*    
      if(!_vel_profile[axis]) 
      std::cout << "_vel_profile " << axis << "NULL!" << std::endl;
      else
      std::cout << "_vel_profile" << axis << " succesfully setted!" << std::endl;
    */
    _vel_profile[axis]->SetProfile(_velocity_local[axis],_velocity_desired[axis]);
    if(_velocity_local.size()!=_num_axes || _duration[axis] < 0)   
      return false;
    else
      return true;
  
  }

  bool nAxesGeneratorVel::driveAxisFinished(const int axis) const
  {
    return (!_is_moving[axis] || _duration[axis] == 0);
  }


  
} // namespace


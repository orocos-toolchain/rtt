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
  

  nAxesGeneratorVel::nAxesGeneratorVel(unsigned int num_axes,  std::string name)
    : nAxesGeneratorVel_typedef(name),
      _num_axes(num_axes), 
      _velocity_local(num_axes)
      
  {
  }
  


  nAxesGeneratorVel::~nAxesGeneratorVel()
  {
   
  }
  
  

  void nAxesGeneratorVel::pull()
  {
    
  }
  


  void nAxesGeneratorVel::calculate()
  {
    _time_passed = TimeService::Instance()->secondsSince(_time_begin);

    // stop moving
    if (_is_moving && _time_passed >= _duration && _duration != 0){
      _is_moving = false;
      for(unsigned int i = 0 ; i < _num_axes ; i++)
	_velocity_local[i]=0;
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
    _is_moving = false;

    // initial velocity is zero
    for(unsigned int i = 0 ; i < _num_axes ; i++)
      _velocity_local[i]=0;

    return true;
  }


  
  bool nAxesGeneratorVel::updateProperties(const ORO_ControlKernel::PropertyBag& bag)
  {
    return true;
  }
  
  void nAxesGeneratorVel::exportProperties(ORO_CoreLib::PropertyBag& bag)
  {
    
  }
  

  CommandFactoryInterface* nAxesGeneratorVel::createCommandFactory()
  {
    TemplateCommandFactory<nAxesGeneratorVel>* my_commandFactory = newCommandFactory( this );
    my_commandFactory->add( "drive", command( &nAxesGeneratorVel::drive,
					      &nAxesGeneratorVel::driveFinished,
					      "Set the velocity",
					      "velocity", "joint velocity for all axes",
					      "duration", "duration of movement") );
    return my_commandFactory;
  }

  bool nAxesGeneratorVel::drive(const std::vector<double>& velocity,double duration)
  {
    //MutexLock locker(_my_lock);
    _time_begin = TimeService::Instance()->getTicks();
    _time_passed = 0;
    _duration = duration;
    _velocity_local=velocity; 
    _is_moving = true;
   
    if(_velocity_local.size()!=_num_axes || _duration < 0)   
      return false;
    else
      return true;

  }

  bool nAxesGeneratorVel::driveFinished() const
  {
    return (!_is_moving || _duration == 0);
  }


  
} // namespace


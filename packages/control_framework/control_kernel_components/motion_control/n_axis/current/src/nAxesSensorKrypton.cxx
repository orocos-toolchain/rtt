// $Id: nAxisGeneratorCartesianPos.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#include "control_kernel/nAxesSensorKrypton.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceDriver;
  using namespace ORO_Geometry;

  nAxesSensorKrypton::nAxesSensorKrypton(unsigned int num_leds, 
					 std::string name)
    : nAxesSensorKrypton_typedef(name),
      _num_leds(num_leds),
      _krypton(num_leds),
      _kryptondata(3 * num_leds),
      _leds_local(num_leds)
  {}


  nAxesSensorKrypton::~nAxesSensorKrypton(){};
  

  void nAxesSensorKrypton::pull()
  {
    // copy values from Krypton LEDs to local variable
    _krypton.PositionGet(_kryptondata);
    for (unsigned int i=0; i<_num_leds; i++){
      Vector temp;
      for (unsigned int j=0; j<3; j++)
	temp(j) = _kryptondata[(3*i) + j];
      _leds_local[i] = temp;
    }
  }


  void nAxesSensorKrypton::calculate()
  {}


  
  void nAxesSensorKrypton::push()      
  {
    _leds_DOI->Set(_leds_local);
  }



  bool nAxesSensorKrypton::componentLoaded()
  {
    // get interface to Input data types
    if (!Input->dObj()->Get("KryptonLeds", _leds_DOI)){
      cerr << "nAxesSensorKrypton::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    // set empty values
    vector<Vector> _temp_vector(_num_leds);
    _leds_DOI->Set(_temp_vector);

    return true;
  }


  bool nAxesSensorKrypton::componentStartup()
  {
    return true;
  }
  
  

} // end namespace ORO_nAxesControlKernel

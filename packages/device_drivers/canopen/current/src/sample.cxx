/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:33 CEST 2004  sample.cxx 

                        sample.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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



int main()
{
    cbus = new CANBus();
    ccontrol = new CANPieController( cbus, 0 );

    cbus->write(msg);
    
    // CAN aware
    // opstarten bus coupler
    bhbus = new BeckHoffBus();
    
    bcoupler = new BeckhoffBusCoupler(cbus, node_id );

    // default LVD, later aparte f_ies per feature
    ssi_module = new BeckhoffSSIModule(bcoupler, 5 );
    ssi_module = new BeckhoffSSIModule(bcoupler, 6 );
    
    mySSI = SSIInterface::getObjectByName("LVDModule");
    
    ssi_module->getPosition();
    
    return 0;
}

/***************************************************************************
  tag: Peter Soetens  Fri Feb 24 17:02:57 CET 2006  CartesianProcess.cxx 

                        CartesianProcess.cxx -  description
                           -------------------
    begin                : Fri February 24 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#include "control_kernel/CartesianProcess.hpp"
#include "corelib/PropertyComposition.hpp"

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

        CartesianProcess::CartesianProcess(const std::string& name)
            : This( name ),
              sensor("Sensor","Sensor reading joint positions", "CartesianSensor"),
              kinematics("Kinematics","Kinematics converting joint positions to frames", "Kinematics"),
              generator("Generator","Generator which interpolates frames", "CartesianGenerator"),
              controller("Controller","Controller which controls in joint space", "CartesianController"),
              effector("Effector","Effector sending joint velocities", "CartesianEffector")
        {
        }

        bool CartesianProcess::updateProperties( const PropertyBag& bag )
        {
            composeProperty( bag, sensor );
            composeProperty( bag, kinematics );
            composeProperty( bag, generator );
            composeProperty( bag, controller );
            composeProperty( bag, effector );
            return true;
        }

        void CartesianProcess::exportProperties( PropertyBag& bag )
        {
            bag.add( &sensor );
            bag.add( &kinematics );
            bag.add( &generator );
            bag.add( &controller );
            bag.add( &effector );
        }

        bool CartesianProcess::componentLoaded()
        {
            components.clear();
            ComponentBaseInterface* c;
            // query name servers in correct order:
            // Sensor->Kinematics->Generator->Controller->Effector
            c = kernel()->sensors.getObject( sensor.get() );
            if ( !c ) {
                Logger::log() << Logger::Warning << "Cartesian Process could not find Sensor '"<< sensor.get()<<"'."<<Logger::endl;
            } else
                components.push_back(c);

            c = kernel()->processes.getObject( kinematics.get() );
            if ( !c ) {
                Logger::log() << Logger::Error << "Cartesian Process could not find Kinematic Process '"<< kinematics.get()<<"'."<<Logger::endl;
                return false;
            } else
                components.push_back(c);

            c = kernel()->generators.getObject( generator.get() );
            if ( !c ) {
                Logger::log() << Logger::Error << "Cartesian Process could not find Generator '"<< generator.get()<<"'."<<Logger::endl;
                return false;
            } else
                components.push_back(c);

            c = kernel()->controllers.getObject( controller.get() );
            if ( !c ) {
                Logger::log() << Logger::Error << "Cartesian Process could not find Controller '"<< controller.get()<<"'."<<Logger::endl;
                return false;
            } else
                components.push_back(c);

            c = kernel()->effectors.getObject( effector.get() );
            if ( !c ) {
                Logger::log() << Logger::Warning << "Cartesian Process could not find Effector '"<< effector.get()<<"'."<<Logger::endl;
            } else
                components.push_back(c);

            return true;
        }

        bool CartesianProcess::componentStartup()
        {
            CList::iterator it = components.begin();
            
            while ( it != components.end() )
                if ( kernel()->startup( *it ) )
                    ++it;
                else {
                    CList::reverse_iterator rit(it);
                    ++rit; // move one back
                    while ( rit != components.rend() ) {
                        kernel()->shutdown( *rit );
                        ++rit;
                    }
#if 0
                    if ( it == components.begin() )
                        return false;
                    // undo startups:
                    --it;
                    while ( it != components.begin() ) {
                        kernel()->shutdown( *it );
                        --it;
                    }
                    kernel()->shutdown( *it );
#endif
                    return false;
                }
            return true;
        }

        void CartesianProcess::componentShutdown()
        {
            // shutdown in reverse order:
            CList::reverse_iterator it = components.rbegin();
            
            while ( it != components.rend() ) {
                kernel()->shutdown( *it );
                ++it;
            }
        }

        void CartesianProcess::update()
        {
            // components are sorted in the correct order, so update each one of them.
            std::for_each( components.begin(), components.end(), boost::bind(&ComponentBaseInterface::update,_1));
        }
}

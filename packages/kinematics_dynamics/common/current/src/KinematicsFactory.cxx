/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:34 CEST 2004  KinematicsFactory.cxx 

                        KinematicsFactory.cxx -  description
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

#include "kindyn/KinematicsFactory.hpp"
#include <pkgconf/system.h>
#ifdef OROPKG_CORELIB_PROPERTIES_MARSHALLING
#include <pkgconf/corelib_properties_marshalling.h>
#include ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE
#include ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE
#endif

#include "kindyn/SerialZYYDWH.hpp"
#include "kindyn/SerialmZXXmZXmZ.hpp"
#include <fstream>

using namespace ORO_CoreLib;
using namespace std;

namespace ORO_KinDyn
{

    KinematicsFactory::KinematicsFactoryPtr KinematicsFactory::instance;

    KinematicsFactory::KinematicsFactory() 
    {
        // Hard coded: two kuka models.
        this->addSerial321Device("Kuka361","ZYYDWH", 1.020,0.480,0.645,0.,0.,0.120 );
        this->addSerial321Device("Kuka160","ZXXZXZ", 0.900,0.970,1.080,0.,0.,0.180 );
    }

    bool KinematicsFactory::hasDevice(std::string name) const
    {
        return serial321Devices.count(name) == 1;
    }

    KinematicsInterface* KinematicsFactory::create(const std::string& name)
    {
        KinematicsInterface* ki = createSerial321Device(name);

        if ( ki )
            return ki;

        return 0;
    }

    KinematicsInterface* KinematicsFactory::createSerial321Device(const std::string& name)
    {
        if ( serial321Devices.count(name) == 0 ) {
            return 0;
        }
        return serial321Devices[name]->clone();
    }

    bool KinematicsFactory::removeSerial321Device( std::string name ) {
        if ( serial321Devices.count(name) == 0 )
            return false;
        serial321Devices.erase(name);
        return true;
    }

    bool KinematicsFactory::addSerial321Device( std::string name, Serial321* device)
    {
        if ( serial321Devices.count(name) ) {
            Logger::log() << Logger::Error << "Could not at device " << name<<" since already defined." << Logger::endl;
            return false;
        }
        serial321Devices[name].reset( device );
        return true;
    }

    bool KinematicsFactory::addSerial321Device( std::string name, std::string kind, 
                                                double l1, double l2, double l3, double l4, double l5, double l6,
                                                double offset, double eccentricity) {
        if ( serial321Devices.count(name) ) {
            Logger::log() << Logger::Error << "Could not at device " << name<<" since already defined." << Logger::endl;
            return false;
        }
        Serial321* ki;
        if (kind == "ZYYDWH"){
            ki = new SerialZYYDWH();
        }
        else if (kind == "ZXXZXZ"){
            ki = new SerialZXXZXZ();
        }
        else{
            return false;
        }
        assert(ki);
        serial321Devices[name].reset( ki );
        ki->geometrySet( l1, l2, l3, l4, l5, l6 );
        ki->offsetSet(offset);
        ki->eccentricitySet( eccentricity );
        return true;
    }

    bool KinematicsFactory::saveDevice(std::string name, std::string filename) {
        return this->saveSerial321Device(name, filename);
    }

    bool KinematicsFactory::saveSerial321Device(std::string name, std::string filename) {
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
        Logger::log() <<Logger::Error << "No Property Marshaller configured !" << Logger::endl;
        return false;
    
#else
        if ( serial321Devices.count(name) == 0 ) {
            return false;
        }
        boost::shared_ptr<Serial321> ki = serial321Devices.find(name)->second;
        PropertyBag bag;
        Property<string> pname("DeviceName","The name/serial number of the device.", name );
        Property<string> pkind("DeviceKind","The Kinematic architecture of the device.", ki->getKind() );
        Property<double> pl1("L1", "Link length", ki->getLinkLength(1));
        Property<double> pl2("L2", "Link length", ki->getLinkLength(2));
        Property<double> pl3("L3", "Link length", ki->getLinkLength(3));
        Property<double> pl4("L4", "Link length", ki->getLinkLength(4));
        Property<double> pl5("L5", "Link length", ki->getLinkLength(5));
        Property<double> pl6("L6", "Link length", ki->getLinkLength(6));
        Property<double> poffset("Offset", "Shoulder Offset", ki->getOffset() );
        Property<double> peccent("Eccentricity", "Arm Eccentricity", ki->getEccentricity() );

        bag << &pname << &pkind << &pl1 << &pl2 << &pl3 << &pl4 << &pl5 << &pl6 << &poffset << &peccent;

        // serialize
        std::ofstream file( filename.c_str() );
        if ( file )
            {
                OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER<std::ostream> marshaller( file );
                marshaller.serialize( bag );
                Logger::log() << Logger::Info << "Wrote "<< filename <<Logger::endl;
                return true;
            }
        else {
            Logger::log() << Logger::Error << "Could not open file "<< filename <<" for writing."<<Logger::endl;
            return false;
        }
#endif
    }

    bool KinematicsFactory::loadDevice(std::string filename) {
        return this->loadSerial321Device(filename);
    }

    bool KinematicsFactory::loadSerial321Device(std::string filename) {
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
        Logger::log() <<Logger::Error << "No Property Marshaller configured !" << Logger::endl;
        return false;
    
#else
        PropertyBag bag;
        Property<string> pname("DeviceName","" );
        Property<string> pkind("DeviceKind","" );
        Property<double> pl1("L1", "");
        Property<double> pl2("L2", "");
        Property<double> pl3("L3", "");
        Property<double> pl4("L4", "");
        Property<double> pl5("L5", "");
        Property<double> pl6("L6", "");
        Property<double> poffset("Offset", "");
        Property<double> peccent("Eccentricity", "");

        bag << &pname << &pkind << &pl1 << &pl2 << &pl3 << &pl4 << &pl5 << &pl6 << &poffset << &peccent;

        try {
            OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER demarshaller(filename);
            PropertyBag propbag;

            if ( demarshaller.deserialize( propbag ) )
                {
                    // refresh our own bag
                    refreshProperties( bag, propbag );
                    if ( serial321Devices.count(pname.get() ) != 0 ) {
                        Logger::log() << Logger::Info
                                      << "Factory creates new "<<pname.get()<<" devices from file "<< filename.c_str() <<Logger::endl;
                        this->removeSerial321Device( pname.get() );
                    }
                    // add the new one.
                    return this->addSerial321Device( pname.get(), pkind.get(),
                                                     pl1.get(), pl2.get(), pl3.get(), pl4.get(), pl5.get(), pl6.get(),
                                                     poffset.get(), peccent.get() );
                }
            else
                {
                    Logger::log() << Logger::Error
                                  << "Some error occured while parsing "<< filename.c_str() <<Logger::endl;
                    return false;
                }
        } catch (...) {
            Logger::log() << Logger::Error
                          << "Could not find "<< filename << endl;
            return false;
        }
        return true;
#endif
    }

}

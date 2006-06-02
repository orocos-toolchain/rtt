/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  PropertyLoader.cxx 

                        PropertyLoader.cxx -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 

#include "execution/PropertyLoader.hpp"
#include <pkgconf/system.h>
#ifdef OROPKG_CORELIB_PROPERTIES_MARSHALLING
#include <pkgconf/corelib_properties_marshalling.h>
#include ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE
#include ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE
#endif
#include <corelib/Logger.hpp>
#include <corelib/PropertyBagIntrospector.hpp>
#include <fstream>

using namespace std;
using namespace ORO_CoreLib;
using namespace ORO_Execution;

bool PropertyLoader::configure(const std::string& filename, TaskContext* target, bool strict ) const
{
    Logger::In in("PropertyLoader:configure");
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
        Logger::log() <<Logger::Error << "No Property Demarshaller configured !" << Logger::endl;
        return false;
    
#else
    if ( target->attributes()->properties() == 0) {
        Logger::log() <<Logger::Error << "TaskContext " <<target->getName()<<" has no Properties to configure." << Logger::endl;
        return false;
    }

    Logger::log() <<Logger::Info << "Configuring TaskContext '" <<target->getName()
                  <<"' with '"<<filename<<"'."<< Logger::endl;
    bool failure = false;
    OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER* demarshaller = 0;
    try
    {
        demarshaller = new OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER (filename);
    } catch (...) {
        Logger::log() << Logger::Error
                      << "Could not open file "<< filename << Logger::endl;
        return false;
    }
    try {
        PropertyBag propbag;
        vector<CommandInterface*> assignComs;

        if ( demarshaller->deserialize( propbag ) )
        {
            // Lookup props vs attrs :
            if ( !strict )
                failure = !updateProperties( *target->attributes()->properties(), propbag );
            else {
                // take restore-copy;
                PropertyBag backup;
                copyProperties( backup, *target->attributes()->properties() );
                if ( updateProperties( *target->attributes()->properties(), propbag ) == false ) {
                    // restore backup:
                    updateProperties( *target->attributes()->properties(), backup );
                    failure = true;
                }
                // cleanup
                flattenPropertyBag( backup );
                deleteProperties( backup );
            }
        }
        else
            {
                Logger::log() << Logger::Error
                              << "Some error occured while parsing "<< filename.c_str() <<Logger::endl;
                failure = true;
            }
    } catch (...)
    {
        Logger::log() << Logger::Error
                      << "Uncaught exception in deserialise !"<< Logger::endl;
        failure = true;
    }
    delete demarshaller;
    return !failure;
#endif // OROPKG_CORELIB_PROPERTIES_MARSHALLING

}

bool PropertyLoader::save(const std::string& filename, TaskContext* target) const
{
    Logger::In in("PropertyLoader::save");
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
        Logger::log() <<Logger::Error << "No Property Marshaller configured !" << Logger::endl;
        return false;
    
#else
    if ( target->attributes()->properties() == 0 ) {
        Logger::log() << Logger::Error << "TaskContext "<< target->getName()
                      << " does not have Properties to save." << Logger::endl;
        return false;
    }
    PropertyBag allProps;
    // Update exising file ?
    {
        // first check if the target file exists.
        std::ifstream ifile( filename.c_str() );
        // if target file does not exist, skip this step.
        if ( ifile ) {
            ifile.close();
            Logger::log() << Logger::Info << target->getName()<<" updating of file "<< filename << Logger::endl;
            // The demarshaller itself will open the file.
            OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER demarshaller( filename );
            if ( demarshaller.deserialize( allProps ) == false ) {
                // Parse error, abort writing of this file.
                Logger::log() << Logger::Error << "While updating "<< target->getName() <<" : Failed to read "<< filename << Logger::endl;
                return false;
            }
        }
        else
            Logger::log() << Logger::Info << "Creating "<< filename << Logger::endl;
    }

    // Write results
    PropertyBag* compProps = target->attributes()->properties();

    // decompose repos into primitive property types.
    PropertyBag  decompProps;
    PropertyBagIntrospector pbi( decompProps );
    pbi.introspect( *compProps );

    // merge with target file contents,
    // override allProps.
    bool updater = ORO_CoreLib::updateProperties( allProps, decompProps );
    if (updater == false) {
        Logger::log() << Logger::Error << "Could not update properties of file "<< filename <<"."<<Logger::endl;
    }        
    // serialize and cleanup
    std::ofstream file( filename.c_str() );
    if ( file )
        {
            OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER<std::ostream> marshaller( file );
            marshaller.serialize( allProps );
            Logger::log() << Logger::Info << "Wrote "<< filename <<Logger::endl;
        }
    else {
        Logger::log() << Logger::Error << "Could not open file "<< filename <<" for writing."<<Logger::endl;
        flattenPropertyBag( allProps );
        deleteProperties( allProps );
        return false;
    }
    // allProps contains copies (clone()), thus may be safely deleted :
    flattenPropertyBag( allProps );
    deleteProperties( allProps ); 
    flattenPropertyBag( decompProps );
    deleteProperties( decompProps ); 
    return true;
#endif
}

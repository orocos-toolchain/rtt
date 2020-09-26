/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  PropertyLoader.cxx

                        PropertyLoader.cxx -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "rtt/marsh/PropertyLoader.hpp"
#include "rtt/rtt-config.h"
#ifdef OROPKG_CORELIB_PROPERTIES_MARSHALLING
#include ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE
#include ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE
#endif
#include "rtt/Logger.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/marsh/PropertyBagIntrospector.hpp"
#include "rtt/types/PropertyComposition.hpp"
#include <fstream>

using namespace std;
using namespace RTT;
using namespace RTT::detail;

PropertyLoader::PropertyLoader(TaskContext *task)
  : target(task->provides().get())
{}

PropertyLoader::PropertyLoader(Service *service)
  : target(service)
{}

bool PropertyLoader::load(const std::string& filename) const
{
    Logger::In in("PropertyLoader:load");
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
        log(Error) << "No Property DemarshallInterface configured !" << endlog();
        return false;

#else
    if ( target->properties() == 0) {
        log(Error) << "Service " <<target->getName()<<" has no Properties to configure." << endlog();
        return false;
    }

    log(Info) << "Loading properties into Service '" <<target->getName()
                  <<"' with '"<<filename<<"'."<< endlog();
    bool failure = false;
    OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER* demarshaller = 0;
    try
    {
        demarshaller = new OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER (filename);
    } catch (...) {
        log(Error) << "Could not open file "<< filename << endlog();
        return false;
    }
    try {
        PropertyBag propbag;
        vector<ActionInterface*> assignComs;

        if ( demarshaller->deserialize( propbag ) )
        {
            // compose propbag:
            PropertyBag composed_props;
            if ( composePropertyBag(propbag, composed_props) == false) {
                delete demarshaller;
                return false;
            }
            // take restore-copy;
            PropertyBag backup;
            copyProperties( backup, *target->properties() );
            // First test if the updateProperties will succeed:
            if ( refreshProperties(  *target->properties(), composed_props, false) ) { // not strict
                // this just adds the new properties, *should* never fail, but
                // let's record failure to be sure.
                failure = !updateProperties( *target->properties(), composed_props );
            } else {
                // restore backup in case of failure:
                refreshProperties( *target->properties(), backup, false ); // not strict
                failure = true;
            }
            // cleanup
            deletePropertyBag( backup );
        }
        else
            {
                log(Error) << "Some error occured while parsing "<< filename.c_str() <<endlog();
                failure = true;
            }
    } catch (...)
    {
        log(Error)
                      << "Uncaught exception in deserialise !"<< endlog();
        failure = true;
    }
    delete demarshaller;
    return !failure;
#endif // OROPKG_CORELIB_PROPERTIES_MARSHALLING

}

bool PropertyLoader::configure(const std::string& filename, bool all ) const
{
    Logger::In in("PropertyLoader:configure");
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
        log(Error) << "No Property DemarshallInterface configured !" << endlog();
        return false;

#else
    if ( target->properties() == 0) {
        log(Error) << "Service " <<target->getName()<<" has no Properties to configure." << endlog();
        return false;
    }

    log(Info) << "Configuring Service '" <<target->getName()
                  <<"' with '"<<filename<<"'."<< endlog();
    bool failure = false;
    OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER* demarshaller = 0;
    try
    {
        demarshaller = new OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER (filename);
    } catch (...) {
        log(Error) << "Could not open file "<< filename << endlog();
        return false;
    }
    try {
        PropertyBag propbag;

        if ( demarshaller->deserialize( propbag ) )
        {
            // compose propbag:
            PropertyBag composed_props;
            if ( composePropertyBag(propbag, composed_props) == false) {
                delete demarshaller;
                return false;
            }
            // take restore-copy;
            PropertyBag backup;
            copyProperties( backup, *target->properties() );
            if ( refreshProperties( *target->properties(), composed_props, all ) == false ) {
                // restore backup:
                refreshProperties( *target->properties(), backup );
                failure = true;
                }
            // cleanup
            deletePropertyBag( backup );
        }
        else
            {
                log(Error) << "Some error occured while parsing "<< filename.c_str() <<endlog();
                failure = true;
            }
        deletePropertyBag( propbag );
    } catch (...)
    {
        log(Error)
                      << "Uncaught exception in deserialise !"<< endlog();
        failure = true;
    }
    delete demarshaller;
    return !failure;
#endif // OROPKG_CORELIB_PROPERTIES_MARSHALLING

}

bool PropertyLoader::store(const std::string& filename) const
{
    Logger::In in("PropertyLoader::store");
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
    log(Error) << "No Property Marshaller configured !" << endlog();
    return false;
#else
    std::ofstream file( filename.c_str() );
    if ( file )
    {
        // Write results
        PropertyBag* compProps = target->properties();
        PropertyBag allProps;

        // decompose repos into primitive property types.
        PropertyBagIntrospector pbi( allProps );
        pbi.introspect( *compProps );

        OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER<std::ostream> marshaller( file );
        marshaller.serialize( allProps );
        deletePropertyBag( allProps );
        log(Info) << "Wrote "<< filename <<endlog();
    }
    else {
        log(Error) << "Could not open file "<< filename <<" for writing."<<endlog();
        return false;
    }
    return true;
#endif
}

bool PropertyLoader::save(const std::string& filename, bool all) const
{
    Logger::In in("PropertyLoader::save");
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
        log(Error) << "No Property MarshallInterface configured !" << endlog();
        return false;

#else
    if ( target->properties() == 0 ) {
        log(Error) << "Service "<< target->getName()
                      << " does not have Properties to save." << endlog();
        return false;
    }
    PropertyBag allProps;
	PropertyBag  decompProps;

	// first check if the target file exists.
	std::ifstream ifile( filename.c_str() );
	// if target file does not exist, skip this step.
	if ( ifile ) {
	    ifile.close();
	    log(Info) << target->getName()<<" updating of file "<< filename << endlog();
	    // The demarshaller itself will open the file.
	    OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER demarshaller( filename );
	    if ( demarshaller.deserialize( allProps ) == false ) {
	        // Parse error, abort writing of this file.
	        log(Error) << "While updating "<< target->getName() <<" : Failed to read "<< filename << endlog();
	        return false;
	    }
	}
	else {
	    log(Info) << "Creating "<< filename << endlog();
	    return store(filename);
	}

	// Write results
	PropertyBag* compProps = target->properties();

	// decompose repos into primitive property types.
	PropertyBagIntrospector pbi( decompProps );
	pbi.introspect( *compProps );

	//Add target properties to existing properties
	bool updater = false;
	if (all) {
	    log(Info) << "Writing all properties of "<<target->getName()<<" to file "<< filename << endlog();
	    updater = updateProperties( allProps, decompProps ); // add new.
	}
	else {
	    log(Info) << "Refreshing properties in file "<< filename << " with values of properties of "<<target->getName() << endlog();
	    updater = refreshProperties( allProps, decompProps ); // only refresh existing.
	}
	if (updater == false) {
	    log(Error) << "Could not update properties of file "<< filename <<"."<<endlog();
	    deletePropertyBag( allProps );
	    deletePropertyBag( decompProps );
	    return false;
	}
    // ok, finish.
    // serialize and cleanup
    std::ofstream file( filename.c_str() );
    if ( file )
        {
            OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER<std::ostream> marshaller( file );
            marshaller.serialize( allProps );
            log(Info) << "Wrote "<< filename <<endlog();
        }
    else {
        log(Error) << "Could not open file "<< filename <<" for writing."<<endlog();
        deletePropertyBag( allProps );
        return false;
    }
    // allProps contains copies (clone()), thus may be safely deleted :
    deletePropertyBag( allProps );
    deletePropertyBag( decompProps );
    return true;
#endif
}

bool PropertyLoader::configure(const std::string& filename, const std::string& name ) const
{
    Logger::In in("PropertyLoader:configure");
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
    log(Error) << "No Property DemarshallInterface configured !" << endlog();
    return false;

#else
    log(Info) << "Reading Property '" <<name
              <<"' from file '"<<filename<<"'."<< endlog();
    bool failure = false;
    OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER* demarshaller = 0;
    try
    {
        demarshaller = new OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER (filename);
    } catch (...) {
        log(Error) << "Could not open file "<< filename << endlog();
        return false;
    }
    try {
        PropertyBag propbag;
        if ( demarshaller->deserialize( propbag ) )
        {
            // compose propbag:
            PropertyBag composed_props;
            if ( composePropertyBag(propbag, composed_props) == false) {
                deletePropertyBag( propbag );
                delete demarshaller;
                return false;
            }
            failure = !refreshProperty( *(target->properties()), composed_props, name );
        }
        else
            {
                log(Error) << "Some error occured while parsing "<< filename.c_str() <<endlog();
                failure = true;
            }
        deletePropertyBag( propbag );
    } catch (...)
    {
        log(Error) << "Uncaught exception in deserialise !"<< endlog();
        failure = true;
    }
    delete demarshaller;
    return !failure;
#endif // OROPKG_CORELIB_PROPERTIES_MARSHALLING
}

bool PropertyLoader::save(const std::string& filename, const std::string& name) const
{
    Logger::In in("PropertyLoader::save");
#ifndef OROPKG_CORELIB_PROPERTIES_MARSHALLING
        log(Error) << "No Property MarshallInterface configured !" << endlog();
        return false;

#else
    PropertyBag fileProps;
    // Update exising file ?
    {
        // first check if the target file exists.
        std::ifstream ifile( filename.c_str() );
        // if target file does not exist, skip this step.
        if ( ifile ) {
            ifile.close();
            log(Info) << "Updating file "<< filename << " with properties of "<<target->getName()<<endlog();
            // The demarshaller itself will open the file.
            OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER demarshaller( filename );
            if ( demarshaller.deserialize( fileProps ) == false ) {
                // Parse error, abort writing of this file.
                log(Error) << "Failed to read "<< filename << endlog();
                return false;
            }
        }
        else
            log(Info) << "Creating "<< filename << endlog();
    }

    // decompose service properties into primitive property types.
    PropertyBag  serviceProps;
    PropertyBagIntrospector pbi( serviceProps );
    pbi.introspect( *(target->properties()) );

    bool failure;
    failure = ! updateProperty( fileProps, serviceProps, name );

    deletePropertyBag( serviceProps );

    if ( failure ) {
        log(Error) << "Could not update properties of file "<< filename <<"."<<endlog();
        deletePropertyBag( fileProps );
        return false;
    }
    // serialize and cleanup
    std::ofstream file( filename.c_str() );
    if ( file )
        {
            OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER<std::ostream> marshaller( file );
            marshaller.serialize( fileProps );
            log(Info) << "Wrote Property "<<name <<" to "<< filename <<endlog();
        }
    else {
        log(Error) << "Could not open file "<< filename <<" for writing."<<endlog();
        deletePropertyBag( fileProps );
        return false;
    }
    // fileProps contains copies (clone()), thus may be safely deleted :
    deletePropertyBag( fileProps );
    return true;
#endif
}


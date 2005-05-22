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
#include <pkgconf/corelib_properties_marshalling.h>
#include ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE
#include ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE
#include "corelib/Logger.hpp"

using namespace std;
using namespace ORO_CoreLib;
using namespace ORO_Execution;

bool PropertyLoader::configure(const std::string& filename, TaskContext* target, bool strict ) const
{
    if ( target->attributeRepository.properties() == 0) {
        Logger::log() <<Logger::Error << "PropertyLoader: TaskContext " <<target->getName()<<" has no Properties to configure." << Logger::endl;
        return false;
    }

    Logger::log() <<Logger::Info << "PropertyLoader: Configuring " <<target->getName()
                  <<" with "<<filename<<"."<< Logger::endl;
    bool failure = false;
    try
    {
        OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER demarshaller(filename);
        PropertyBag propbag;
        vector<CommandInterface*> assignComs;

        if ( demarshaller.deserialize( propbag ) )
        {
            // Lookup props vs attrs :
            for( PropertyBag::iterator it = propbag.getProperties().begin();
                 it != propbag.getProperties().end();
                 ++it)
                {
                    PropertyBase* v = target->attributeRepository.properties()->find( (*it)->getName() );
                    if ( v == 0 ) {
                        Logger::log() << Logger::Debug;
                        Logger::log() << "PropertyLoader: Skipping Property " << (*it)->getName()
                                      <<": not in task "<< target->getName() <<Logger::endl;

                        continue;
                    }
                    DataSourceBase::shared_ptr origds(  (*it)->createDataSource() );
                    CommandInterface* ac = v->refreshCommand( origds.get() );
                    if ( ac )
                        assignComs.push_back( ac ); // store the assignment.
                    else {
                        DataSourceBase::shared_ptr tgtds( v->createDataSource() );
                        if (strict ) {
                            Logger::log() << Logger::Error;
                            failure = true;
                        }
                        else
                            Logger::log() << Logger::Info;
                        Logger::log()<< "PropertyLoader: Could not refresh Property '"<< tgtds->getType() << " " << (*it)->getName()
                                     <<"' with '"<< origds->getType() << " " << (*it)->getName() << "' from file."<<Logger::endl;
                    }
                }
            // Do all assignments. In strict mode, don't do any upon failure.
            for(vector<CommandInterface*>::iterator it = assignComs.begin();
                it != assignComs.end(); ++it) {
                if ( !failure )
                    (*it)->execute();
                delete *it;
            }

            // cleanup
            flattenPropertyBag( propbag );
            deleteProperties( propbag );
        }
        else
            {
                Logger::log() << Logger::Error
                              << "PropertyLoader: Some error occured while parsing "<< filename.c_str() <<Logger::endl;
                return false;
            }
    } catch (...)
    {
        Logger::log() << Logger::Error
                      << "PropertyLoader: Could not find "<< filename << endl;
        return false;
    }
    return !failure;

}

bool PropertyLoader::save(const std::string& filename, TaskContext* target) const
{
    if ( target->attributeRepository.properties() == 0 ) {
        Logger::log() << Logger::Error << "PropertyLoader : TaskContext "<< target->getName()
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
            Logger::log() << Logger::Info << "PropertyLoader: "<< target->getName()<<" updating of file "<< filename << Logger::endl;
            // The demarshaller itself will open the file.
            OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER demarshaller( filename );
            if ( demarshaller.deserialize( allProps ) == false ) {
                // Parse error, abort writing of this file.
                Logger::log() << Logger::Error << "PropertyLoader: While updating "<< target->getName() <<" : Failed to read "<< filename << Logger::endl;
                return false;
            }
        }
        else
            Logger::log() << Logger::Info << "PropertyLoader: Creating "<< filename << Logger::endl;
    }

    // Write results
    PropertyBag* compProps = target->attributeRepository.properties();

    // merge with target file contents,
    // override allProps.
    ORO_CoreLib::updateProperties( allProps, *compProps );
    // serialize and cleanup
    std::ofstream file( filename.c_str() );
    if ( file )
        {
            OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER<std::ostream> marshaller( file );
            marshaller.serialize( allProps );
            Logger::log() << Logger::Info << "PropertyLoader: Wrote "<< filename <<Logger::endl;
        }
    else {
        Logger::log() << Logger::Error << "PropertyLoader: Could not open file "<< filename <<" for writing."<<Logger::endl;
        flattenPropertyBag( allProps );
        deleteProperties( allProps );
        return false;
    }
    // allProps contains copies (clone()), thus may be safely deleted :
    flattenPropertyBag( allProps );
    deleteProperties( allProps ); 
    return true;

}

/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:38 CEST 2004  KernelConfig.cxx 

                        KernelConfig.cxx -  description
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
#include <control_kernel/KernelConfig.hpp>

#include <corelib/marshalling/CPFDemarshaller.hpp>
#include <corelib/marshalling/StreamProcessor.hpp>
#include <corelib/Logger.hpp>

namespace ORO_ControlKernel
{
    using namespace std;
    using namespace ORO_CoreLib;
    using detail::ExtensionInterface;
    
    KernelConfig::KernelConfig( ControlKernelInterface& _k, const std::string& _filename)
        : filename( _filename ),
          baseBag(0), extensionBag(0), selectBag(0), kernel(&_k)
    {
    }

    KernelConfig::~KernelConfig() 
    {
    }

    bool KernelConfig::configure()
    {
        // read/parse file
        try {
            {
                CPFDemarshaller parser(filename);

                //cout <<"Parsing... ";
                if ( !parser.deserialize( config ) )
                    {
                        Logger::log() << Logger::Error << "KernelConfig : "
                                      << "Error parsing "<< filename << Logger::endl;
                        return false;
                    }
            }
            PropertyBase* base = config.find("KernelProperties");
            if (base)
                baseBag = dynamic_cast<Property<PropertyBag>*>(base);
            else
                Logger::log() << Logger::Warning << "KernelConfig: "
                              << filename 
                              << " : No KernelProperties defined." << Logger::endl;

            PropertyBase* extension = config.find("Extensions");
            if (extension)
                extensionBag = dynamic_cast<Property<PropertyBag>*>(extension);
            else
                Logger::log() << Logger::Warning << "KernelConfig: "
                              << filename 
                              << " : No Extensions listed." << Logger::endl;

            PropertyBase* complist = config.find("StartupComponents");
            if (complist)
                selectBag = dynamic_cast<Property<PropertyBag>*>(complist);
            else
                Logger::log() << Logger::Info << "KernelConfig: "
                              << filename 
                              << " : No KernelComponents listed." << Logger::endl;

            if ( baseBag == 0 )
                {
                    Logger::log() << Logger::Error << "KernelConfig: "
                                  << filename 
                                  << " : No valid KernelProperties found." << Logger::endl;
                    return false; // failed !
                }

            Logger::log() << Logger::Debug << "KernelConfig: "
                          << filename 
                          << " : Setting KernelProperties." << Logger::endl;

            // update the list of selected components
            PropertyBag kernelbasebag( baseBag->value() );
            if (selectBag)
                copyProperties( kernelbasebag, selectBag->value() );

            // other possibility :  do not store in bag, dispatch right away, but then
            // need our own xml parser.
            // Iterate over all extensions
            if ( kernel->getExtensions().empty() )
                Logger::log() << Logger::Debug
                              << "KernelConfig: No Extensions present in this kernel." << Logger::endl;

            vector<ExtensionInterface*>::const_iterator it = kernel->getExtensions().begin();
            while (it != kernel->getExtensions().end() )
                {
                    // this is a bit of a hack because the KernelBaseFunction
                    // does not have an extension property file.
                    if ( (*it)->getName() == "Kernel" ) {
                        if ( (*it)->updateProperties( kernelbasebag ) ==  false )
                            Logger::log() <<Logger::Error << "The KernelBaseFunction failed to update its properties from file "
                                          << filename <<" !"<<Logger::nl
                                          << "  The file contained valid XML, but the wrong properties."<< Logger::nl
                                          << "  Check your configuration files." << Logger::endl;
                        ++it;
                        continue;
                    }

                    // read the file associated with each extension
                    PropertyBase* res = extensionBag->get().find( (*it)->getName() );
                    Property<string>*  extFileName;
                    if ( res && (extFileName = dynamic_cast<Property<string>* >(res)) )
                        {
                            try 
                                {
                                    CPFDemarshaller parser(extFileName->get());
                                    // We Got the filename of the Extension.
                                    Logger::log() <<Logger::Info << "KernelConfig: Parsing "
                                                  << extFileName->get() <<"."<<Logger::endl;
                                    PropertyBag extensionConfig;
                                    if ( parser.deserialize( extensionConfig ) )
                                        {
                                            // update nameserved props.
                                            if ( (*it)->updateProperties( extensionConfig ) == false )
                                                {
                                                    Logger::log() <<Logger::Error<< "KernelConfig: The "<< (*it)->getName()
                                                         << " Extension failed to update its properties from file "
                                                                  << extFileName->get() <<" !"<<Logger::nl
                                                         << "  The file contained valid XML, but the wrong properties."<< Logger::nl
                                                         << "  Check your configuration files." << Logger::endl;
                                                    return false;
                                                }
                                        } else
                                            {
                                                Logger::log() <<Logger::Error << "KernelConfig: File "<< extFileName->get() <<" found but unable to parse !"<<Logger::endl;
                                                return false;
                                            }
                                    flattenPropertyBag( extensionConfig );
                                    deleteProperties  ( extensionConfig );
                                } catch (...)
                                    {
                                        Logger::log() <<Logger::Error << "KernelConfig: File "<< extFileName->get() <<" not found !"<<Logger::endl;
                                        return false;
                                    }
                        
                        }
                    else {
                        Logger::log() <<Logger::Info
                                      << "KernelConfig: Extension \'" << (*it)->getName() << Logger::nl 
                                      << "\' is present in the Control Kernel, but not listed in the file \'"
                                      << filename <<"\'."<<endl;
                        PropertyBag emptyBag;
                        if ( (*it)->updateProperties(emptyBag) == false) {
                            Logger::log() <<Logger::Error
                                          << "KernelConfig: Extension \'" << (*it)->getName() << Logger::nl 
                                          << "\' did not accept empty properties. List its property file in \'"
                                          << filename <<"\'."<<endl;
                            return false;
                        }
                    }
                    ++it;
                }
            // this updates the user's properties
            if ( kernel->updateKernelProperties( baseBag->value() ) == false ) {
                Logger::log() <<Logger::Error
                              << "KernelConfig: User's updateKernelProperties returned false with properties from"
                              << "\'" << filename <<"\'."<<endl;
                return false;
            }
        } catch (...)
            {
                Logger::log() <<Logger::Error
                              <<"KernelConfig: file "<<filename<<" not found !"<<endl;
                return false;
            }

        // cleanup all parse left-overs
        flattenPropertyBag( config );
        deleteProperties( config  );

        return true;
    }

}                

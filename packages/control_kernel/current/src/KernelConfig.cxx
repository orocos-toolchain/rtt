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
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <corelib/marshalling/StreamProcessor.hpp>

namespace ORO_ControlKernel
{
    using namespace std;
    using namespace ORO_CoreLib;
    using detail::ExtensionInterface;
    
    KernelConfig::KernelConfig( KernelBaseFunction& _k, const std::string& _filename)
        : filename( _filename ),
          baseBag(0), extensionBag(0), selectBag(0), kernel(&_k)
    {
        XMLPlatformUtils::Initialize();
    }

    KernelConfig::~KernelConfig() 
    {
        XMLPlatformUtils::Terminate();
    }

    bool KernelConfig::configure()
    {
        // read/parse file
        XMLCh* fname = 0;
        try {
            fname = XMLString::transcode( filename.c_str() );
            LocalFileInputSource fis( fname );
            delete[] fname;
            CPFDemarshaller<LocalFileInputSource> parser(fis);

            //cout <<"Parsing... ";
            if ( !parser.deserialize( config ) )
                {
                    cerr << "Error parsing "<<filename<<endl;
                    return false;
                }
            PropertyBase* base = config.find("KernelProperties");
            if (base)
                baseBag = dynamic_cast<Property<PropertyBag>*>(base);
            else cerr << "No base." << endl;

            PropertyBase* extension = config.find("Extensions");
            if (extension)
                extensionBag = dynamic_cast<Property<PropertyBag>*>(extension);
            else cerr << "No extension" << endl;

            PropertyBase* complist = config.find("StartupComponents");
            if (complist)
                selectBag = dynamic_cast<Property<PropertyBag>*>(complist);
            else cerr << "No KernelComponents" << endl;


            if ( baseBag == 0 )
                {
                    cerr <<"No valid KernelProperties found !"<<endl;
                    return false; // failed !
                }

            cout << "Setting KernelProperties..."<<endl;
            // this updates the 'standard' properties
            kernel->updateProperties( baseBag->value() );
            // update the list of selected components
            if (selectBag)
                kernel->startupComponents( selectBag->value() );

            // this updates the user's properties
            kernel->updateKernelProperties( baseBag->value() );

            // other possibility :  do not store in bag, dispatch right away, but then
            // need our own xml parser.
            // Iterate over all extensions
            if ( ExtensionInterface::nameserver.getValueBegin() == ExtensionInterface::nameserver.getValueEnd() )
                cerr << "No Extensions present in this kernel."<<endl;
            ExtensionInterface::NameServerType::value_iterator it = ExtensionInterface::nameserver.getValueBegin();
            while (it != ExtensionInterface::nameserver.getValueEnd() )
                {
                    // read the file associated with each extension
                    //cout <<"Checking "<< ExtensionInterface::nameserver.getNameByObject(*it)<<endl;
                    PropertyBase* res = extensionBag->get().find( ExtensionInterface::nameserver.getNameByObject(*it) );
                    Property<string>*  extFileName;
                    if ( res && (extFileName = dynamic_cast<Property<string>* >(res)) )
                        {
                            try 
                                {
                                    // Parse The properties of the Extension.
                                    fname = XMLString::transcode( extFileName->get().c_str() );
                                    LocalFileInputSource extfis( fname );
                                    delete[] fname;
                                    parser.setStream(extfis);
                                    // We Got the filename of the Extension.
                                    PropertyBag extensionConfig;
                                    if ( parser.deserialize( extensionConfig ) )
                                        {
                                            // update nameserved props.
                                            if ( (*it)->updateProperties( extensionConfig ) == false )
                                                {
                                                    cerr << "  The "<< ExtensionInterface::nameserver.getNameByObject(*it)
                                                         << " Extenstion failed to update its properties from file "
                                                         << extFileName->get() <<" !"<<endl
                                                         << "  The file contained valid XML, but the wrong properties."<<endl
                                                         << "  Check your configuration files." << endl;
                                                    return false;
                                                }
                                        } else
                                            {
                                                cerr << "  File "<< extFileName->get() <<" found but unable to parse !"<<endl;
                                                return false;
                                            }
                                    flattenPropertyBag( extensionConfig );
                                    deleteProperties  ( extensionConfig );
                                } catch (...)
                                    {
                                        delete[] fname;
                                        cerr << "  File "<< extFileName->get() << " not found !"<<endl;
                                        return false;
                                    }
                        
                        }
                    else {
                        cerr << "  Warning: Extension \'" << ExtensionInterface::nameserver.getNameByObject(*it) << endl <<
                            " is present in the Control Kernel, but not listed in the file \'"<<
                            filename <<"\'."<<endl;
                    }
                    ++it;
                }
        } catch (...)
            {
                delete[] fname;
                cerr <<"  Kernel config file "<<filename<<" not found !"<<endl;
                return false;
            }

        // cleanup all parse left-overs
        flattenPropertyBag( config );
        deleteProperties( config  );

        return true;
    }

}                

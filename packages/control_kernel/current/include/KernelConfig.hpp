#ifndef KERNEL_CONFIG_HPP
#define KERNEL_CONFIG_HPP

#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include <corelib/marshalling/CPFDemarshaller.hpp>
#include "KernelInterfaces.hpp"
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <corelib/marshalling/StreamProcessor.hpp>

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    /**
     * A class for managing the initial kernel configuration.
     * It parses an xml file with the initial data.
     */
    template< class Kernel>
    class KernelConfig
    {
        public:
            /**
             * Create a kernel configurator which reads XML data
             * from a file.
             *
             * @param _k The kernel to configure.
             * @param _filename The filename of the XML data in cpf format.
             */
            KernelConfig( Kernel& _k, const std::string& _filename)
            : filename( _filename ),
            priority("priority","The priority of the kernel."),
            frequency("frequency","The periodic execution frequency in Hz"),
            baseBag(0), extensionBag(0), kernel(&_k)
            {
                XMLPlatformUtils::Initialize();
            }

            ~KernelConfig() 
            {
                XMLPlatformUtils::Terminate();
            }

            void configure()
            {
                using namespace std;

                // read/parse file
                try {
                     LocalFileInputSource fis( XMLString::transcode( filename.c_str() ) );
                    CPFDemarshaller<LocalFileInputSource> parser(fis);

                    cout <<"Parsing... ";
                    if ( !parser.deserialize( config ) )
                    {
                        cout << "Error parsing "<<filename<<endl;
                        return;
                    }
                PropertyBase* base = config.find("KernelProperties");
                if (base)
                    baseBag = dynamic_cast<Property<PropertyBag>*>(base);
                else cout << "No base." << endl;
                PropertyBase* extension = config.find("Extensions");
                if (extension)
                    extensionBag = dynamic_cast<Property<PropertyBag>*>(extension);
                else cout << "No extension" << endl;

                if ( baseBag == 0 || extensionBag == 0 )
                {
                    cout <<"No valid KernelProperties or Extensions tags found !"<<endl;
                    return; // failed !
                }

                cout << "Setting KernelProperties..."<<endl;
                kernel->updateKernelProperties( baseBag->value() );

                // debugging -->
                vector<string> vlist;
                extensionBag->get().list(vlist);
                copy(vlist.begin(), vlist.end(), ostream_iterator<string>(cout, "\n"));
                // --<
                
                // other possibility :  do not store in bag, dispatch right away, but then
                // need our own xml parser.
                ExtensionInterface::NameServerType::value_iterator it = ExtensionInterface::nameserver.getValueBegin();
                while (it != ExtensionInterface::nameserver.getValueEnd() )
                {
                    // read the file associated with each extension
                    cout <<"Checking "<< ExtensionInterface::nameserver.getNameByObject(*it)<<endl;
                    PropertyBase* res = extensionBag->get().find( ExtensionInterface::nameserver.getNameByObject(*it) );
                    Property<string>*  extFileName;
                    if ( res && (extFileName = dynamic_cast<Property<string>* >(res)) )
                    {
                        PropertyBag extensionConfig;
                        cout <<"Trying to open "<< extFileName->value() <<"...";
                        try 
                        {
                             LocalFileInputSource extfis(XMLString::transcode( extFileName->get().c_str() ) );
                             parser.setStream(extfis);
                             if ( parser.deserialize( extensionConfig ) )
                             {
                                 // update nameserved props.
                                 cout << "Updating ...";
                                 (*it)->updateProperties( extensionConfig );
                                 cout << "done !"<<endl;
                             } else
                                 cout << "file found but unable to parse !"<<endl;
                        } catch (...)
                        {
                            cout << "file not found !"<<endl;
                        }
                        
                    }
                    ++it;
                }
                } catch (...)
                {
                    cout <<"Kernel config file "<<filename<<" not found !"<<endl;
                    cout <<"Skipping kernel configuration."<<endl;
                    return;
                }
            }
                
        protected:
            std::string filename;
            Property<std::string> priority;
            Property<double>      frequency;
            /**
             * A bag containing all extensions properties.
             */
            PropertyBag            config;
            Property<PropertyBag>*           baseBag;
            Property<PropertyBag>*           extensionBag;
            Kernel* kernel;
    };

}



#endif

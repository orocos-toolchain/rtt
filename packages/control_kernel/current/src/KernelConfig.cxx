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
          baseBag(0), extensionBag(0), kernel(&_k)
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

            if ( baseBag == 0 || extensionBag == 0 )
                {
                    cerr <<"No valid KernelProperties or Extensions tags found !"<<endl;
                    return false; // failed !
                }

            cout << "Setting KernelProperties..."<<endl;
            // this updates the 'standard' properties
            kernel->updateProperties( baseBag->value() );
            // this updates the user's properties
            kernel->updateKernelProperties( baseBag->value() );

            flattenPropertyBag( baseBag->value() );
            deleteProperties( baseBag->value()  );
            delete baseBag;

            // other possibility :  do not store in bag, dispatch right away, but then
            // need our own xml parser.
            // Iterate over all extensions
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
                                                cerr << "File "<< extFileName->get() <<" found but unable to parse !"<<endl;
                                                return false;
                                            }
                                    flattenPropertyBag( extensionConfig );
                                    deleteProperties  ( extensionConfig );
                                } catch (...)
                                    {
                                        delete[] fname;
                                        cerr << "File "<< extFileName->get() << " not found !"<<endl;
                                        return false;
                                    }
                        
                        }
                    ++it;
                }
        } catch (...)
            {
                delete[] fname;
                cerr <<"Kernel config file "<<filename<<" not found !"<<endl;
                return false;
            }
        return true;
    }

}                

#include "control_kernel/ComponentConfigurator.hpp"
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <corelib/marshalling/CPFDemarshaller.hpp>
#include <control_kernel/PropertyExtension.hpp>

using namespace std;
using namespace ORO_CoreLib;
using namespace ORO_ControlKernel;

bool ComponentConfigurator::configure(const std::string& filename, PropertyComponentInterface* target) const
{
    bool result = false;
    XMLCh* name;
    cout << "Configuring " <<target->getName()<< endl;
    try
    {
        name =  XMLString::transcode( filename.c_str() );
        LocalFileInputSource fis( name );
        delete[] name;
        CPFDemarshaller<InputSource> demarshaller(fis);

        // store results in Components own bag if component exists.
        if ( demarshaller.deserialize( target->getLocalStore().value() ) )
        {
            // instruct component to update.
            if (!target->updateProperties( target->getLocalStore().value() ) )
                cerr << "Component "<<target->getName() << " dit not successfully update its properties !"<<endl;
            else
                result = true;
            // cleanup
            flattenPropertyBag( target->getLocalStore().value() );
            deleteProperties( target->getLocalStore().value() );
        }
        else
            {
                cerr << "Some error occured while parsing "<< filename.c_str() <<endl;
            }
    } catch (...)
    {
        delete[] name;
        cerr << "Could not find "<< filename << endl;
    }
    return result;
}


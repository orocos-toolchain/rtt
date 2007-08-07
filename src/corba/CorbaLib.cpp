#include "CorbaTemplateProtocol.hpp"
#include <os/StartStopManager.hpp>

namespace RTT {
    namespace Corba {
        
        using namespace RTT::detail;

        struct CorbaLibRegistrator
            : public TransportRegistrator
        {
            bool registerTransport(std::string name, TypeInfo* ti)
            {
                assert( name == ti->getTypeName() );
                if ( name == "int" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<int>() );
                if ( name == "double" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<double>() );
                if ( name == "string" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<std::string>() );
                if ( name == "float" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<float>() );
                if ( name == "unsigned int" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<unsigned int>() );
                //if ( name == "long" )
                //    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<long>() );
                if ( name == "PropertyBag" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<PropertyBag>() );
                if ( name == "bool" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol<bool>() );
                if ( name == "array" )
                    return ti->addProtocol(ORO_CORBA_PROTOCOL_ID, new CorbaTemplateProtocol< std::vector<double> >() );
                return false;
            }

            std::string getTransportName() const {
                return "CORBA";
            }

        } CorbaLibRegistrator;

        /**
         * This struct has the sole purpose of invoking
         * the Import function.
         */
        int loadCorbaLib()
        {
            log(Info) << "Loading CorbaLib in RTT type system." <<endlog();
            TypeInfoRepository::Instance()->registerTransport( &CorbaLibRegistrator );
            return 0;
        }

        OS::InitFunction CorbaLoader( &loadCorbaLib );
    }

    
}



#include "MQLib.hpp"
#include "MQTemplateProtocol.hpp"
#include "../../plugin/TransportPlugin.hpp"
#include "../../types/Toolkit.hpp"
#include "../../os/StartStopManager.hpp"

using namespace std;
using namespace RTT::detail;

namespace RTT {
    namespace mqueue {

        struct MQLibPlugin
            : public TransportPlugin
        {
            bool registerTransport(std::string name, TypeInfo* ti)
            {
                assert( name == ti->getTypeName() );
                if ( name == "int" )
                    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<int>() );
                if ( name == "double" )
                    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<double>() );
                //if ( name == "string" )
                //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<std::string>() );
                if ( name == "float" )
                    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<float>() );
                if ( name == "uint" )
                    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<unsigned int>() );
                //if ( name == "long" )
                //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<long>() );
                //if ( name == "PropertyBag" )
                //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<PropertyBag>() );
                if ( name == "bool" )
                    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol<bool>() );
                //if ( name == "array" )
                //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQTemplateProtocol< std::vector<double> >() );
                //if ( name == "void" )
                //    return ti->addProtocol(ORO_MQUEUE_PROTOCOL_ID, new MQFallBackProtocol(false)); // warn=false
                return false;
            }

            std::string getTransportName() const {
                return "MQueue";
            }

            std::string getName() const {
                return "MQueueRealtime";
            }

        } MQLibPlugin;

        /**
         * This struct has the sole purpose of invoking
         * the Import function.
         */
        int loadMQLib()
        {
            Toolkit::Import(MQLibPlugin);
            // register fallback also.
            //DataSourceTypeInfo<UnknownType>::getTypeInfo()->addProtocol( ORO_MQUEUE_PROTOCOL_ID, new MQFallBackProtocol() );
            return 0;
        }

        os::InitFunction MQLoader( &loadMQLib );
    }


}



#include "rtt-config.h"
#include "RealTimeTypekit.hpp"
#include "Types.hpp"
#include "TemplateTypeInfo.hpp"
#include "SequenceTypeInfo.hpp"
#include "StdTypeInfo.hpp"
#include "StdVectorTypeInfo.hpp"
#include "StructTypeInfo.hpp"

#include "../rtt-fwd.hpp"
#include "../FlowStatus.hpp"
#include "../ConnPolicy.hpp"
#include "ConnPolicyType.hpp"

namespace RTT
{
    using namespace std;
    using namespace detail;


     namespace types {
         /**
          * This is a placeholder for scripting purposes.
          */
         struct EmptySendHandle {};

         // load the Orocos specific types:
         void loadOrocosTypes( TypeInfoRepository::shared_ptr ti ) {
             ti->addType( new StdTypeInfo<FlowStatus>("FlowStatus"));
             ti->addType( new StdTypeInfo<SendStatus>("SendStatus"));
             ti->addType( new TemplateTypeInfo<PropertyBag, true>("PropertyBag") );
             ti->addType( new StdVectorTypeInfo("array") );
             ti->addType( new StructTypeInfo<ConnPolicy,false>("ConnPolicy") );
             ti->addType( new TemplateTypeInfo<EmptySendHandle>("SendHandle") ); //dummy, replaced by real stuff when seen by parser.
         }
     }
}

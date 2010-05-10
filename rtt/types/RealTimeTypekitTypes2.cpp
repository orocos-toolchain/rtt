
#include "rtt-config.h"
#include "RealTimeToolkit.hpp"
#include "Types.hpp"
#include "TemplateTypeInfo.hpp"
#include "SequenceTypeInfo.hpp"
#include "StdTypeInfo.hpp"
#include "TypeInfoName.hpp"
#include "../rtt-fwd.hpp"
#include "../FlowStatus.hpp"
#include "../ConnPolicy.hpp"

#include "TypeStream.hpp"
#include "../PropertyBag.hpp"
#include "VectorComposition.hpp"
#include <ostream>


namespace RTT
{
    using namespace std;
    using namespace detail;

    /**
     * This class tells Orocos how to handle std::vector<double>.
     */
    struct StdVectorTypeInfo
        : public SequenceTypeInfo<std::vector<double>, true >
    {
        StdVectorTypeInfo( std::string name )
            : SequenceTypeInfo<std::vector<double>, true >(name)
        {}

        bool decomposeTypeImpl(const std::vector<double>& vec, PropertyBag& targetbag) const
        {
            decomposeProperty( vec, targetbag );
            return true;
        }

        bool composeTypeImpl(const PropertyBag& bag, std::vector<double>& result) const
        {
            return composeProperty( bag, result );
        }
    };

        /**
         * This class tells Orocos how to handle ConnPolicy
         */
        struct ConnPolicyTypeInfo
            : public TemplateTypeInfo<ConnPolicy>
        {
            ConnPolicyTypeInfo(const std::string& name )
                : TemplateTypeInfo<ConnPolicy>(name)
            {}

            bool decomposeTypeImpl(const ConnPolicy& cp, PropertyBag& targetbag) const
            {
                decomposeProperty( cp, targetbag );
                return true;
            }

            bool composeTypeImpl(const PropertyBag& bag, ConnPolicy& result) const
            {
                return composeProperty( bag, result );
            }
        };

        /**
         * This is a placeholder for scripting purposes.
         */
        struct EmptySendHandle {};

     namespace types {
         // load the Orocos specific types:
         void loadOrocosTypes( TypeInfoRepository::shared_ptr ti ) {
             ti->addType( new StdTypeInfo<FlowStatus>("FlowStatus"));
             ti->addType( new StdTypeInfo<SendStatus>("SendStatus"));
             ti->addType( new TemplateTypeInfo<PropertyBag, false>("PropertyBag") );
             ti->addType( new StdVectorTypeInfo("array") );
             ti->addType( new ConnPolicyTypeInfo("ConnPolicy") );
             ti->addType( new TemplateTypeInfo<EmptySendHandle>("SendHandle") ); //dummy, replaced by real stuff when seen by parser.
         }
     }
}

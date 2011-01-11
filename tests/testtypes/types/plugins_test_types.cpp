
#include <rtt/types/Types.hpp>
#include <rtt/types/TypekitPlugin.hpp>
#include "types/SequenceTypeInfo.hpp"


class TypesPluginTest : public RTT::types::TypekitPlugin
{
public:
    virtual bool loadTypes() {
        RTT::types::Types()->addType(new RTT::types::SequenceTypeInfo<std::vector<std::vector<double> > >("matrix") );
        return true;
    }

    virtual bool loadOperators() {
        return true;
    }

    virtual bool loadConstructors() {
        return true;
    }

    virtual bool loadGlobals() { return true; }

    virtual std::string getName() { return "TypesTest"; }

};

ORO_TYPEKIT_PLUGIN( TypesPluginTest )

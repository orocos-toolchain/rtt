
#include "rtt/types/Types.hpp"
#include "rtt/types/TypekitPlugin.hpp"
#include "rtt/types/SequenceTypeInfo.hpp"
#include "tests/datasource_fixture.hpp"

void loadStructTypes();
void loadSequenceTypes();
void loadArrayTypes();

class TypesPluginTest : public RTT::types::TypekitPlugin
{
public:
    virtual bool loadTypes() {
        loadStructTypes();
        loadSequenceTypes();
        loadArrayTypes();
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

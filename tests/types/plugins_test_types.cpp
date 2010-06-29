/**
 * plugins_test_types.cpp
 *
 *  Created on: May 25, 2010
 *      Author: kaltan
 */

#include <rtt/types/TypekitPlugin.hpp>

class TypesPluginTest : public RTT::types::TypekitPlugin
{
public:
    virtual bool loadTypes() {
        return true;
    }

    virtual bool loadOperators() {
        return true;
    }

    virtual bool loadConstructors() {
        return true;
    }

    virtual bool loadGlobals() { return true; }

    virtual std::string getName() { return "TypesPluginTest"; }

};

ORO_TYPEKIT_PLUGIN( TypesPluginTest )

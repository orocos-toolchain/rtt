#ifndef COMPONENTCONFIGURATOR_HPP
#define COMPONENTCONFIGURATOR_HPP
#include <string>

namespace ORO_ControlKernel
{
    using namespace std;

    class PropertyComponentInterface;
    
    /**
     * A class for reading component property files (cpf)
     * and configuring a component with these properties.
     */
    class ComponentConfigurator
    {
        public:
            ComponentConfigurator()
            {}

            /**
             * Read the XML cpf files and try to configure the given component.
             */
            virtual bool configure(std::string& filename, PropertyComponentInterface* target);

    };
}
#endif

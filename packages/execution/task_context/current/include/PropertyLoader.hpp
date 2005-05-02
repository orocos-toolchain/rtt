#ifndef ORO_PROPERTY_LOADER_HPP
#define ORO_PROPERTY_LOADER_HPP

#include <string>

namespace ORO_Execution
{
    /**
     * This class loads and saves property files
     * to a TaskContext's AttributeRepository.
     * The default file format is 'cpf' from the
     * ORO_CoreLib::CPFMarshaller class.
     */
    class PropertyLoader
    {
    public:

        /**
         * Read the XML cpf file and try to update the attributes of the given TaskContext.
         */
        bool configure(const std::string& filename, TaskContext* target) const;

        /**
         * Write the XML cpf file with the attributes of the given TaskContext.
         */
        bool save(const std::string& filename, TaskContext* target) const;

    };
}

#endif

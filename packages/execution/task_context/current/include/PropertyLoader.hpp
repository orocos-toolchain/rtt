#ifndef ORO_PROPERTY_LOADER_HPP
#define ORO_PROPERTY_LOADER_HPP

#include <string>
#include "TaskContext.hpp"

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
         * Read the XML cpf file and 'refresh' the matching attributes of the given TaskContext.
         * There may be more properties in the file than attributes in the target.
         * @param filename The file to read from.
         * @param target   The TaskContext to configure.
         * @param strict   Do not change any target Property if something goes wrong
         * @return true on success, false on error, consult ORO_CoreLib::Logger output for messages.
         */
        bool configure(const std::string& filename, TaskContext* target, bool strict = true) const;

        /**
         * Write the XML cpf file with the attributes of the given TaskContext.
         * The file is first read in, the resulting tree is updated with the task's
         * attributes and then written to disk again. This allows to share files
         * between tasks.
         * @param filename The file to read from and write to (updating).
         * @param target   The TaskContext to configure.
         * @return true on success, false on error, consult ORO_CoreLib::Logger output for messages.
         */
        bool save(const std::string& filename, TaskContext* target) const;

    };
}

#endif

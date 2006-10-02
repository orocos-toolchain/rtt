 
#ifndef ORO_MARSHALLING_ACCESS_HPP
#define ORO_MARSHALLING_ACCESS_HPP

#include <string>
#include "TaskContext.hpp"

namespace RTT
{
    /**
     * Load and save properties of a TaskContext.
     */
    class MarshallingAccess
    {
        TaskContext* mparent;
    public:
        /**
         * This object is normally created by a TaskContext.
         */
        MarshallingAccess(TaskContext* parent);

        /**
         * Read the XML cpf file and 'refresh' the matching properties of a TaskContext.
         * There may be more properties in the file than properties in the TaskContext.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read from.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool readProperties(const std::string& filename) const;

        /**
         * Write the XML cpf file with the properties of a TaskContext.
         * The file is first read in, the resulting tree is updated with the task's
         * properties and then written to disk again. This allows to share files
         * between tasks.
         * @param filename The file to read from and write to (updating).
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool writeProperties(const std::string& filename) const;

    };
}

#endif

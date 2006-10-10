 
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
         * Read the XML cpf file and 'refresh' the matching properties of the TaskContext.
         * There may be more properties in the file than properties in the TaskContext.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read from.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool readProperties(const std::string& filename) const;

        /**
         * Read the XML cpf file and 'refresh' \b all properties of the TaskContext.
         * There may be more properties in the file than properties in the TaskContext,
         * but all properties of the TaskContext must be present in the file.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read all the properties from.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool readAllProperties(const std::string& filename) const;

        /**
         * Read a single property from a file. The name may be a 'path' like
         * location of a Property in the hierarchy.
         * @param name The name of the property in the file. For example
         * "MyProp" for a top-level property or "Settings::XML::Editor" to
         * specify a ::-separated 'path' to a property.
         * @param filename The name of the file in which to lookup \a name.
         */
        bool readProperty(const std::string& name,const std::string& filename);

        /**
         * Write the XML cpf file with the properties of a TaskContext, update
         * only properties present in the file.
         * The file is first read in, the resulting tree is updated with the task's
         * properties and then written to disk again. This allows to share files
         * between tasks.
         * @param filename The file to read from and write to (updating).
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool writeProperties(const std::string& filename) const;

        /**
         * Write the XML cpf file with all the properties of a TaskContext, add
         * new entries in the file if necessary.
         * The file is first read in, the resulting tree is updated with the task's
         * properties, new entries are added if missing and then written to disk again.
         * @param filename The file to read from and write to.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool writeAllProperties(const std::string& filename) const;

        /**
         * Write a single property to a file. The name may be a 'path' like
         * location of a Property in the hierarchy.
         * @param name The name of the property in the file. For example
         * "MyProp" for a top-level property or "Settings::XML::Editor" to
         * specify a ::-separated 'path' to a property.
         * @param filename The name of the file in which to write \a name.
         */
        bool writeProperty(const std::string& name, const std::string& filename);

    };
}

#endif

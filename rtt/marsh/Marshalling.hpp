#ifndef ORO_MARSHALLING_HPP_
#define ORO_MARSHALLING_HPP_

#include <string>
#include "../marsh/rtt-marsh-config.h"
#include "../rtt-fwd.hpp"
#include "../ServiceRequester.hpp"
#include "../OperationCaller.hpp"

namespace RTT
{
    /**
     * Service requester to load and save properties of a TaskContext.
     */
    class RTT_MARSH_API Marshalling
    : public ServiceRequester
    {
        TaskContext* mparent;
    public:
        /**
         * This object is normally created by a TaskContext.
         */
        Marshalling(TaskContext* parent);

        ~Marshalling();

        /**
         * Read a property file and update (or create any missing) properties in the
         * TaskContext. This function allows to dynamically add properties to a
         * component. This can be useful in combination with Orocos scripts when
         * the script requires additional properties from the parent TaskContext.
         */
        OperationCaller<bool (const std::string& filename)> loadProperties;

        /**
         * Read the property file and 'refresh' \b all the properties of the TaskContext.
         * There may be more properties in the file than properties in the TaskContext,
         * but all properties of the TaskContext must be present in \a filename.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read from.
         * @return true on success, false on error, consult Logger output for messages.
         */
        OperationCaller<bool (const std::string& filename)> readProperties;

        /**
         * Read the property file and 'refresh' \b some properties of the TaskContext.
         * There may be more properties in the file than properties in the TaskContext,
         * and some may be missing in the file. This method will update 'as much as possible'
         * properties.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read all the properties from.
         * @return true on success, false on error, consult Logger output for messages.
         */
        OperationCaller<bool (const std::string& filename)> updateProperties;

        /**
         * Read a single property from a file. The name may be a 'path' like
         * location of a Property in the hierarchy.
         * @param name The name of the property in the file. For example
         * "MyProp" for a top-level property or "Settings.XML.Editor" to
         * specify a dot-separated 'path' to the property 'Editor'.
         * @param filename The name of the file in which to lookup \a name.
         */
        OperationCaller<bool (const std::string& name,const std::string& filename)> readProperty;

        /**
         * Write the property file with the properties of a TaskContext.
         * The file is first read in, the resulting tree is updated with the task's
         * properties and then written to disk again. This allows to share files
         * between tasks.
         * @param filename The file to read from and write to (updating).
         * @return true on success, false on error, consult Logger output for messages.
         */
        OperationCaller<bool (const std::string& filename)> writeProperties;

        /**
         * Write the property file with the properties of a TaskContext, which
         * are already present in \a filename.
         * No new entries are created in the file, only existing ones are updated.
         * The file is first read in, the resulting tree is updated with the task's
         * properties, and then written to disk again.
         * @param filename The file to read from and write to.
         * @return true on success, false on error, consult Logger output for messages.
         */
        OperationCaller<bool (const std::string& filename)> updateFile;

        /**
         * Write a single property to a file. The name may be a 'path' like
         * location of a Property in the hierarchy.
         * @param name The name of the property in the file. For example
         * "MyProp" for a top-level property or "Settings.XML.Editor" to
         * specify a dot-separated 'path' to the property 'Editor'.
         * @param filename The name of the file in which to write \a name.
         */
        OperationCaller<bool (const std::string& name, const std::string& filename)> writeProperty;

    };
}

#endif

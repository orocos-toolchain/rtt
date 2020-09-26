/***************************************************************************
  tag: FMTC  do nov 2 13:06:11 CET 2006  MarshallingService.hpp

                        MarshallingService.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#ifndef ORO_MARSHALLING_ACCESS_HPP
#define ORO_MARSHALLING_ACCESS_HPP

#include <string>
#include "rtt/marsh/rtt-marsh-config.h"
#include "rtt/rtt-fwd.hpp"
#include "rtt/Service.hpp"

namespace RTT
{ namespace marsh {

    /**
     * Service which loads and saves properties of a TaskContext.
     */
    class RTT_MARSH_API MarshallingService
        : public Service
    {
    public:
        typedef boost::shared_ptr<MarshallingService> shared_ptr;

        /**
         * Creates a MarshallingService object and registers
         * the service to \a parent.
         */
        static shared_ptr Create(TaskContext* parent);

        /**
         * Creates a MarshallingService object.
         * You need to add the service to \a parent yourself,
         * or use Create().
         * This object is normally created by a TaskContext.
         */
        MarshallingService(TaskContext* parent);

        /**
         * Read a property file and update (or create any missing) properties in the
         * TaskContext. This function allows to dynamically add properties to a
         * component. This can be useful in combination with Orocos scripts when
         * the script requires additional properties from the parent TaskContext.
         */
        bool loadProperties(const std::string& filename) const;

        /**
         * Stores all properties of a TaskContext in a new file or overwrite an existing one.
         * The file given in filename will always be overwritten and any existing content
         * will be lost.
         * @param filename The file to store to.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool storeProperties(const std::string& filename) const;

        /**
         * Read the property file and 'refresh' \b all the properties of the TaskContext.
         * There may be more properties in the file than properties in the TaskContext,
         * but all properties of the TaskContext must be present in \a filename.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read from.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool readProperties(const std::string& filename) const;

        /**
         * Read the property file and 'refresh' \b some properties of the TaskContext.
         * There may be more properties in the file than properties in the TaskContext,
         * and some may be missing in the file. This method will update 'as much as possible'
         * properties.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read all the properties from.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool updateProperties(const std::string& filename) const;

        /**
         * Read a single property from a file. The name may be a 'path' like
         * location of a Property in the hierarchy.
         * @param name The name of the property in the file. For example
         * "MyProp" for a top-level property or "Settings.XML.Editor" to
         * specify a dot-separated 'path' to the property 'Editor'.
         * @param filename The name of the file in which to lookup \a name.
         */
        bool readProperty(const std::string& name,const std::string& filename);

        /**
         * Write the property file with the properties of a TaskContext.
         * The file is first read in, the resulting tree is updated with the task's
         * properties and then written to disk again. This allows to share files
         * between tasks.
         * @param filename The file to read from and write to (updating).
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool writeProperties(const std::string& filename) const;

        /**
         * Write the property file with the properties of a TaskContext, which
         * are already present in \a filename.
         * No new entries are created in the file, only existing ones are updated.
         * The file is first read in, the resulting tree is updated with the task's
         * properties, and then written to disk again.
         * @param filename The file to read from and write to.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool updateFile(const std::string& filename) const;

        /**
         * Write a single property to a file. The name may be a 'path' like
         * location of a Property in the hierarchy.
         * @param name The name of the property in the file. For example
         * "MyProp" for a top-level property or "Settings.XML.Editor" to
         * specify a dot-separated 'path' to the property 'Editor'.
         * @param filename The name of the file in which to write \a name.
         */
        bool writeProperty(const std::string& name, const std::string& filename);

        /**
         * Read a property file and update (or create any missing) properties of a Service.
         * This function allows to dynamically add properties to a
         * component. This can be useful in combination with Orocos scripts when
         * the script requires additional properties from the Service.
         */
        bool loadServiceProperties(const std::string& filename, const std::string& servicename) const;

        /**
         * Stores all properties of a Service in a new file or overwrite an existing one.
         * The file given in filename will always be overwritten and any existing content
         * will be lost.
         * @param filename The file to store to.
         * @param servicename Service to use
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool storeServiceProperties(const std::string& filename, const std::string& servicename) const;

        /**
         * Read the property file and 'refresh' \b all the properties of a Service.
         * There may be more properties in the file than properties in the Service,
         * but all properties of the Service must be present in \a filename.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read from.
         * @param servicename Service to use
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool readServiceProperties(const std::string& filename, const std::string& servicename) const;

        /**
         * Read the property file and 'refresh' \b some properties of the Service.
         * There may be more properties in the file than properties in the Service,
         * and some may be missing in the file. This method will update 'as much as possible'
         * properties.
         * In case a type mismatch occurs, this method will fail and update no properties.
         * @param filename The file to read all the properties from.
         * @param servicename Service  to use
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool updateServiceProperties(const std::string& filename, const std::string& servicename) const;

        /**
         * Read a single property from a file. The name may be a 'path' like
         * location of a Property in the hierarchy.
         * @param name The name of the property in the file. For example
         * "MyProp" for a top-level property or "Settings.XML.Editor" to
         * specify a dot-separated 'path' to the property 'Editor'.
         * @param filename The name of the file in which to lookup \a name.
         * @param servicename Service  to use
         */
        bool readServiceProperty(const std::string& name,const std::string& filename, const std::string& servicename);

        /**
         * Write the property file with the properties of a Service.
         * The file is first read in, the resulting tree is updated with the task's
         * properties and then written to disk again. This allows to share files
         * between tasks.
         * @param filename The file to read from and write to (updating).
         * @param servicename Service  to use
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool writeServiceProperties(const std::string& filename, const std::string& servicename) const;

        /**
         * Write the property file with the properties of a Service, which
         * are already present in \a filename.
         * No new entries are created in the file, only existing ones are updated.
         * The file is first read in, the resulting tree is updated with the task's
         * properties, and then written to disk again.
         * @param filename The file to read from and write to.
         * @param servicename Service  to use
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool updateServiceFile(const std::string& filename, const std::string& servicename) const;

        /**
         * Write a single property to a file. The name may be a 'path' like
         * location of a Property in the hierarchy.
         * @param name The name of the property in the file. For example
         * "MyProp" for a top-level property or "Settings.XML.Editor" to
         * specify a dot-separated 'path' to the property 'Editor'.
         * @param filename The name of the file in which to write \a name.
         * @param servicename Service  to use
         */
        bool writeServiceProperty(const std::string& name, const std::string& filename, const std::string& servicename);

    };
}}

#endif

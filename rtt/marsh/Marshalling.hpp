/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  Marshalling.hpp

                        Marshalling.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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
         * Stores all properties of parent TaskContext in a new file or overwrite an existing one.
         * The file given in filename will always be overwritten and any existing content
         * will be lost.
         * @param filename The file to store to.
         * @return true on success, false on error, consult Logger output for messages.
         * @see loadProperties() for loading properties in a TaskContext.
         */
        OperationCaller<bool (const std::string& filename)> storeProperties;

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

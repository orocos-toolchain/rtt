/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  PropertyLoader.hpp

                        PropertyLoader.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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


#ifndef ORO_PROPERTY_LOADER_HPP
#define ORO_PROPERTY_LOADER_HPP

#include <string>
#include "rtt/marsh/rtt-marsh-config.h"
#include "rtt/rtt-fwd.hpp"

namespace RTT
{ namespace marsh {

    /**
     * Load and save property files to a Service's PropertyBag.
     * The default file format is 'cpf' from the CPFMarshaller class.
     */
    class RTT_MARSH_API PropertyLoader
    {
    private:
        Service* target;

    public:
        /**
         * Constructor.
         * @param task   The TaskContext to load the new properties into.
         */
        PropertyLoader(TaskContext *task);

        /**
         * Constructor.
         * @param service   The Service to load the new properties into.
         */
        PropertyLoader(Service *service);

        /**
         * Read the XML cpf file and create (or refresh the matching properties) of the given Service.
         * Any property in the file which is not in the target, is created in the target.
         * @param filename The file to read from.
         * @return true on success, false on error, consult Logger output for messages.
         * @see store() for dumping properties in a file.
         */
        bool load(const std::string& filename) const;

        /**
         * Stores all properties of a Service in a new file or overwrite an existing one.
         * The file given in filename will always be overwritten and any existing content
         * will be lost.
         * @param filename The file to store to.
         * @return true on success, false on error, consult Logger output for messages.
         * @see load() for loading properties in a Service.
         */
        bool store(const std::string& filename) const;

        /**
         * Read the XML cpf file and 'refresh' the matching properties of the given Service.
         * There may be more properties in the file than properties in the target.
         * @param filename The file to read from.
         * @param all   Configure all properties of \a target. Return an error
         * if not all properties were found in \a filename.
         * @return true on success, false on error, consult Logger output for messages.
         * @see save() to create this file.
         */
        bool configure(const std::string& filename, bool all = true) const;

        /**
         * Write the XML cpf file with the properties of the given Service.
         * The file is first read into memory, the resulting tree is updated with the task's
         * properties and then written to disk again. This allows to share files
         * between tasks.
         * @param filename The file to read from and write to (updating).
         * @param all   Write out all properties of \a target to \a filename,
         * add missing ones if necessary.
         * @return true on success, false on error, consult Logger output for messages.
         * @see configure() to re-read this file and update matching properties.
         */
        bool save(const std::string& filename, bool all = true) const;

        /**
         * Read a single property from a file.
         * @param filename The file to read from.
         * @param name The path to or name of a property in \a target.
         * Separate path components with dots. For example, to safe property x: bag_1.bag_2.x
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool configure(const std::string& filename, const std::string& name) const;

        /**
         * Write a single property to a file, or update an existing file.
         * @param filename The file to update or write to.
         * @param name The path to or name of a property in \a target.
         * Separate path components with dots. For example, to safe property x: bag_1.bag_2.x
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool save(const std::string& filename, const std::string& name) const;
    };
}}

#endif

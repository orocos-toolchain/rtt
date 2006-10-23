/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  PropertyLoader.hpp 

                        PropertyLoader.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_PROPERTY_LOADER_HPP
#define ORO_PROPERTY_LOADER_HPP

#include <string>
#include "TaskContext.hpp"

namespace RTT
{
    /**
     * Load and save property files
     * to a TaskContext's PropertyBag.
     * The default file format is 'cpf' from the
     * CPFMarshaller class. You can select
     * another file format using the configuration tool
     * in CoreLib->Properties->Marshalling->Default.
     */
    class PropertyLoader
    {
    public:

        /**
         * Read the XML cpf file and 'refresh' the matching attributes of the given TaskContext.
         * There may be more properties in the file than attributes in the target.
         * @param filename The file to read from.
         * @param target   The TaskContext to configure.
         * @param all   Configure all properties of \a target. Return an error
         * if not all properties were found in \a filename.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool configure(const std::string& filename, TaskContext* target, bool all = true) const;

        /**
         * Write the XML cpf file with the attributes of the given TaskContext.
         * The file is first read in, the resulting tree is updated with the task's
         * attributes and then written to disk again. This allows to share files
         * between tasks.
         * @param filename The file to read from and write to (updating).
         * @param target   The TaskContext to configure.
         * @param all   Write out all properties of \a target to \a filename,
         * add missing ones if necessary.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool save(const std::string& filename, TaskContext* target, bool all = true) const;

        /**
         * Read a single property from a file.
         * @param filename The file to read from.
         * @param target   The TaskContext to configure.
         * @param name The path to or name of a property in \a target.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool configure(const std::string& filename, TaskContext* target, const std::string& name) const;

        /**
         * Write a single property to a file, or update an existing file.
         * @param filename The file to update or write to.
         * @param target   The TaskContext to configure.
         * @param name The path to or name of a property in \a target.
         * @return true on success, false on error, consult Logger output for messages.
         */
        bool save(const std::string& filename, TaskContext* target, const std::string& name) const;
    };
}

#endif

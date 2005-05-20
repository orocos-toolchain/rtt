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

namespace ORO_Execution
{
    /**
     * Load and save property files
     * to a TaskContext's AttributeRepository.
     * The default file format is 'cpf' from the
     * ORO_CoreLib::CPFMarshaller class. You can select
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

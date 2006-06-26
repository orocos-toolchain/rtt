/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  Toolkit.hpp 

                        Toolkit.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_TOOLKIT_HPP
#define ORO_TOOLKIT_HPP

#include <string>
#include <vector>
#include "RTT.hpp"

namespace ORO_CoreLib
{

    /**
     * This interface defines how additional toolkits
     * are loaded into Orocos.
     * A ToolkitPlugin defines additional user data types for a Toolkit.
     */
    class ToolkitPlugin
    {
    public:
        virtual ~ToolkitPlugin() {}

        /**
         * Implement this method to add types to the 
         * Orocos type system.
         */
        virtual bool loadTypes() = 0;

        /**
         * Implement this method to load Scripting
         * operators on types, such as '+', '*', ...
         */
        virtual bool loadOperators() = 0;

        /**
         * Each plugin must have a unique name.
         */
        virtual std::string getName() = 0;
    };

    /**
     * This class allows the Orocos Real-Time Toolkit to be extended
     * with additional toolkits (ToolkitPlugin), such as for kinematics or any
     * library foreign to Orocos.
     */
    class Toolkit
    {
        static std::vector<ToolkitPlugin*> Tools;
    public:
        /**
         * Import a plugin into Orocos.
         * One may try to load the same plugin multiple
         * times from different places. Only the first
         * import will succeed. Multiple imports will not
         * be considered as an error.
         */
        static void Import( ToolkitPlugin& tkp );

        /**
         * Get the names of all imported tools.
         */
        static std::vector<std::string> getTools();

        /**
         * Check if a tool with given name was already imported.
         */
        static bool hasTool( const std::string& toolname );
    };

}

#endif

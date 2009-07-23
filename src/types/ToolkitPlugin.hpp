/***************************************************************************
 Copyright (c) 2009 S Roderick <xxxkiwi DOT xxxnet AT macxxx DOT comxxx>
                               (remove the x's above)

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


#ifndef ORO_TOOLKITPLUGIN_HPP
#define ORO_TOOLKITPLUGIN_HPP 1

#include <string>
#include <vector>
#include "rtt-config.h"

namespace RTT
{

    /**
     * This interface defines how additional toolkits
     * are loaded into Orocos.
     * A ToolkitPlugin defines additional user data types for a Toolkit.
     * @ingroup CoreLib
     */
    class RTT_API ToolkitPlugin
    {
    public:
        virtual ~ToolkitPlugin() {}

        /**
         * Implement this method to add types to the
         * Orocos type system.
         * @see The TypeRepository for adding the types.
         * @see The TypeInfo class for the interface of a 'type'
         * @see The TemplateTypeInfo class for easy addition of new user types
         * in scripting, XML,...
         * @see The TypeInfoName class for only 'name' addition of a user type
         * @see The TemplateIndexTypeInfo for adding container types.
         * @see The RealTimeToolkit for an implementation example.
         */
        virtual bool loadTypes() = 0;

        /**
         * Implement this method to load Scripting
         * operators on types, such as '+', '*', ...
         * @see The OperatorRepository for adding the Operators
         * @see The detail::UnaryOp, detail::BinaryOp and detail::DotOp
         * classes for the interface of an operation.
         * @see The newUnaryOperator, newBinaryOperator and newDotOperator
         * functions for creating new operator objects.
         * @see The RealTimeToolkit for an implementation example.
         */
        virtual bool loadOperators() = 0;

        /**
         * Implement this method to load Scripting
         * constructors of types, such as in C++.
         * Default constructors (which take no arguments)
         * need not to be added.
         * @see The TypeInfo class for adding a constructor to a type
         * @see The TypeBuilder class for the interface of a constructor.
         * @see The newConstructor utility function for creating a new
         * constructor object.
         * @see The RealTimeToolkit for an implementation example.
         */
        virtual bool loadConstructors() = 0;

        /**
         * Each plugin must have a unique name.
         */
        virtual std::string getName() = 0;
    };

}

#endif

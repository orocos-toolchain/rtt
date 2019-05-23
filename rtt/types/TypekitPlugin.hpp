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


#ifndef ORO_TYPEKITPLUGIN_HPP
#define ORO_TYPEKITPLUGIN_HPP 1

#include <string>
#include <vector>
#include "../rtt-config.h"

namespace RTT
{ namespace types {


    /**
     * This interface defines how additional typekits
     * are loaded into Orocos.
     * A TypekitPlugin defines additional user data types for a Typekit.
     * @ingroup Typekit
     */
    class RTT_API TypekitPlugin
    {
    public:
        virtual ~TypekitPlugin() {}

        /**
         * Implement this method to add types to the
         * Orocos type system.
         * @see The TypeRepository for adding the types.
         * @see The TypeInfo class for the interface of a 'type'
         * @see The TemplateTypeInfo class for easy addition of new user types
         * in scripting, XML,...
         * @see The TypeInfoName class for only 'name' addition of a user type
         * @see The TemplateIndexTypeInfo for adding container types.
         * @see The RealTimeTypekit for an implementation example.
         */
        virtual bool loadTypes() = 0;

        /**
         * Implement this method to load Scripting
         * operators on types, such as '+', '*', ...
         * @see The OperatorRepository for adding the Operators
         * @see The UnaryOp, BinaryOp and DotOp
         * classes for the interface of an operation.
         * @see The newUnaryOperator, newBinaryOperator and newDotOperator
         * functions for creating new operator objects.
         * @see The RealTimeTypekit for an implementation example.
         */
        virtual bool loadOperators() = 0;

        /**
         * Implement this method to load Scripting
         * constructors of types, such as in C++.
         * Default constructors (which take no arguments)
         * need not to be added.
         * @see The TypeInfo class for adding a constructor to a type
         * @see The TypeConstructor class for the interface of a constructor.
         * @see The newConstructor utility function for creating a new
         * constructor object.
         * @see The RealTimeTypekit for an implementation example.
         */
        virtual bool loadConstructors() = 0;

        /**
         * Implement this function to add global variables to the type system.
         * This is necessary to have something like enumeration values, without
         * being force to look these up in a component.
         */
        virtual bool loadGlobals() { return true; }

        /**
         * Each plugin must have a unique name.
         * This name is used globally in the process to identify
         * this instance.
         */
        virtual std::string getName() = 0;
    };

}}

#include "TypekitRepository.hpp"

// Disable foo() has C-linkage specified, but returns UDT 'bar' which is incompatible with C
#ifdef _MSC_VER
#pragma warning (disable:4190)
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif

/**
 * Once you defined your TypekitPlugin or TransportPlugin class,
 * you can use this macro to make it available as a
 * plugin.
 * @note Do not use this macro inside a namespace !
 * For example: ORO_TYPEKIT_PLUGIN ( KDL::KDLTypekit )
 * where KDL::KDLTypekit is a *classname*, derived from
 * RTT::types::TypekitPlugin or RTT::types::TransportPlugin
 */
#define ORO_TYPEKIT_PLUGIN( TYPEKIT ) \
    namespace RTT { class TaskContext; } \
    extern "C" {                      \
        RTT_EXPORT bool loadRTTPlugin(RTT::TaskContext* tc); \
        bool loadRTTPlugin(RTT::TaskContext* tc) { \
            if (tc == 0) { \
                RTT::types::TypekitRepository::Import( new TYPEKIT() ); \
                return true; \
            } \
            return false; \
        } \
        RTT_EXPORT std::string getRTTPluginName(); \
        std::string getRTTPluginName() { \
        TYPEKIT tk; \
        return tk.getName(); \
        } \
        RTT_EXPORT std::string getRTTTargetName(); \
        std::string getRTTTargetName() { \
            return OROCOS_TARGET_NAME; \
        } \
    }

#endif

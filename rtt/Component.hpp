/***************************************************************************
  tag: Peter Soetens  Thu Jul 3 15:35:28 CEST 2008  Component.hpp

                        Component.hpp -  description
                           -------------------
    begin                : Thu July 03 2008
    copyright            : (C) 2008 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


/**
 * @file Component.hpp
 *
 * This file contains the macros and definitions to create dynamically
 * loadable components. You need to include this header and use one
 * of the macros if you wish to make a run-time loadable component.
 */

#ifndef RTT_COMPONENT_HPP
#define RTT_COMPONENT_HPP

#include <string>
#include <map>
#include <vector>
#include "rtt-fwd.hpp"
#include "rtt-config.h"

#include <rtt/deployment/ComponentLoader.hpp>

// Helper macros.
#define ORO_CONCAT_LINE2(x,y) x##y
#define ORO_CONCAT_LINE1(x,y) ORO_CONCAT_LINE2(x,y)
#define ORO_CONCAT_LINE(x) ORO_CONCAT_LINE1(x,__LINE__)

#define ORO_LIST_COMPONENT_TYPE_str(s) ORO_LIST_COMPONENT_TYPE__str(s)
#define ORO_LIST_COMPONENT_TYPE__str(s) #s

#if defined(__GNUC__)
    #define ORO_PP_PRAGMA(x) _Pragma(#x)
    #define ORO_PP_WARNING(text) ORO_PP_PRAGMA(GCC warning text)
    #define ORO_PP_ERROR(text) ORO_PP_PRAGMA(GCC error text)
#else
    #define ORO_PP_WARNING(text)
    #define ORO_PP_ERROR(text)
#endif

/**
 * Use this macro to register multiple components in a shared library (plug-in).
 * For each component, add this line in the .cpp file. Use this macro in combination with
 * ORO_CREATE_COMPONENT_LIBRARY.
 *
 * The advantage of this approach is that one library can create different component
 * \a types and that you may link multiple component libraries with each other.
 *
 * This macro can be used for both shared and static libraries. In case of a shared library,
 * the component factory will be registered to the shared library's local FactoryMap. In case
 * of a static library, the component factory will be registered in the static library's global
 * FactoryMap. In both cases, the DeploymentComponent can access these factories and
 * create the registered component types.
 *
 * @param CLASS_NAME the class name of the component you are adding to the library.
 */

#define ORO_LIST_COMPONENT_TYPE(CLASS_NAME) namespace { namespace ORO_CONCAT_LINE(LOADER_) { RTT::ComponentFactoryLoader<CLASS_NAME> m_cloader(ORO_LIST_COMPONENT_TYPE_str(CLASS_NAME)); } }


// ORO_CREATE_COMPONENT and ORO_CREATE_COMPONENT_LIBRARY are only used in shared libraries.
#if defined(OCL_DLL_EXPORT) || defined (RTT_COMPONENT)

#ifdef _MSC_VER
#pragma warning (disable:4190)
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif

/**
 * Use this macro to create a component library which contains all components listed with
 * ORO_LIST_COMPONENT_TYPE. 
 *
 * It will add to your library an extern "C" function 'createComponentType' which can create a component of
 * each class added with ORO_LIST_COMPONENT_TYPE.
 */
#define ORO_CREATE_COMPONENT_LIBRARY() \
RTT::FactoryMap* RTT::ComponentFactories::Factories = 0;	\
extern "C" { \
  RTT_EXPORT RTT::TaskContext* createComponentType(std::string instance_name, std::string type_name) \
  { \
    if( RTT::ComponentFactories::Instance().count(type_name) ) \
      return RTT::ComponentFactories::Instance()[type_name](instance_name); \
    return 0; \
  } \
  RTT_EXPORT std::vector<std::string> getComponentTypeNames() \
  { \
    std::vector<std::string> ret; \
    RTT::FactoryMap::iterator it; \
    for(it = RTT::ComponentFactories::Instance().begin(); it != RTT::ComponentFactories::Instance().end(); ++it) { \
        ret.push_back(it->first); \
    } \
    return ret; \
  } \
  RTT_EXPORT RTT::FactoryMap* getComponentFactoryMap() { return &RTT::ComponentFactories::Instance(); } \
} /* extern "C" */


/**
 * Use this macro to register a single component in a shared library (plug-in).
 * You can only use this macro \b once in a .cpp file for the whole shared library \b and
 * you may \b not link with another component library when using this macro. Use
 * ORO_CREATE_COMPONENT_LIBRARY if you are in that situation.
 *
 * It adds a function 'createComponent', which will return a new instance of
 * the library's component type and a function 'getComponentType', which returns
 * the type (namespace::class) name of the component.
 *
 * The advantage of this approach is that the user does not need to know the
 * class name of the component, he just needs to locate the shared library itself.
 * The disadvantage is that only one component \a type per shared library can be created.
 *
 * @param CNAME the class name of the component you are adding to the library.
 */
#define ORO_CREATE_COMPONENT(CNAME) \
ORO_LIST_COMPONENT_TYPE(CNAME) \
ORO_CREATE_COMPONENT_LIBRARY()

#else

#if !defined(OCL_STATIC) && !defined(RTT_STATIC) && !defined(RTT_DLL_EXPORT)
#error "You're compiling with static library settings. The resulting component library \
 will not be loadable at runtime with the deployer.\
 Compile with -DRTT_COMPONENT to enable dynamic loadable components, \
 or use -DRTT_STATIC to suppress this warning."
#endif

// Static OCL library:
// Identical to ORO_LIST_COMPONENT_TYPE:
#define ORO_CREATE_COMPONENT(CLASS_NAME) ORO_LIST_COMPONENT_TYPE(CLASS_NAME)
#define ORO_CREATE_COMPONENT_LIBRARY() __attribute__((weak)) RTT::FactoryMap* RTT::ComponentFactories::Factories = 0;

#endif

/**
 * Backwards compatibility macro which is now replaced by ORO_CREATE_COMPONENT_LIBRARY( )
 */
#define ORO_CREATE_COMPONENT_TYPE( ) \
    ORO_PP_WARNING("ORO_CREATE_COMPONENT_TYPE() is deprecated. Please use ORO_CREATE_COMPONENT_LIBRARY() instead.") \
    ORO_CREATE_COMPONENT_LIBRARY( )

#endif

//#undef ORO_CLOADER_CONCAT

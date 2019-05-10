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

#ifndef ORO_PLUGIN_HPP
#define ORO_PLUGIN_HPP 1

// Disable foo() has C-linkage specified, but returns UDT 'bar' which is incompatible with C
#ifdef _MSC_VER
#pragma warning (disable:4190)
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif

/**
 * @file Plugin.hpp
 * This file defines the Orocos plugin API.
 * A plugin is a dynamic library which has a unique name and
 * can be loaded in a running process. In case the loading
 * is done in an Orocos TaskContext, the plugin is notified of
 * the TaskContext. A plugin can reject to load in a process, in which
 * case the library will be unloaded from the process again.
 * Once loaded, a plugin remains in the current process until the
 * process exits.
 */
#include <string>
#include "../rtt-config.h"
namespace RTT {
    class TaskContext;
}
extern "C" {
    /**
     * Instructs this plugin to load itself into the process or a component.
     * This function will first be called with \a t being equal to zero, giving
     * the plugin the opportunity to load something in the whole process.
     * Implement in this function any startup code your plugin requires.
     * This function should not throw.
     *
     * @param t The optional TaskContext which is loading this plugin.
     * Is zero when the plugin is loaded into the process, non-zero
     * when loaded in a TaskContext. A plugin may choose to load only
     * in the process and not in a TaskContext (typekits for example). If
     * a plugin only wants to load in a TaskContext, it must return true when
     * t is zero, such that the plugin remains loaded in the process.
     * @return true if the loading succeeded, false otherwise.
     */
    RTT_EXPORT bool loadRTTPlugin( RTT::TaskContext* t );

    /**
     * Return the unique name of this plugin. No two plugins with
     * the same name will be allowed to live in a single process.
     */
    RTT_EXPORT std::string getRTTPluginName();

    /**
     * Returns the target name for which this plugin was built.
     *
     * @return The name as set by OROCOS_TARGET. When the empty
     * string is returned, it is assumed that the loadRTTPlugin
     * function will check if this plugin may be loaded or not.
     */
    RTT_EXPORT std::string getRTTTargetName();
}

#endif

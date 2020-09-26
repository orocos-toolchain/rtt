/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  PluginLoader.hpp

                        PluginLoader.hpp -  description
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


#ifndef ORO_PLUGINLOADER_HPP_
#define ORO_PLUGINLOADER_HPP_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "rtt/rtt-fwd.hpp"
#include "rtt/rtt-config.h"
#include "rtt/os/Mutex.hpp"

namespace RTT {
    namespace plugin {
        /**
         * Loads plugins found on the filesystem and keeps track of found plugins, typekits and services.
         * It has no facilities of unloading plugins. Plugins remain in memory until the process
         * terminates.
         *
         * @section paths Plugin Paths
         * The PluginLoader reads the RTT_COMPONENT_PATH variable when the application starts
         * and stores it using setPluginPath(). Paths are scanned in this order:
         *
         * ** First the paths specified by the function argument \a path_list if the function takes such argument
         * ** Second the paths specified using the setPluginPath() function.
         *
         * If neither is specified, it looks for plugins in the current directory (".").
         *
         * @see Plugin.hpp
         */
        class RTT_API PluginLoader
        {
            /**
             * Keep a list of all loaded libraries such that double
             * loads are avoided during import/loadLibrary.
             */
            class LoadedLib{
            public:
                LoadedLib(std::string n, std::string short_name, void* h)
                : filename(n), shortname(short_name), handle(h), loadPlugin(0), is_typekit(false), is_service(false)
                {
                }
                /**
                 * File name of the library.
                 */
                std::string filename;
                /**
                 * Short name of the library (without lib/dll/so)
                 */
                std::string shortname;
                /**
                 * Advertised name by the plugin.
                 */
                std::string plugname;
                void* handle;
                bool (*loadPlugin)(RTT::TaskContext*);
                RTT::ServicePtr (*createService)(void);
                bool is_typekit, is_service;
            };

            std::vector< LoadedLib > loadedLibs;

            /**
             * Path to look for if all else fails.
             */
            std::string plugin_path;

            /**
             * Protects for concurrent access of this shared object.
             */
            mutable os::MutexRecursive listlock;

            /**
             * Internal function that does all library loading.
             * @param filename The path+filename to open
             * @param shortname The short name of this file
             * @param kind The kind of plugin to load: 'plugin' or 'typekit'.
             * @param log_error Log errors to users. Set to false in case you are poking
             * files to see if they can be loaded.
             * @return true if a new library was loaded or if this library was already loaded.
             */
            bool loadInProcess(std::string filename, std::string shortname, std::string kind, bool log_error );
            /**
             * Helper function for loadTypekit and loadPlugin.
             * @param name
             * @param path_list
             * @param subdir
             * @param kind
             * @return false if no plugins were found
             * @throw std::runtime_exception if one of the found plugins refused to load.
             */
            bool loadPluginInternal( std::string const& name, std::string const& path_list, std::string const& subdir, std::string const& kind );
            /**
             * Helper function for loadTypekits and loadPlugins.
             * @param path_list
             * @param subdir
             * @param kind
             * @return false if no plugins were found
             * @throw std::runtime_exception if one of the found plugins refused to load.
             */
            bool loadPluginsInternal( std::string const& path_list, std::string const& subdir, std::string const& kind );
            /**
             * This function does not hold the listlock.
             * @see isLoaded()
             */
            bool isLoadedInternal(std::string name);
            /**
             * Internal helper function that validate if a plugin is compatible with the
             * current target. Currently, there's a validation only on the win32 platform
             * to avoid mixing debug/release runtime libraries.
             * @param filepath Full path of plugin to validate (with extension)
             * @return true if the plugin is compatible
             */
            bool isCompatiblePlugin(std::string const& filepath);

        public:
            PluginLoader();
            ~PluginLoader();

            typedef boost::shared_ptr<PluginLoader> shared_ptr;
            /**
             * Create the instance of the PluginLoader. It will keep track
             * of the loaded libraries for this process.
             * @return A singleton.
             */
            static boost::shared_ptr<PluginLoader> Instance();

            /**
             * Release the PluginLoader, erasing all knowledge of loaded
             * libraries. No libraries will be unloaded from the process.
             */
            static void Release();

            /**
             * Loads a library as plugin or typekit.
             * @param path an absolute or relative path to a library.
             * Relative paths are interpreted with regard to the plugin path.
             */
            bool loadLibrary(std::string const& path);

            /**
             * Load any typekit found in the 'types/' subdirectory of each path in path_list in the process.
             * This is not a 'smart' function. It will only look into that directory and will not try
             * to guess a target specific subdir or any other means for locating typekits.
             * @return false if some typekit caused an error, or some path was not found or if path_list was empty.
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for typekits. No other paths will be searched.
             * @throw std::runtime_exception if one of the found typekits refused to load.
             */
            bool loadTypekits(std::string const& path_list);

            /**
             * Load a typekit found in the 'types/' subdirectory of a package present in path_list.
             * @param name The name of the package (a directory name) which contains the typekit you wish to load.
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for the \a name subdirectory. If this list is empty, the current plugin path is searched and
             * the current working directory.
             * @throw std::runtime_exception if the found typekit refused to load.
             */
            bool loadTypekit(std::string const& name, std::string const& path_list);

            /**
             * Loads any plugin found in the 'plugins/' subdirectory of each path in path_list in the current process.
             * @return false if some typekit caused an error, or some path was not found.
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for plugins. May be the empty string.
             * @throw std::runtime_exception if one of the found plugin refused to load.
             */
            bool loadPlugins(std::string const& path_list);

            /**
             * Checks if a given plugin or filename has been loaded.
             * This function accepts full filenames ('libthe_plugin.so.1.99.0'), short names
             * ('the_plugin') or the name provided by the plugin ('The Plugin').
             * @param name name of a file or the plugin name.
             * @return true if so.
             */
            bool isLoaded(std::string name);

            /**
             * Loads a plugin found in the 'plugins/' subdirectory of each path in path_list in the current process.
             * @param name The name of the plugin to load, must match the library name (without lib/dll/so pre-/suffixes).
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for plugins. May be the empty string.
             * @throw std::runtime_exception if the found plugin refused to load.
             */
            bool loadPlugin(std::string const& name, std::string const& path_list);

            /**
             * Loads an earlier discovered service into a TaskContext.
             * @param servicename The name of the service or plugin containing
             * the service
             * @param tc The TaskContext to load into.
             * @return false if the service or plugin refused to load into the TaskContext.
             */
            bool loadService(std::string const& servicename, TaskContext* tc);

            /**
             * Lists all services discovered by the PluginLoader.
             * @return A list of service names
             */
            std::vector<std::string> listServices() const;

            /**
             * Lists all plugins (= services + typekits) discovered by the PluginLoader.
             * @return A list of plugin names
             */
            std::vector<std::string> listPlugins() const;

            /**
             * Lists all typekits discovered by the PluginLoader.
             * @return A list of typekit names
             */
            std::vector<std::string> listTypekits() const;

            /**
             * Returns the current plugin path list.
             * Defaults to the value of RTT_COMPONENT_PATH, when
             * the RTT was started for the current process.
             * @return A colon separated list of paths or the empty string if not set.
             */
            std::string getPluginPath() const;

            /**
             * Sets the plugin path list. This is typically done by RTT
             * startup code with the contents of the RTT_COMPONENT_PATH variable.
             *
             * @param newpath The new paths to look for plugins.
             */
            void setPluginPath( std::string const& newpath );
        };
    }
}


#endif /* ORO_PLUGINLOADER_HPP_ */

#ifndef ORO_PLUGINLOADER_HPP_
#define ORO_PLUGINLOADER_HPP_

#include <string>
#include "TaskContext.hpp"

namespace RTT {
    namespace plugin {
        /**
         * Loads plugins found on the filesystem and keeps track of found plugins, typekits and services.
         * It has no facilities of unloading plugins. Plugins remain in memory until the process
         * terminates.
         */
        class PluginLoader
        {
            static boost::shared_ptr<PluginLoader> minstance;

            /**
             * Keep a list of all loaded libraries such that double
             * loads are avoided during import/loadLibrary.
             */
            class LoadedLib{
            public:
                LoadedLib(std::string n, std::string short_name, void* h)
                : filename(n), shortname(short_name), handle(h), loadPlugin(0)
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
            };

            std::vector< LoadedLib > loadedLibs;

            /**
             * Handle of last loaded library.
             */
            void* handle;

            /**
             * Name of last loaded library.
             */
            std::string libname;

            /**
             * Internal function that does all library loading.
             * @param filename The path+filename to open
             * @param shortname The short name of this file
             * @param log_error Log errors to users. Set to false in case you are poking
             * files to see if they can be loaded.
             * @return true if a new library was loaded or if this library was already loaded.
             */
            bool loadInProcess(std::string filename, std::string shortname, bool log_error );
            /**
             * Helper function for loadTypekit and loadPlugin.
             * @param name
             * @param path_list
             * @param subdir
             * @param kind
             * @return
             */
            bool loadPluginInternal( std::string const& name, std::string const& path_list, std::string const& subdir, std::string const& kind );
            /**
             * Helper function for loadTypekits and loadPlugins.
             * @param path_list
             * @param subdir
             * @param kind
             * @return
             */
            void loadPluginsInternal( std::string const& path_list, std::string const& subdir, std::string const& kind );
        public:
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
             * Load any typekit found in the 'types/' subdirectory of each path in path_list in the process.
             * This is a best effort function, ie it will silently ignore wrong paths or paths without any
             * typekits.
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for typekits.
             */
            void loadTypekits(std::string const& path_list);

            /**
             * Load a typekit found in the 'types/' subdirectory of each path in path_list in the process.
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for typekits.
             */
            bool loadTypekit(std::string const& name, std::string const& path_list);

            /**
             * Loads any plugin found in the 'plugins/' subdirectory of each path in path_list in the current process.
             * This is a best effort function, ie it will silently ignore wrong paths or paths without any
             * plugins.
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for plugins.
             */
            void loadPlugins(std::string const& path_list);

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
             * to look for plugins.
             */
            bool loadPlugin(std::string const& name, std::string const& path_list);

            /**
             * Loads an earlier discovered service into a TaskContext.
             * @param servicename The name of the service or plugin containing
             * the service
             * @param tc The TaskContext to load into.
             * @return false if the plugin refused to load into the TaskContext.
             */
            bool loadService(std::string const& servicename, TaskContext* tc);
        };
    }
}


#endif /* ORO_PLUGINLOADER_HPP_ */

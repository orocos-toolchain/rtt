#ifndef ORO_PLUGINLOADER_HPP_
#define ORO_PLUGINLOADER_HPP_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "../rtt-fwd.hpp"

namespace RTT {
    namespace plugin {
        /**
         * Loads plugins found on the filesystem and keeps track of found plugins, typekits and services.
         * It has no facilities of unloading plugins. Plugins remain in memory until the process
         * terminates.
         *
         * @name Plugin Paths
         * The PluginLoader reads the RTT_PLUGIN_PATH variable when the application starts
         * and stores it using setPluginPath(). Paths are scanned in this order:
         *
         * ** First the paths specified by the function argument \a path_list if the function takes such argument
         * ** Second the paths specified using the setPluginPath() function.
         *
         * If neither is specified, it looks for plugins in the current directory (".").
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
                : filename(n), shortname(short_name), handle(h), loadPlugin(0), is_typekit(0)
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
                bool is_typekit;
            };

            std::vector< LoadedLib > loadedLibs;

            /**
             * Path to look for if all else fails.
             */
            std::string plugin_path;

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
             * to look for typekits. May be empty the empty string.
             */
            bool loadTypekit(std::string const& name, std::string const& path_list);

            /**
             * Loads any plugin found in the 'plugins/' subdirectory of each path in path_list in the current process.
             * This is a best effort function, ie it will silently ignore wrong paths or paths without any
             * plugins.
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for plugins. May be the empty string.
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
             * to look for plugins. May be the empty string.
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
             * Defaults to the value of RTT_PLUGIN_PATH, when
             * the RTT was started for the current process.
             * @return A colon separated list of paths or the empty string if not set.
             */
            std::string getPluginPath() const;

            /**
             * Sets the plugin path list. This is typically done by RTT
             * startup code with the contents of the RTT_PLUGIN_PATH variable.
             *
             * @param newpath The new paths to look for plugins.
             */
            void setPluginPath( std::string const& newpath );
        };
    }
}


#endif /* ORO_PLUGINLOADER_HPP_ */

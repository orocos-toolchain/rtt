#ifndef ORO_COMPONENTLOADER_HPP_
#define ORO_COMPONENTLOADER_HPP_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <rtt/Component.hpp>

namespace RTT {
        /**
         * Locates Component libraries found on the filesystem and keeps track of loaded Components.
         * In case a no components of a component library are running, the library can be unloaded.
         *
         * @name Component Paths
         * Paths are scanned in this order:
         *
         * ** First the paths specified by the function argument \a path_list if the function takes such argument
         * ** Second the paths specified using the setComponentPath() function.
         *
         * If neither is specified, it looks for Components in the current directory (".").
         */
        class ComponentLoader
        {
            /**
             * Keep a list of all loaded libraries such that double
             * loads are avoided during import/loadLibrary.
             */
            class LoadedLib{
            public:
                LoadedLib(std::string n, std::string short_name, void* h)
                : filename(n), shortname(short_name), handle(h)
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
                 * Library handle.
                 */
                void* handle;

                std::vector<std::string> components_type;
            };

            struct ComponentData {
                ComponentData()
                    : instance(0), type("")
                {}
                /**
                 * The component instance. This is always a valid pointer.
                 */
                RTT::TaskContext* instance;
                std::string type;
            };

            /**
             * This vector holds the dynamically loaded components.
             */
            typedef std::map<std::string, ComponentData> CompList;

            CompList comps;


            std::vector< LoadedLib > loadedLibs;

            std::vector< std::string > loadedPackages;

            /**
             * Path to look for if all else fails.
             */
            std::string component_path;

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
             * Internal function that does try to reload a previously loaded library by first dl_close'ing the library.
             * @param filename The path+filename to open
             * @param shortname The short name of this file
             * @return true if this library was reloaded.
             */
            bool reloadInProcess(std::string filename, std::string shortname);

            /**
             * Internal helper function that validate if a component is compatible with the
             * current target. Currently, there's a validation only on the win32 platform
             * to avoid mixing debug/release runtime libraries.
             * @param filepath Full path of component to validate (with extension).
             * @return true if the component is compatible
             */
            bool isCompatibleComponent(std::string const& filepath);

            /**
             * Returns true if \a package is a subdir of the RTT_COMPONENT_PATH
             */
            bool importInstalledPackage(std::string const& package, std::string const& path_list);
        public:
            typedef boost::shared_ptr<ComponentLoader> shared_ptr;
            /**
             * Create the instance of the ComponentLoader. It will keep track
             * of the loaded libraries for this process.
             * @return A singleton.
             */
            static boost::shared_ptr<ComponentLoader> Instance();

            /**
             * Release the ComponentLoader, erasing all knowledge of loaded
             * libraries. No libraries will be unloaded from the process.
             */
            static void Release();

            /**
             * Imports any Component library found in each path in path_list in the current process.
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for Components. May be the empty string.
             * @return true if all paths were valid and contained components. If at least one path
             * did not exist or did not contain any components or plugins, returns false.
             */
            bool import(std::string const& path_list);

            /**
             * Checks if a given Component type, filename or package name has been imported.
             * This function accepts full filenames ('libthe_Component.so.1.99.0'), short names
             * ('the_Component'), the name provided by the Component Factory ('App::Component'),
             * or a package name ('myrobot')
             *
             * @param type_name name of a file, package directory or the Component type.
             * @return true if so.
             */
            bool isImported(std::string type_name);

            /**
             * Imports a package found in each path in path_list in the current process.
             * @param name The name of the (package) directory to import
             * @param path_list A colon or semi-colon seperated list of paths
             * to look for Components. May be the empty string. In that case, the
             * default component search path is used.
             */
            bool import(std::string const& name, std::string const& path_list);

            /**
             * Loads a library as component library.
             * @param path an absolute or relative path to a library.
             * Relative paths are interpreted with regard to the plugin path.
             */
            bool loadLibrary(std::string const& path);

            /**
             * Reloads a component library.
             * @param filepath an absolute path to a library.
             */
            bool reloadLibrary(std::string const& filepath);

            /**
             * Creates a new component an earlier discovered component type.
             * @param name The name of the to be created Component
             * @param type The type of component to be created.
             * @return null on error or a new component.
             */
            RTT::TaskContext* loadComponent(std::string const& name, std::string const& type);

            /**
             * Destroys an earlier created component.
             * @param tc The TaskContext to be destroyed. tc may no longer
             * be used after this function returns true.
             * @return false if \a tc was not loaded by this ComponentLoader.
             */
            bool unloadComponent( RTT::TaskContext* tc );

            /**
             * Lists all Component created by loadComponent().
             * @return A list of Component names
             */
            std::vector<std::string> listComponents() const;

            /**
             * Lists all Component types discovered by the ComponentLoader.
             * @return A list of Component type names
             */
            std::vector<std::string> listComponentTypes() const;

            /**
             * Returns the current Component path list.
             * Defaults to the value of RTT_COMPONENT_PATH, when
             * the RTT was started for the current process.
             * @return A colon separated list of paths or the empty string if not set.
             */
            std::string getComponentPath() const;

            /**
             * Sets the Component path list. This is typically done by RTT
             * startup code with the contents of the RTT_COMPONENT_PATH variable.
             *
             * @param newpath The new paths to look for Components.
             */
            void setComponentPath( std::string const& newpath );

            /**
             * Returns the factory singleton which creates all types of components
             * for the ComponentLoader.
             */
            const FactoryMap& getFactories() const;

            /**
             * Adds a factory to the component factory map.
             */
            void addFactory( std::string const& name, ComponentLoaderSignature factory );
        };
}


#endif /* ORO_COMPONENTLOADER_HPP_ */

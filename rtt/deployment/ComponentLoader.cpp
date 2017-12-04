#include "ComponentLoader.hpp"
#include <rtt/TaskContext.hpp>
#include <rtt/Logger.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <rtt/os/StartStopManager.hpp>
#include <rtt/plugin/PluginLoader.hpp>
#include <rtt/types/TypekitRepository.hpp>

#ifndef _WIN32
# include <dlfcn.h>
#endif

#include <vector>
#include <set>

using namespace RTT;
using namespace RTT::detail;
using namespace std;
using namespace boost::filesystem;

namespace RTT
{
    // We have our own copy of the Factories object to store all
    // loaded component types. This is pointer is only shared with the DeploymentComponent.
    FactoryMap* ComponentFactories::Factories = 0;
}

// chose the file extension and debug postfix applicable to the O/S
#ifdef  __APPLE__
static const std::string SO_EXT(".dylib");
static const std::string SO_POSTFIX("");
#else
# ifdef _WIN32
static const std::string SO_EXT(".dll");
#  ifdef _DEBUG
static const std::string SO_POSTFIX("d");
#  else
static const std::string SO_POSTFIX("");
#  endif // _DEBUG
# else
static const std::string SO_EXT(".so");
static const std::string SO_POSTFIX("");
# endif
#endif

// The full library suffix must be enforced by the UseOrocos macros
static const std::string FULL_COMPONENT_SUFFIX(string("-") + string(OROCOS_TARGET_NAME) + SO_POSTFIX + SO_EXT);

// choose how the PATH looks like
# ifdef _WIN32
static const std::string delimiters(";");
static const char default_delimiter(';');
# else
static const std::string delimiters(":;");
static const char default_delimiter(':');
# endif

/** Determine whether a file extension is actually part of a library version

    @return true if \a ext satisfies "^\.[:digit:]+$"

    So for example
    returns true  for ".1", ".12", ".123"
    returns false for ".a", "1", "123", ".123 ", "a", "", ".1.a", ".1a2"
*/
static RTT_UNUSED bool isExtensionVersion(const std::string& ext)
{
    bool isExtensionVersion = false;

	if (!ext.empty() && ('.' == ext[0]))
	{
        std::istringstream	iss;
        int					i;

		iss.str(ext.substr((size_t)1));	    // take all after the '.'
        iss >> std::dec >> std::noskipws >> i;
        isExtensionVersion = !iss.fail() && iss.eof();
    }

    return isExtensionVersion;
}

/* Is this a dynamic library that we should load from within a directory scan?

   Versioned libraries are not loaded, to prevent loading both libfoo.so and
   libfoo.so.1 (which is typically a symlink to libfoo.so, and so loading
   the same library twice).

   Libraries are either (NB x.y.z is version, and could also be x or x.y)

   Linux
   libfoo.so          = load this
   libfoo.so.x.y.z    = don't load this

   Windows
   libfoo.dll         = load this

   Mac OS X
   libfoo.dylib       = load this
   libfoo.x.y.z.dylib = don't load this

   All the above also apply without the "lib" prefix.
*/
static bool isLoadableLibrary(const path& filename)
{
    bool isLoadable = false;

#if     defined(__APPLE__)
	std::string	ext;
#if BOOST_VERSION >= 104600
	ext = filename.extension().string();
#else
	ext = filename.extension();
#endif
    // ends in SO_EXT?
	if (0 == ext.compare(SO_EXT))
	{
		// Ends in SO_EXT and so must not be a link for us to load
		// Links are of the form abc.x.dylib or abc.x.y.dylib or abc.x.y.z.dylib,
		// where x,y,z are positive numbers
		path	name	= filename.stem();	    // drop SO_EXT
		path	ext 	= name.extension();
		isLoadable =
			// wasn't just SO_EXT
			!name.empty() &&
			// and if there is and extension then it is not a number
			(ext.empty() || !isExtensionVersion(ext.string()));
    }
    // else is not loadable

#else
    // Linux or Windows

    // must end in SO_EXT and have a non-extension part
    isLoadable =
        (filename.extension() == SO_EXT) &&
        !filename.stem().empty();
#endif

    return isLoadable;
}

namespace {

// copied from RTT::PluginLoader
static vector<string> splitPaths(string const& str)
{
    vector<string> paths;

    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        if ( !str.substr(lastPos, pos - lastPos).empty() )
            paths.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    if ( paths.empty() )
        paths.push_back(".");
    return paths;
}

static void removeDuplicates(string& path_list)
{
    vector<string> paths;
    set<string> seen;
    string result;

    // split path_lists
    paths = splitPaths( path_list );

    // iterate over paths and append to result
    for(vector<string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
    {
        if (seen.count(*it))
            continue;
        else
            seen.insert(*it);

        result = result + *it + default_delimiter;
    }

    // remove trailing delimiter
    if (result.size() > 0 && result.at(result.size() - 1) == default_delimiter)
        result = result.substr(0, result.size() - 1);

    path_list.swap(result);
}

/**
 * Strips the 'lib' prefix and '.so'/'.dll'/... suffix (ie SO_EXT) from a filename.
 * Do not provide paths, only filenames, for example: "libcomponent.so"
 * @param str filename.
 * @return stripped filename.
 */
static string makeShortFilename(string const& str) {
    string ret = str;
    if (str.substr(0,3) == "lib")
        ret = str.substr(3);
    if (ret.rfind(FULL_COMPONENT_SUFFIX) != string::npos)
        ret = ret.substr(0, ret.rfind(FULL_COMPONENT_SUFFIX) );
    return ret;
}

}

static RTT_UNUSED bool hasEnding(string const &fullString, string const &ending)
{
    if (fullString.length() > ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

namespace RTT {
    extern char const* default_comp_path_build;
}

namespace {
    /**
     * Reads the RTT_COMPONENT_PATH and inits the ComponentLoader.
     */
    int loadComponents()
    {
        std::string default_comp_path = ::default_comp_path_build;

        char* paths = getenv("RTT_COMPONENT_PATH");
        string component_paths;
        if (paths) {
            component_paths = paths;
            // prepend the default search path.
            if ( !default_comp_path.empty() )
                component_paths = component_paths + default_delimiter + default_comp_path;
            removeDuplicates( component_paths );
            log(Info) <<"RTT_COMPONENT_PATH was set to: " << paths << " . Searching in: "<< component_paths<< endlog();
        } else {
            component_paths = default_comp_path;
            removeDuplicates( component_paths );
            log(Info) <<"No RTT_COMPONENT_PATH set. Using default: " << component_paths <<endlog();
        }
        // we set the component path such that we can search for sub-directories/projects lateron
        ComponentLoader::Instance()->setComponentPath(component_paths);
        return 0;
    }

    os::InitFunction component_loader( &loadComponents );

    void unloadComponents()
    {
        ComponentLoader::Release();
    }

    os::CleanupFunction component_unloader( &unloadComponents );
}

static boost::shared_ptr<ComponentLoader> instance2;

boost::shared_ptr<ComponentLoader> ComponentLoader::Instance() {
    if (!instance2)
        instance2.reset( new ComponentLoader() );
    return instance2;
}

void ComponentLoader::Release() {
    instance2.reset();
}

// This is the dumb import function that takes an existing directory and
// imports components and plugins from it.
bool ComponentLoader::import( std::string const& path_list )
{
    RTT::Logger::In in("ComponentLoader::import(path_list)");

    if (path_list.empty() ) {
        log(Error) << "import paths: No paths were given for loading ( path_list = '' )."<<endlog();
        return false;
    }

    // we return false if nothing was found here, or an error happened during loading of a library.
    vector<string> paths;
    paths = splitPaths( path_list ); // import package or path list.

    bool all_good = true, found = false;
    // perform search in paths:
    for (vector<string>::iterator it = paths.begin(); it != paths.end(); ++it)
    {
        // Scan path/* (non recursive) for components
        path p = path(*it);
        if (is_directory(p))
        {
            log(Info) << "Importing directory " << p.string() << " ..."<<endlog();
            for (directory_iterator itr(p); itr != directory_iterator(); ++itr)
            {
                log(Debug) << "Scanning file " << itr->path().string() << " ...";
                if (is_regular_file(itr->status()) && isLoadableLibrary(itr->path()) ) {
                    found = true;
                    std::string libname;
#if BOOST_VERSION >= 104600
                    libname = itr->path().filename().string();
#else
                    libname = itr->path().filename();
#endif
                    if(!isCompatibleComponent(libname))
                    {
                        log(Debug) << "not a compatible component: ignored."<<endlog();
                    }
                    else
                    {
                        found = true;
                        all_good = loadInProcess( itr->path().string(), makeShortFilename(libname ),  true) && all_good;
                    }
                } else {
                    if (!is_regular_file(itr->status()))
                        log(Debug) << "not a regular file: ignored."<<endlog();
                    else
                        log(Debug) << "not a " + SO_EXT + " library: ignored."<<endlog();
                }
            }
            log(Debug) << "Looking for plugins or typekits in directory " << p.string() << " ..."<<endlog();
            try {
                found = PluginLoader::Instance()->loadTypekits( p.string() ) || found;
                found = PluginLoader::Instance()->loadPlugins( p.string() ) || found;
            } catch (std::exception& e) {
                all_good = false;
                log(Error) << e.what() <<endlog();
            }
        }
        else {
            // If the path is not complete (not absolute), look it up in the search directories:
            log(Debug) << "No such directory: " << p<< endlog();
        }
#if 0
        // Repeat for path/OROCOS_TARGET: (already done in other import function)
        p = path(*it) / OROCOS_TARGET_NAME;
        if (is_directory(p))
        {
            log(Info) << "Importing component libraries from directory " << p.string() << " ..."<<endlog();
            for (directory_iterator itr(p); itr != directory_iterator(); ++itr)
            {
                log(Debug) << "Scanning file " << itr->path().string() << " ...";
                if (is_regular_file(itr->status()) && isLoadableLibrary(itr->path()) ) {
                    found = true;
#if BOOST_VERSION >= 104600
                    all_good = loadInProcess( itr->path().string(), makeShortFilename(itr->path().filename().string() ),  true) && all_good;
#else
                    all_good = loadInProcess( itr->path().string(), makeShortFilename(itr->path().filename() ),  true) && all_good;
#endif
                }else {
                    if (!is_regular_file(itr->status()))
                        log(Debug) << "not a regular file: ignored."<<endlog();
                    else
                        log(Debug) << "not a " + SO_EXT + " library: ignored."<<endlog();
                }
            }
            log(Info) << "Importing plugins and typekits from directory " << p.string() << " ..."<<endlog();
            try {
                found = PluginLoader::Instance()->loadTypekits( p.string() ) || found;
                found = PluginLoader::Instance()->loadPlugins( p.string() ) || found;
            } catch (std::exception& e) {
                all_good = false;
                log(Error) << e.what() <<endlog();
            }
        }
#endif
    }
    if (!all_good)
        throw std::runtime_error("Some found plugins could not be loaded !");
    return found;
}

// this is the smart import function that tries to guess where 'package' lives in path_list or
// the search path.
bool ComponentLoader::import( std::string const& package, std::string const& path_list )
{
    RTT::Logger::In in("ComponentLoader::import(package, path_list)");

    // check first for exact match to *file*:
    path arg( package );
    if (is_regular_file(arg) && isLoadableLibrary(arg)) {
#if BOOST_VERSION >= 104600
	    return loadInProcess(arg.string(), makeShortFilename( arg.filename().string() ), true);
#else
	    return loadInProcess(arg.string(), makeShortFilename( arg.filename() ), true);
#endif
    }

    // check for absolute path:
    if ( arg.is_complete() ) {
        // plain import
        bool ret = import(package);
        // if not yet given, test for target subdir:
        if ( arg.parent_path().leaf() != OROCOS_TARGET_NAME )
            ret = import( (arg / OROCOS_TARGET_NAME).string() ) || ret;
        // if something found, return true:
        if (ret)
            return true;
        // both failed:
        log(Error) << "Could not import absolute path '"<<package << "': nothing found."<<endlog();
        return false;
    }

    if ( isImported(package) ) {
        log(Info) <<"Component package '"<< package <<"' already imported." <<endlog();
        return true;
    }

    // Try the RTT_COMPONENT_PATH:
    return importInstalledPackage(package, path_list);
}

bool ComponentLoader::importInstalledPackage(std::string const& package, std::string const& path_list)
{
    RTT::Logger::In in("ComponentLoader::importInstalledPackage(package, path_list)");

    string paths;
    string trypaths;
    vector<string> tryouts;
    if (path_list.empty())
        paths = component_path + default_delimiter + ".";
    else
        paths = path_list;

    bool path_found = false;

    // if ros did not find anything, split the paths above.
    // set vpaths from (default) search paths.
    vector<string> vpaths;
    vpaths = splitPaths(paths);
    trypaths = paths; // store for error reporting below.
    paths.clear();
    // Detect absolute/relative import:
    path p( package );
    if (is_directory( p )) {
        path_found = true;
        // search in dir + dir/TARGET
        paths += p.string() + default_delimiter + (p / OROCOS_TARGET_NAME).string() + default_delimiter;
        if ( p.is_complete() ) {
            // 2.2.x: path may be absolute or relative to search path.
            //log(Warning) << "You supplied an absolute directory to the import directive. Use 'path' to set absolute directories and 'import' only for packages (sub directories)."<<endlog();
            //log(Warning) << "Please modify your XML file or script. I'm importing it now for the sake of backwards compatibility."<<endlog();
        } // else: we allow to import a subdirectory of '.'.
    }
    // append '/package' or 'target/package' to each plugin path in order to search all of them:
    for(vector<string>::iterator it = vpaths.begin(); it != vpaths.end(); ++it) {
        p = *it;
        p = p / package;
        // we only search in existing directories:
        if (is_directory( p )) {
            path_found = true;
            paths += p.string() + default_delimiter ;
        } else
            tryouts.push_back( p.string() );
        p = *it;
        p = p / OROCOS_TARGET_NAME / package;
        // we only search in existing directories:
        if (is_directory( p )) {
            path_found = true;
            paths += p.string() + default_delimiter ;
        } else
            tryouts.push_back( p.string() );
    }
    if ( path_found )
        paths.erase( paths.size() - 1 ); // remove trailing delimiter ';'

    // when at least one directory exists:
    if (path_found) {
        if ( import(paths) ) {
            loadedPackages.push_back( package );
            return true;
        } else {
            log(Error) << "Failed to import components, types or plugins from package or directory '"<< package <<"' found in:"<< endlog();
            log(Error) << paths << endlog();
            return false;
        }
    }
    log(Error) << "No such package or directory found in search path: " << package << ". Search path is: " << trypaths << endlog();
    log(Error) << "Directories searched include the following: " << endlog();
    for(vector<string>::iterator it=tryouts.begin(); it != tryouts.end(); ++it)
        log(Error) << " - " << *it << endlog();
    return false;
}

bool ComponentLoader::reloadLibrary(std::string const& name)
{
    path arg = name;
    // check for direct match:
#if BOOST_VERSION >= 104600
    if (is_regular_file( arg ) && reloadInProcess( arg.string(), makeShortFilename( arg.filename().string() ) ) )
#else
    if (is_regular_file( arg ) && reloadInProcess( arg.string(), makeShortFilename( arg.filename() ) ) )
#endif
        return true;
    // bail out if not an absolute path
    return false;
}

bool ComponentLoader::loadLibrary( std::string const& name )
{
    path arg = name;
    // check for direct match:
#if BOOST_VERSION >= 104600
    if (is_regular_file( arg ) && loadInProcess( arg.string(), makeShortFilename( arg.filename().string() ), true ) )
#else
    if (is_regular_file( arg ) && loadInProcess( arg.string(), makeShortFilename( arg.filename() ), true ) )
#endif
        return true;
    // bail out if absolute path
    if ( arg.is_complete() )
        return false;

    // search for relative match
    vector<string> paths = splitPaths( component_path );
    vector<string> tryouts( paths.size() * 4 );
    tryouts.clear();
    path dir = arg.parent_path();
#if BOOST_VERSION >= 104600
    string file = arg.filename().string();
#else
    string file = arg.filename();
#endif

    for (vector<string>::iterator it = paths.begin(); it != paths.end(); ++it)
    {
        path p = path(*it) / dir / (file + FULL_COMPONENT_SUFFIX);
        tryouts.push_back( p.string() );
        if (is_regular_file( p ) && loadInProcess( p.string(), makeShortFilename(file), true ) )
            return true;
        p = path(*it) / dir / ("lib" + file + FULL_COMPONENT_SUFFIX);
        tryouts.push_back( p.string() );
        if (is_regular_file( p ) && loadInProcess( p.string(), makeShortFilename(file), true ) )
            return true;
        p = path(*it) / OROCOS_TARGET_NAME / dir / (file + FULL_COMPONENT_SUFFIX);
        tryouts.push_back( p.string() );
        if (is_regular_file( p ) && loadInProcess( p.string(), makeShortFilename(file), true ) )
            return true;
        p = path(*it) / OROCOS_TARGET_NAME / dir / ("lib" + file + FULL_COMPONENT_SUFFIX);
        tryouts.push_back( p.string() );
        if (is_regular_file( p ) && loadInProcess( p.string(), makeShortFilename(file), true ) )
            return true;
    }
    log(Debug) << "No such library found in path: " << name << ". Tried:"<< endlog();
    for(vector<string>::iterator it=tryouts.begin(); it != tryouts.end(); ++it)
        log(Debug) << *it << endlog();
    return false;
}

bool ComponentLoader::isImported(string type_name)
{
    if (ComponentFactories::Instance().find(type_name) != ComponentFactories::Instance().end() )
        return true;
    if (find(loadedPackages.begin(), loadedPackages.end(), type_name) != loadedPackages.end())
        return true;
    // hack: in current versions, ocl is loaded most of the times by default because it does not reside in a package subdir
    // once ocl is in the 'ocl' package directory, this code becomes obsolete:
    if ( type_name == "ocl" && TypekitRepository::hasTypekit("OCLTypekit")) {
        return true;
    }
    return false;
}

bool ComponentLoader::reloadInProcess(string file, string libname)
{
    path p(file);

    // check if the library is already loaded
    // NOTE if this library has been loaded, you can unload and reload it to apply changes (may be you have updated the dynamic library)
    // anyway it is safe to do this only if there isn't any instance whose type was loaded from this library

    std::vector<LoadedLib>::iterator lib = loadedLibs.begin();
    while (lib != loadedLibs.end()) {
        // We only reload if it's exactly the same file.
        if ( lib->filename == file) {
            log(Info) <<"Component library "<< lib->filename <<" already loaded... " ;

            bool can_unload = true;
            CompList::iterator cit;
            for( std::vector<std::string>::iterator ctype = lib->components_type.begin();  ctype != lib->components_type.end() && can_unload; ++ctype) {
                for ( cit = comps.begin(); cit != comps.end(); ++cit) {
                    if( (*ctype) == cit->second.type ) {
                        // the type of an allocated component was loaded from this library. it might be unsafe to reload the library
                        log(Info) << "can NOT reload library because of the instance " << cit->second.type  <<"::"<<cit->first <<endlog();
                        can_unload = false;
                    }
                }
            }
            if( can_unload ) {
                log(Info) << "try to RELOAD"<<endlog();
                dlclose(lib->handle);
                // remove the library info from the vector
                std::vector<LoadedLib>::iterator lib_un = lib;
                loadedLibs.erase(lib_un);
                return loadInProcess(file, libname, true);
            }
            else
                return false;
        }
        else lib++;
    }
    log(Error) << "Can't reload Component library "<< file << " since it was not loaded or is not a component library." <<endlog();
    return false;
}

// loads a single component in the current process.
bool ComponentLoader::loadInProcess(string file, string libname, bool log_error) {
    path p(file);
    char* error;
    void* handle;
    bool success=false;

    // Last chance to validate component compatibility
    if(!isCompatibleComponent(file))
    {
        if(log_error)
            log(Error) << "Could not load library '"<< p.string() <<"': incompatible." <<endlog();
        return false;
    }

    handle = dlopen ( p.string().c_str(), RTLD_NOW);

    if (!handle) {
        if ( log_error ) {
            log(Error) << "Could not load library '"<< p.string() <<"':"<<endlog();
            log(Error) << dlerror() << endlog();
        }
        return false;
    }

    //------------- if you get here, the library has been loaded -------------
    log(Debug)<<"Succesfully loaded "<<libname<<endlog();
    LoadedLib loading_lib(file, libname, handle);
    dlerror();    /* Clear any existing error */

    // Lookup Component factories (multi component case):
    FactoryMap* (*getfactory)(void) = 0;
    vector<string> (*getcomponenttypes)(void) = 0;
    FactoryMap* fmap = 0;
    getfactory = (FactoryMap*(*)(void))( dlsym(handle, "getComponentFactoryMap") );
    if ((error = dlerror()) == NULL) {
        // symbol found, register factories...
        fmap = (*getfactory)();
        ComponentFactories::Instance().insert( fmap->begin(), fmap->end() );
        log(Info) << "Loaded multi component library '"<< file <<"'"<<endlog();
        getcomponenttypes = (vector<string>(*)(void))(dlsym(handle, "getComponentTypeNames"));
        if ((error = dlerror()) == NULL) {
            log(Debug) << "Components:";
            vector<string> ctypes = getcomponenttypes();
            for (vector<string>::iterator it = ctypes.begin(); it != ctypes.end(); ++it)
                log(Debug) <<" "<< *it;
            log(Debug) << endlog();
        }
        loadedLibs.push_back(loading_lib);
        success = true;
    }

    // Lookup createComponent (single component case):
    dlerror();    /* Clear any existing error */

    RTT::TaskContext* (*factory)(std::string) = 0;
    std::string(*tname)(void) = 0;
    factory = (RTT::TaskContext*(*)(std::string))(dlsym(handle, "createComponent") );
    string create_error;
    error = dlerror();
    if (error) create_error = error;
    tname = (std::string(*)(void))(dlsym(handle, "getComponentType") );
    string gettype_error;
    error = dlerror();
    if (error) gettype_error = error;
    if ( factory && tname ) {
        std::string cname = (*tname)();
        if ( ComponentFactories::Instance().count(cname) == 1 ) {
            log(Warning) << "Component type name "<<cname<<" already used: overriding."<<endlog();
        }
        ComponentFactories::Instance()[cname] = factory;
        log(Info) << "Loaded component type '"<< cname <<"'"<<endlog();
        loading_lib.components_type.push_back( cname );
        loadedLibs.push_back(loading_lib);
        success = true;
    }

    if (success) return true;

    log(Error) <<"Unloading "<< loading_lib.filename  <<": not a valid component library:" <<endlog();
    if (!create_error.empty())
        log(Error) << "   " << create_error << endlog();
    if (!gettype_error.empty())
        log(Error) << "   " << gettype_error << endlog();
    dlclose(handle);
    return false;
}

std::vector<std::string> ComponentLoader::listComponentTypes() const {
    vector<string> names;
    FactoryMap::iterator it;
    for( it = ComponentFactories::Instance().begin(); it != ComponentFactories::Instance().end(); ++it) {
        names.push_back( it->first );
    }
    return names;
}

std::string ComponentLoader::getComponentPath() const {
    string ret = component_path;
    // append default delimiter if not present. such that it can be combined with a new path.
    if ( ret.length() && ret[ ret.length() -1 ] != default_delimiter )
	ret += default_delimiter;
    return ret;
}

void ComponentLoader::setComponentPath( std::string const& newpath ) {
    component_path = newpath;
}


RTT::TaskContext *ComponentLoader::loadComponent(const std::string & name, const std::string & type)
{
    TaskContext* instance = 0;
    RTT::TaskContext* (*factory)(std::string name) = 0;
    log(Debug) << "Trying to create component "<< name <<" of type "<< type << endlog();

    // First: try loading from imported libraries. (see: import).
    if ( ComponentFactories::Instance().count(type) == 1 ) {
        factory = ComponentFactories::Instance()[ type ];
        if (factory == 0 ) {
            log(Error) <<"Found empty factory for Component type "<<type<<endlog();
            return 0;
        }
    }

    if ( factory ) {
        log(Debug) <<"Found factory for Component type "<<type<<endlog();
    } else {
        log(Error) << "Unable to create Orocos Component '"<<type<<"': unknown component type." <<endlog();
        return 0;
    }

    comps[name].type = type;

    try {
        comps[name].instance = instance = (*factory)(name);
    } catch(...) {
        log(Error) <<"The constructor of component type "<<type<<" threw an exception!"<<endlog();
    }

    if ( instance == 0 ) {
        log(Error) <<"Failed to load component with name "<<name<<": refused to be created."<<endlog();
    }
    return instance;
}

bool ComponentLoader::unloadComponent( RTT::TaskContext* tc ) {
    if (!tc)
        return false;
    CompList::iterator it = comps.begin();
    for(; it != comps.end(); ++it ) {
        if ( it->second.instance == tc) break;
    }

    if ( it != comps.end() ) {
        delete it->second.instance;
        comps.erase(it);
        return true;
    }
    log(Error) <<"Refusing to unload a component I didn't load myself."<<endlog();
    return false;
}

std::vector<std::string> ComponentLoader::listComponents() const
{
    vector<string> names( comps.size() );
    for(map<string,ComponentData>::const_iterator it = comps.begin(); it != comps.end(); ++it)
        names.push_back( it->first );
    return names;
}

bool ComponentLoader::isCompatibleComponent(std::string const& filepath)
{
    path p(filepath);

#if BOOST_VERSION >= 104600
    string libname = p.filename().string();
#else
    string libname = p.filename();
#endif

    //log(Debug) << "Validating compatility of component file '" + libname + "'" <<endlog();

#ifdef _WIN32
    // On WIN32 target:
    // - look if the library name ends with "win32.dll" on release mode
    // - look if the library name ends with "win32d.dll" on debug mode
    if(!hasEnding(libname, FULL_COMPONENT_SUFFIX))
    {
        //log(Debug) << "Component file '" + libname + "' is incompatible because it doesn't have the suffix " << FULL_COMPONENT_SUFFIX << endlog();
        return false;
    }
#endif // _WIN32

    // There's no validation on other targets

    //log(Debug) << "Component file '" + libname + "' is valid." <<endlog();

    return true;
}

const FactoryMap& ComponentLoader::getFactories() const
{
  return ComponentFactories::Instance();
}

void ComponentLoader::addFactory(std::string const& name, ComponentLoaderSignature factory)
{
  ComponentFactories::Instance()[name] = factory;
}

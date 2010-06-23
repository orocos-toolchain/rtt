/**
 * Plugin.cpp
 *
 *  Created on: Jun 21, 2010
 *      Author: kaltan
 */

#include "../../os/StartStopManager.hpp"
#include <cstdlib>
#include "PluginLoader.hpp"
#include "../Logger.hpp"

using namespace std;

namespace RTT {
    namespace plugin {
        /**
         * This struct has the sole purpose of invoking
         * the Import function.
         */
        int loadPlugins()
        {
            char* paths = getenv("RTT_PLUGIN_PATH");
            if (paths) {
                string plugin_paths = paths;
                log(Info) <<"RTT_PLUGIN_PATH was set to " << plugin_paths << endlog();
                PluginLoader::Instance()->setPluginPath(plugin_paths);
            } else {
                log(Info) <<"No RTT_PLUGIN_PATH set." <<endlog();
            }
            PluginLoader::Instance()->loadPlugins("");
            PluginLoader::Instance()->loadTypekits("");
            return 0;
        }

        os::InitFunction plugin_loader( &loadPlugins );
    }


}

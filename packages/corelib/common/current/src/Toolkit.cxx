#include <corelib/Toolkit.hpp>
#include <corelib/Logger.hpp>
#include <algorithm>

namespace ORO_CoreLib
{
    using namespace std;

    std::vector<ToolkitPlugin*> Toolkit::Tools;

    void Toolkit::Import( ToolkitPlugin& tkpr )
    {
        ToolkitPlugin* tkp = &tkpr;
        Logger::In in("Toolkit");
        if ( find( Tools.begin(), Tools.end(), tkp ) != Tools.end() ) {
            Logger::log() <<Logger::Debug << "Tool "<<tkp->getName() <<" already loaded."<<Logger::endl;
            return;
        }

        Logger::log() <<Logger::Info << "Loading Tool "<<tkp->getName() <<"."<<Logger::endl;
        Tools.push_back( tkp );

        if ( tkp->loadTypes() == false ) {
            Logger::log() <<Logger::Error << "Tool "<<tkp->getName() <<" failed to load types."<<Logger::endl;
        }

        if ( tkp->loadOperators() == false ) {
            Logger::log() <<Logger::Error << "Tool "<<tkp->getName() <<" failed to load type operators."<<Logger::endl;
        }
    }

    std::vector<std::string> Toolkit::getTools()
    {
        std::vector<std::string> ret;
        for (std::vector<ToolkitPlugin*>::const_iterator it = Tools.begin();
             it != Tools.end(); ++it)
            ret.push_back( (*it)->getName() );
        return ret;
    }

    bool Toolkit::hasTool( const std::string& toolname )
    {
        for (std::vector<ToolkitPlugin*>::const_iterator it = Tools.begin();
             it != Tools.end(); ++it)
            if ((*it)->getName() == toolname)
                return true;
        return false;
    }
        

}

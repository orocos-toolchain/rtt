
#include "os/StartStopManager.hpp"

namespace ORO_OS
{

    StartStopManager* StartStopManager::mt;

    StartStopManager* StartStopManager::Instance()
    {
        if ( mt == 0 )
            mt = new StartStopManager();
        return mt;
    }

    void StartStopManager::Release()
    {
        delete mt;
        mt = 0;
    }

}

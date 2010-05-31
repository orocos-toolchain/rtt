/**
 * GlobalEngine.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: kaltan
 */

#include "GlobalEngine.hpp"
#include "../Activity.hpp"

namespace RTT
{

    namespace internal
    {
        GlobalEngine* GlobalEngine::mengine = 0;

        GlobalEngine::GlobalEngine()
            : mact( new Activity( this, "GlobalEngine" ) )
        {
            mact->start();
        }

        GlobalEngine::~GlobalEngine()
        {
            mact->stop();
            mact.reset();
        }

        ExecutionEngine* GlobalEngine::Instance() {
            if (mengine == 0) {
                mengine = new GlobalEngine();
            }
            return mengine;
        }
        void GlobalEngine::Release() {
            delete mengine;
            mengine = 0;
        }
    }

}

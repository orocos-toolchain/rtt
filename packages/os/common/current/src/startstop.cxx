/**
 * @file startstop.cxx
 *
 * This file defines the __os_init and __os_exit functions
 * which call manually crafted initialisation functions.
 * The target calls these functions at startup time.
 */

#include <pkgconf/system.h>
#include <pkgconf/os.h>

#include <os/startstop.h>
 
#ifdef HAVE_MANUAL_MAIN
#include "os/MainThread.hpp"

static ORO_OS::MainThread* mainT;
#endif

#include "os/StartStopManager.hpp"
static ORO_OS::StartStopManager* initM;

extern "C"
int __os_init(int argc,const char** argv )
{
#ifdef HAVE_MANUAL_CRT
    DO_GLOBAL_CTORS();
#endif

#ifdef HAVE_MANUAL_MAIN
    // this must be called to init the os scheduler, which is used in
    // any ComponentActive. Call this only once EVER.
    mainT = ORO_OS::MainThread::Instance();

    mainT->start();
#endif

    initM = ORO_OS::StartStopManager::Instance();
    return initM->start();
}

extern "C"
void __os_exit(void)
{
    initM->stop();
    ORO_OS::StartStopManager::Release();

#ifdef HAVE_MANUAL_MAIN
    ORO_OS::MainThread::Release();
#endif

#ifdef HAVE_MANUAL_CRT
    DO_GLOBAL_DTORS();
#endif
}

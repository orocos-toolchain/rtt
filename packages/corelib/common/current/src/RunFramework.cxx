
#include <os/fosi.h>
#include <os/cpp.hpp>

#include "corelib/Framework.hpp"
#include "corelib/HeartBeatGenerator.hpp"
#include "corelib/ZeroTimeThread.hpp"
#include "corelib/ZeroLatencyThread.hpp"
#include "corelib/CompletionProcessor.hpp"
#include <rtstl/rtstreams.hpp>
#include <os/MainThread.hpp>

/**
 *
 * START AND STOP THE HEARTBEATGENERATOR AND COMPLETIONPROCESSOR
 *
 * 
 */

// This is to be kept local --> static

using namespace ORO_CoreLib;

static CompletionProcessor* comp_proc;
static ZeroTimeThread* ztt;
static ZeroLatencyThread* zlt;


#if !HAVE_MAIN
static MainThread* mainT;

void FuncStart()
{
    comp_proc->start();
    ztt->start();
    zlt->start();
}

void FuncStop() 
{
    ztt->stop();
    zlt->stop();
    comp_proc->stop();
}

#endif

extern "C"
void startFramework( void )
{
#if !HAVE_MAIN
    mainT = MainThread::Instance();
#else
    // do nothing
#endif
    ztt = ZeroTimeThread::Instance();
    ztt->start();
    zlt = ZeroLatencyThread::Instance();
    zlt->start();
    comp_proc = CompletionProcessor::Instance();
    comp_proc->start();

}

extern "C"
void stopFramework( void )
{
#if !HAVE_MAIN
    // intentional leak, we can not use static allocated FOO.
    mainT->execute( FuncStop );
    // the complete does not work yet.
    mainT->complete();
#else
    ztt->stop();
    zlt->stop();
    comp_proc->stop();
#endif

}


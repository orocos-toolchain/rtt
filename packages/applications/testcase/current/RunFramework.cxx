
#include <os/fosi.h>

#include "corelib/Framework.hpp"

#include "corelib/ZeroTimeThread.hpp"
#include "corelib/ZeroLatencyThread.hpp"
#include "corelib/NonRealTimeThread.hpp"
#include "corelib/CompletionProcessor.hpp"

#include <iostream>

using namespace ORO_CoreLib;

static CompletionProcessor* comp_proc;
static ZeroTimeThread* ztt;
static ZeroLatencyThread* zlt;
static NonRealTimeThread* nrt;



extern "C"
void startFramework( void )
{
    ztt = ZeroTimeThread::Instance();
    ztt->start();
    zlt = ZeroLatencyThread::Instance();
    zlt->start();
    nrt = NonRealTimeThread::Instance();
    nrt->start();
    comp_proc = CompletionProcessor::Instance();
    comp_proc->start();
}

extern "C"
void stopFramework( void )
{
    ztt->stop();
    zlt->stop();
    nrt->stop();
    comp_proc->stop();
}


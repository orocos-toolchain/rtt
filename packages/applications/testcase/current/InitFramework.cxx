/***************************************************************************
tag: Peter Soetens  Mon Jun 10 14:35:52 CEST 2002  InitFramework.cpp

                      InitFramework.cpp -  description
                         -------------------
  begin                : Mon June 10 2002
  copyright            : (C) 2002 Peter Soetens
  email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#include <os/fosi.h>

#include "corelib/Framework.hpp"
#include "corelib/HeartBeatGenerator.hpp"
#include "corelib/CompletionProcessor.hpp"
#include "corelib/ZeroTimeThread.hpp"
#include "corelib/ZeroLatencyThread.hpp"
#include "corelib/NonRealTimeThread.hpp"
#include "corelib/EventPeriodic.hpp"

#include "corelib/EventEmergencyStop.hpp"

#include <iostream>

#include <pkgconf/system.h>
#include <pkgconf/os.h>

/**
 * @file InitFramework.cxx
 * This initializes the threads.
 */

using namespace ORO_CoreLib;

static HeartBeatGenerator* heartbeat;
static CompletionProcessor* comp_proc;
static ZeroTimeThread* ztt;
static ZeroLatencyThread* zlt;
static NonRealTimeThread* nrt;

static EventPeriodic* events[10]; 
extern "C"
void initFramework( void )
{
    heartbeat = HeartBeatGenerator::Instance();
    comp_proc = CompletionProcessor::Instance();
    ztt = ZeroTimeThread::Instance();
    zlt = ZeroLatencyThread::Instance();
    nrt = NonRealTimeThread::Instance();

    // ZTT
    events[0] = new EventPeriodic( 0.001 ) ; // 1000Hz
    events[1] = new EventPeriodic( 0.01 ); // 100Hz
    // ZLT
    events[2] = new EventPeriodic( 0.05 ) ; // 200Hz
    events[3] = new EventPeriodic( 0.01 ); // 100Hz
    events[4] = new EventPeriodic( 0.1 );// 10Hz
    // NRT
    events[5] = new EventPeriodic( 0.01 ) ; // 100Hz
    events[6] = new EventPeriodic( 0.1 ); // 10Hz
    events[7] = new EventPeriodic( 1.0 );// 1Hz
    
    ztt->eventAdd( events[0]  ); // 1kHz
    ztt->eventAdd( events[1]  );// 100Hz
    zlt->eventAdd( events[2] );// 200Hz
    zlt->eventAdd( events[3]  );// 100Hz
    zlt->eventAdd( events[4] );// 10Hz
    nrt->eventAdd( events[5] );// 100Hz
    nrt->eventAdd( events[6]  );// 10Hz
    nrt->eventAdd( events[7] );// 1Hz

    using std::cout;
    using std::endl;
    cout <<"Zero Time Thread created with periods of 1ms and 10ms"<<endl;
    cout <<"Zero Latency Thread created with periods of 5ms, 10ms and 100ms"<<endl;
    cout <<"Completion Processor created with periods of 10ms, 100ms and 1000ms"<<endl;

}

extern "C"
void cleanupFramework( void )
{
    // these calls try to delete the component
    
    HeartBeatGenerator::Release();
    
    CompletionProcessor::Release();
    ZeroTimeThread::Release();
    ZeroLatencyThread::Release();
    NonRealTimeThread::Release();
    
    delete events[0];
    delete events[1];
    delete events[2];
    delete events[3];
    delete events[4];
    delete events[5];
    delete events[6];
    delete events[7];
}


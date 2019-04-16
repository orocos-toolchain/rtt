/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.cpp

                        generictask_test.cpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


// need access to all TLSF functions embedded in RTT
// this call must occur before ALL RTT include's!!
#define ORO_MEMORY_POOL
#include <rtt/os/tlsf/tlsf.h>

#include <iostream>
#include <rtt/OperationCaller.hpp>
#include <rtt/Service.hpp>
#include <transports/corba/DataFlowI.h>
#include <rtt/transports/corba/RemotePorts.hpp>
#include <transports/corba/ServiceC.h>
#include <transports/corba/corba.h>
#include <rtt/InputPort.hpp>
#include <rtt/OutputPort.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/plugin/PluginLoader.hpp>
#include <transports/corba/TaskContextServer.hpp>
#include <transports/corba/TaskContextProxy.hpp>
#include <string>
#include <os/main.h>
#include "operations_fixture.hpp"
#include <fstream>

using namespace RTT;
using namespace RTT::detail;
using namespace std;

class TheServer : public TaskContext, public OperationsFixture
{
public:
    // Ports
    InputPort<double>  mi1;
    OutputPort<double> mo1;
    enum { INITIAL, CALL, SEND, FINAL } callBackPeer_step;
    int callBackPeer_count;

    TheServer(string name) : TaskContext(name), mi1("mi"), mo1("mo"), callBackPeer_step(INITIAL), callBackPeer_count(0) {
        ports()->addEventPort( mi1 );
        ports()->addPort( mo1 );
        this->createOperationCallerFactories( this );
        ts = corba::TaskContextServer::Create( this, /* use_naming = */ true );
        this->start();
        addOperation("callBackPeer", &TheServer::callBackPeer, this,ClientThread);
        addOperation("callBackPeerOwn", &TheServer::callBackPeer, this,OwnThread);
        addOperation("resetCallBackPeer", &TheServer::resetCallBackPeer, this,OwnThread);
    }
    ~TheServer() {
        this->stop();
    }

    void updateHook(){
        double d = 123456.789;
        FlowStatus fs = NoData;
        while( (fs = mi1.read(d, false)) == NewData ) {
            mo1.write(d);
        }
    }

    corba::TaskContextServer* ts;

    void callBackPeer(TaskContext* peer, string const& opname) {
        OperationCaller<void(TaskContext*, string const&)> op1(peer->getOperation(opname), this->engine());
        int count = ++callBackPeer_count;

        if (callBackPeer_step == INITIAL) callBackPeer_step = CALL;

        log(Info) << "Server executes callBackPeer():"<< count <<endlog();
        if (callBackPeer_step == CALL) {
            callBackPeer_step = SEND;
            log(Info) << "Server calls back peer:" << count << endlog();
            op1(this, "callBackPeerOwn");
            log(Info) << "Server finishes call back peer:" << count << endlog();
        }
        else if (callBackPeer_step == SEND) {
            callBackPeer_step = FINAL;
            log(Info) << "Server sends back peer:" << count << endlog();
            SendHandle<void(TaskContext*, string const&)> handle = op1.send(
                                                                       this, "callBackPeer");
            log(Info) << "Server finishes send back peer:" << count << endlog();
        }
        log(Info) << "Server finishes callBackPeer():" << count << endlog();
    }

    void resetCallBackPeer() {
        log(Info) << "Server resets callBackPeer state." <<endlog();
        callBackPeer_count = 0;
        callBackPeer_step = INITIAL;
    }

};

int ORO_main(int argc, char** argv)
{
#ifdef OS_RT_MALLOC
	void*   rtMem=0;
	size_t  freeMem=0;

	/// setup real-time memory allocation
	rtMem		= malloc(BUILD_TEST_RT_MEM_POOL_SIZE);	// don't calloc() as is first thing TLSF does.
	assert(0 != rtMem);
	freeMem		= init_memory_pool(BUILD_TEST_RT_MEM_POOL_SIZE, rtMem);
	assert((size_t)-1 != freeMem); // increase MEMORY_SIZE above most likely, as TLSF has a several kilobyte overhead
    (void)freeMem;          // avoid compiler warning
#endif
    corba::TaskContextProxy::InitOrb(argc,argv);

    PluginLoader::Instance()->loadTypekits("../rtt");

#ifndef WIN32
    pid_t pid = getpid();
    std::ofstream pidfile("corba-ipc-server.pid");
    pidfile << pid << endl;
    pidfile.close();
#endif
    {
        TheServer ctest1("peerRMC");
        TheServer ctest2("peerRM");
        TheServer ctest3("peerAM");
        TheServer ctest4("peerDFI");
        TheServer ctest5("peerPC");
        TheServer ctest6("peerPP");
        TheServer ctest7("peerDH");
        TheServer ctest8("peerBH");
        TheServer ctest9("peerRMCb");

        // wait for shutdown.
        corba::TaskContextServer::RunOrb();
    }

    corba::TaskContextProxy::DestroyOrb();

    return 0;
}

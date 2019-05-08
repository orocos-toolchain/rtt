/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  corba_mqueue_ipc_server.cpp

                        corba_mqueue_ipc_server.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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

#include <transports/corba/TaskContextServer.hpp>
#include <transports/corba/TaskContextProxy.hpp>
#include <rtt/Port.hpp>
#include <rtt/plugin/PluginLoader.hpp>
#include <os/main.h>
#include <fstream>

using namespace std;
using namespace RTT;
using namespace RTT::detail;

class TheServer : public TaskContext
{
public:
    // Ports
    InputPort<double>  mi1;
    OutputPort<double> mo1;

    TheServer(string name) : TaskContext(name), mi1("mr"), mo1("mw") {
        ports()->addEventPort( mi1 );
        ports()->addPort( mo1 );
        this->start();
        ts = corba::TaskContextServer::Create( this, /* use_naming = */ true );
    }
    ~TheServer() {
        this->stop();
    }

    void updateHook(){
        log(Info) << "Received data on port" <<endlog();
        double d = 123456.789;
        FlowStatus fs = NoData;
        while( (fs = mi1.read(d, false)) == NewData ) {
            mo1.write(d);
        }
    }

    corba::TaskContextServer* ts;

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
    std::ofstream pidfile("corba-mqueue-ipc-server.pid");
    pidfile << pid << endl;
    pidfile.close();
#endif

    {
        TheServer cmt("other");
        corba::TaskContextServer::RunOrb();
    }
    corba::TaskContextServer::ShutdownOrb(true);
    corba::TaskContextServer::DestroyOrb();
    return 0;
}



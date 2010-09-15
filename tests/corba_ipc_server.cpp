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

using namespace RTT;
using namespace RTT::detail;
using namespace std;

class TheServer : public TaskContext, public OperationsFixture
{
public:
    // Ports
    InputPort<double>  mi1;
    OutputPort<double> mo1;
    bool is_calling, is_sending;
    int cbcount;

    TheServer(string name) : TaskContext(name), mi1("mi"), mo1("mo"), is_calling(false), is_sending(false), cbcount(0) {
        ports()->addEventPort( mi1 );
        ports()->addPort( mo1 );
        this->createOperationCallerFactories( this );
        ts = corba::TaskContextServer::Create( this, true ); //use-naming
        this->start();
        addOperation("callBackPeer", &TheServer::callBackPeer, this,ClientThread);
        addOperation("callBackPeerOwn", &TheServer::callBackPeer, this,OwnThread);
    }
    ~TheServer() {
        this->stop();
    }

    void updateHook(){
        double d = 123456.789;
        mi1.read(d);
        mo1.write(d);
    }

    corba::TaskContextServer* ts;

    void callBackPeer(TaskContext* peer, string const& opname) {
    	int count = ++cbcount;
    	log(Info) << "Server executes callBackPeer():"<< count <<endlog();
    	OperationCaller<void(TaskContext*, string const&)> op1 =
				peer->getOperation(opname);
		if (!is_calling) {
			is_calling = true;
			log(Info) << "Server calls back peer:" << count << endlog();
			op1(this, "callBackPeerOwn");
			log(Info) << "Server finishes call back peer:" << count << endlog();
		}

		if (!is_sending) {
			is_sending = true;
			log(Info) << "Server sends back peer:" << count << endlog();
			SendHandle<void(TaskContext*, string const&)> handle = op1.send(
					this, "callBackPeer");
			log(Info) << "Server finishes send back peer:" << count << endlog();
		}
		log(Info) << "Server finishes callBackPeer():" << count << endlog();
    }

};

int ORO_main(int argc, char** argv)
{
    corba::TaskContextProxy::InitOrb(argc,argv);

    PluginLoader::Instance()->loadTypekits("../rtt");
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

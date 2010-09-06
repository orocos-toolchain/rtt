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



#include <transports/corba/TaskContextServer.hpp>
#include <transports/corba/TaskContextProxy.hpp>
#include <rtt/Port.hpp>
#include <os/main.h>

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
        ts = corba::TaskContextServer::Create( this, false ); //use-naming
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

};

int ORO_main(int argc, char** argv)
{
    corba::TaskContextProxy::InitOrb(argc,argv);
    {
        TheServer cmt("other");
        corba::TaskContextServer::RunOrb();
    }
    corba::TaskContextServer::ShutdownOrb(true);
    corba::TaskContextServer::DestroyOrb();
    return 0;
}



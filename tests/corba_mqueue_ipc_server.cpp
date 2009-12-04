
#include <transports/corba/ControlTaskServer.hpp>
#include <transports/corba/ControlTaskProxy.hpp>
#include <Port.hpp>
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
        ports()->addEventPort( &mi1 );
        ports()->addPort( &mo1 );
        this->start();
        ts = corba::ControlTaskServer::Create( this, true ); //use-naming
    }
    ~TheServer() {
        this->stop();
    }

    void updateHook(){
        double d = 123456.789;
        mi1.read(d);
        mo1.write(d);
    }

    corba::ControlTaskServer* ts;

};

int ORO_main(int argc, char** argv)
{
    corba::ControlTaskProxy::InitOrb(argc,argv);
    {
        TheServer cmt("other");
        corba::ControlTaskServer::RunOrb();
    }
    corba::ControlTaskServer::ShutdownOrb(true);
    corba::ControlTaskServer::DestroyOrb();
    return 0;
}



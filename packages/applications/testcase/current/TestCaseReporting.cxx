
#include "TestCaseReporting.hpp"

#include <corelib/FifoRTIn.hpp>
#include <corelib/FifoRTOut.hpp>

namespace ORO_CoreLib
{
    extern fifo_info_t rt_fifos[64];
}
namespace UnitTesting
{
    using namespace ORO_CoreLib;

  
    TestCaseReporting::TestCaseReporting( TestSuite* suite, WriteInterface* output ) 
        : TestCase( "TestCaseReporting", suite, 10 ), out(output), 
#ifndef HAVE_FIFO_RT
          rtfIn(new FifoRTIn(18)), 
          notifier(output, rtfIn),
#endif
          errorStr("ReportWriter error"), toMonitor("M","Monitored variable")
    {
        rtf = new FifoRTOut(18,8096);
        rw = new ReportWriter( output ); // was : rtf
        repBag = new Reporter("TestReport", Reporter::XML);

#ifndef HAVE_FIFO_RT
        rtfIn->addHandler(&notifier,Completer::None);
#endif
        rw->exporterAdd(repBag);
    }
  
    TestCaseReporting::~TestCaseReporting()
    {
        rw->exporterRemove(repBag);
        delete rw;
        delete repBag;
        delete rtf;
#ifndef HAVE_FIFO_RT
        delete rtfIn;
#endif
    }

    bool TestCaseReporting::initialize()
    {
        char message[ 50 ] = "Initialize TestCaseReporting...\n";
        out->write( message, strlen( message ) + 1 );
        repBag->add( &toMonitor );
        toMonitor=13.13;
        rw->start();
        return true;
    }

    void TestCaseReporting::step()
    {
        toMonitor = toMonitor+1;
    }

    void TestCaseReporting::finalize()
    {
        rw->stop();
        repBag->remove( &toMonitor );
        char message[ 50 ] = "Finalize TestCaseReporting...\n";
        out->write( message, strlen( message ) + 1 );
    }
    
#ifndef HAVE_FIFO_RT
    TestCaseReporting::Writer::Writer(WriteInterface * out, ObservableReadInterface* in) : writer(out), reader(in) {}

    void TestCaseReporting::Writer::handleEvent( CallbackInterface* )
    {
        char buf[100];
        int tot = reader->read(buf,100);
        //cout << "TOTAL IS " <<tot<<endl;
        buf[tot+1] = 0;
        writer->write(buf,tot+1 );
        writer->write("\n",2);
    }
#endif
}

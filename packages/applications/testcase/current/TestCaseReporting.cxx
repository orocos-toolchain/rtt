/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseReporting.cxx 

                        TestCaseReporting.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

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

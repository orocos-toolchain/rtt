/**
 * @file tests.cxx
 * In this file all test cases are added to the test suite.
 */

#include "TestSuite.hpp"
#include "TestCaseSimple.hpp"
//#include "TestCaseConditionVariableTimed.hpp"
#include "TestCaseEvent.hpp"
#include "TestCaseEventInterrupt.hpp"
#include "TestCaseHeartBeatGenerator.hpp"
#include "TestCaseTiming.hpp"
#include "TestCaseReporting.hpp"
#include "TestRunnerNonInteractive.hpp"
#include "TestCaseTasks.hpp"
//#include "TestCaseSimpleDemarshaller.hpp"
//#include "TestCaseXMLDemarshaller.hpp"
//#include "TestCaseXMLRPCDemarshaller.hpp"
#include "TestCaseCPFDemarshaller.hpp"
#include "TestCaseCPFMarshaller.hpp"
#include <corelib/WriteCout.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_OS
#include <pkgconf/os.h>
#endif

using namespace ORO_CoreLib;

namespace UnitTesting
{

    void TestSuite::createTests( WriteInterface* out)
    {
        // Add your own test cases here.

        new TestCaseSimple( this, out );

#ifdef OROPKG_CORELIB_EVENTS
        new TestCaseEvent( this );
#endif
#ifdef OROINT_OS_EVENT_INTERRUPT
        new TestCaseEventInterrupt( this );
#endif

#ifdef OROPKG_CORELIB_TIMING
        new TestCaseTiming( this, out );
        new TestCaseHeartBeatGenerator( this );
#endif
        // cond vars fails for now
        //new TestCaseConditionVariableTimed( this );
#ifdef OROPKG_CORELIB_TASKS
        new TestCaseTasks( this );
#endif
        
  //      ********************
  //      All these tests are disabled, because they crash LXRT
  //      ********************
  //      new TestCaseSimpleDemarshaller ( this, out );
  //      new TestCaseXMLDemarshaller ( this, out );
  //      new TestCaseXMLRPCDemarshaller ( this, out );
  //      new TestCaseCPFDemarshaller ( this );
  //      new TestCaseCPFMarshaller ( this );
  //      new TestCaseReporting(this, out);

    }
}

/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  tests.cxx 

                        tests.cxx -  description
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

/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestMain.cxx 

                        TestMain.cxx -  description
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
#include "TestMain.hpp"
#include <os/fosi.h>
#include <corelib/Event.hpp>

//void quit();

namespace UnitTesting
{

    Event *quitEvent;

    TestMain::TestMain( EventInterface* ei ) :
            qEvent( ei ), ttq( false )
    {
        qEvent->addHandler( this, this );
    }

    TestMain::~TestMain()
    {
        qEvent->removeHandler( this, this );
    }

    bool TestMain::timeToQuit()
    {
        return ttq;
    }

    void TestMain::handleEvent( CallbackInterface* event )
    {
        event->complete();
    }

    void TestMain::completeEvent()
    {
        ttq = true;
    }

}

using namespace UnitTesting;
using namespace std;
    
#include <iostream>
#include <os/main.h>

int ORO_main(int argc, const char** argv)
{
    cout << "Starting Tests..."<<endl;
    quitEvent = new Event(Event::SYNASYN);

    TestMain* testMain = new TestMain( quitEvent );

    // create the TestSuite and TestRunner
    retValue = testInit( -1 );
    //Now we wait until the TestRunner has finished..
    
    while ( !testMain->timeToQuit() )
        {
            sleep(1);
        }
    
    cout << "Tests are finished..."<<endl;

    testTerminate();
    
    delete testMain;
    delete quitEvent;
    return retValue;
}

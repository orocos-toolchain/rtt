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

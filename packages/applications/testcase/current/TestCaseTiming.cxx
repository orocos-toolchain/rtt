#include "TestCaseTiming.hpp"

extern "C"
    void parport_write( unsigned char );


namespace UnitTesting
{

    TestCaseTiming::TestCaseTiming( TestSuite* suite, WriteInterface* output ) : TestCase( "TestCaseTiming", suite, 50 ), out( output ), outbit( 0 )
    {}

    TestCaseTiming::~TestCaseTiming()
    {}

    bool TestCaseTiming::initialize()
    {
        //char message[ 50 ] = "Initialize TestCaseTiming...\n";
        //out->write( message, strlen( message ) + 1 );
        return true;
    }

    void TestCaseTiming::step()
    {
        if ( outbit == 0 )
            outbit = 1;
        else
            outbit = 0;

#ifdef OROINT_OS_KERNEL
        parport_write( outbit );
#endif
    }

    void TestCaseTiming::finalize()
    {
        //char message[ 50 ] = "Finalize TestCaseTiming...\n";
        //out->write( message, strlen( message ) + 1 );
    }

}

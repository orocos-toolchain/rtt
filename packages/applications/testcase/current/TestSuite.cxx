#include "TestSuite.hpp"

namespace UnitTesting
{

    TestSuite::TestSuite(WriteInterface* out)
    {
        tests.reserve( 32 );
        reset();
        createTests(out);
    }

    TestSuite::~TestSuite()
    {
        for ( TestList::iterator itl = tests.begin(); itl != tests.end(); ++itl)
            delete (*itl);
        tests.clear();
    }

    void TestSuite::addTest( TestCaseInterface* testCase )
    {
        tests.push_back( testCase );
    }

    TestCaseInterface* TestSuite::getNextTest()
    {
        // return old, increment to reach new
        return * ( nextTest++ );
    }

    int TestSuite::testsNumber()
    {
        return tests.size();
    }

    bool TestSuite::atEnd()
    {
        return ( nextTest == tests.end() );
    }

    void TestSuite::reset()
    {
        nextTest = tests.begin();
    }

    int TestSuite::errorsNumber()
    {
        int n = 0;

        for ( TestList::iterator i = tests.begin(); i != tests.end(); ++i )
        {
            n += ( *i ) ->errorsNumber();
        }

        return n;

    }

    int TestSuite::testsFailed()
    {
        int n = 0;
        reset();

        while ( !atEnd() )
        {
            n += ( getNextTest() ->errorsNumber() != 0 ? 1 : 0 );
        }

        return n;

    }

}


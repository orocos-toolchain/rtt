#include "TestCaseXMLRPCDemarshaller.hpp"

namespace UnitTesting
{

    using namespace rt_std;
    
    TestCaseXMLRPCDemarshaller::TestCaseXMLRPCDemarshaller( TestSuite* suite )
        : TestCase( "TestCaseXMLRPCDemarshaller", suite, 13 ), stepCounter( 0 ),
          errorAssert( "The assert construction doesn't work.\n" ),
          errorStep( "Step doesn't work.\n" )
    {
    }

    TestCaseXMLRPCDemarshaller::~TestCaseXMLRPCDemarshaller()
    {
    }

    bool TestCaseXMLRPCDemarshaller::initialize()
    {
    }

    void TestCaseXMLRPCDemarshaller::step()
    {
        cout <<".";
        stepCounter++;
        // Test if the assert construction works.
    	XMLRPCDemarshaller<istream> ds( cin );
		ds.deserialize( bag );
	
		cout << "The list of incarnated properties: " << endl;
		vector<std::string> names;
		bag.list( names );
		for ( vector<std::string>::iterator i = names.begin(); i != names.end(); ++i )
			cout << *i << "\n";
    	isSet = true;
        testAssert( isSet, errorAssert );

    }

    void TestCaseXMLRPCDemarshaller::finalize()
    {
        testAssert( stepCounter == 13, errorStep );
    }

}

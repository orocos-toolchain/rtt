/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseXMLRPCDemarshaller.cxx 

                        TestCaseXMLRPCDemarshaller.cxx -  description
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

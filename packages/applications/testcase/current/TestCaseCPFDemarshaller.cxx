/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseCPFDemarshaller.cxx 

                        TestCaseCPFDemarshaller.cxx -  description
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
#include "TestCaseCPFDemarshaller.hpp"

#include <sstream>

namespace UnitTesting
{

    using namespace std;
    
    TestCaseCPFDemarshaller::TestCaseCPFDemarshaller( TestSuite* suite )
        : TestCase( "TestCaseCPFDemarshaller", suite, 1 ), stepCounter( 0 ),
          errorDemarshallingAsCPF( "CPF demarshalling error\n" )
    {
    }

    TestCaseCPFDemarshaller::~TestCaseCPFDemarshaller()
    {
    }

    bool TestCaseCPFDemarshaller::initialize()
    {
        return true;
    }

    void TestCaseCPFDemarshaller::step()
    {
        string serialized_contents = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<properties>\n"
            "<simple name=\"counter\" type=\"long\"><description>counts stuff</description><value>10</value></simple>\n"
            "<simple name=\"alfa\" type=\"char\"><description>our speed</description><value>b</value></simple>\n"
            "<simple name=\"legal\" type=\"boolean\"><description>our speed</description><value>0</value></simple>\n"
            "<simple name=\"speed\" type=\"double\"><description>our speed</description><value>120</value></simple>\n"
            "<simple name=\"mass\" type=\"double\"><description>should we leave the chocolate</description><value>76</value></simple>\n"
            "<simple name=\"iq\" type=\"double\"><description>uh oh</description><value>0</value></simple>\n"
            "<simple name=\"address\" type=\"string\"><description>where do we live</description><value>Hoogbaan 11b</value></simple>\n"
            "<struct name=\"maxima\" type=\"type_less\">\n"
            "<simple name=\"maxspeed\" type=\"double\"><description>our speed</description><value>80</value></simple>\n"
            "<simple name=\"maxmass\" type=\"double\"><description>our maximum mass</description><value>80</value></simple>\n"
            "</struct>\n\n"
            "</properties>\n";

        stringstream s;
        s <<serialized_contents;
//        CPFDemarshaller<stringstream> dms( s );
//        FIXME: Problem with ambiguity, XMLChar ~ char

        PropertyBag bag;
//        dms.deserialize( bag );
//

        Property<int> i( "counter", "counts stuff", 0 );
        i = 10;

        PropertyBag expected_bag;
        expected_bag.add( &i );

        Property<char> alfabet( "alfa", "our speed", 'a' );
        alfabet = 'b';
        expected_bag.add( &alfabet );

        Property<bool> legal( "legal", "our speed", true );
        legal = false;
        expected_bag.add( &legal );

        Property<double> speed( "speed", "our speed", 110 );
        speed = 120;
        expected_bag.add( &speed );

        Property<double> mass( "mass", "should we leave the chocolate", 70 );
        mass = 76;
        expected_bag.add( &mass );

        Property<double> iq( "iq", "uh oh", 0 );
        expected_bag.add( &iq );

        Property<string> address( "address", "where do we live", "Hoogbaan 11b" );
        expected_bag.add( &address );


        PropertyBag maxima;

        Property<double> maxspeed( "maxspeed", "our speed", 120 );
        maxspeed = 120;
        maxima.add( &maxspeed );

        Property<double> maxmass( "maxmass", "our maximum mass", 80 );
        maxspeed = 80;
        maxima.add( &maxmass );

        Property<PropertyBag> psubexpected_bag( "maxima", "all the maxima", maxima );
        expected_bag.add( &psubexpected_bag );

        //testAssert( bag == expected_bag, errorDemarshallingAsCPF ); 
        // FIXME: Implement comparison operator for PropertyBags
        testAssert( false, errorDemarshallingAsCPF ); 
    }

    void TestCaseCPFDemarshaller::finalize()
    {
    }

}

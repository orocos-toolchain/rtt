/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseCPFMarshaller.cxx 

                        TestCaseCPFMarshaller.cxx -  description
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
#include "TestCaseCPFMarshaller.hpp"

#include <sstream>

namespace UnitTesting
{

    using namespace std;
    
    TestCaseCPFMarshaller::TestCaseCPFMarshaller( TestSuite* suite )
        : TestCase( "TestCaseCPFMarshaller", suite, 1 ), stepCounter( 0 ),
          errorMarshallingAsCPF( "CPF marshalling error\n" )
    {
    }

    TestCaseCPFMarshaller::~TestCaseCPFMarshaller()
    {
    }

    bool TestCaseCPFMarshaller::initialize()
    {
        return true;
    }

    void TestCaseCPFMarshaller::step()
    {
        stepCounter++;

        Property<int> i( "counter", "counts stuff", 0 );
        i = 10;

        PropertyBag bag;
        bag.add( &i );

        Property<char> alfabet( "alfa", "our speed", 'a' );
        alfabet = 'b';
        bag.add( &alfabet );

        Property<bool> legal( "legal", "our speed", true );
        legal = false;
        bag.add( &legal );

        Property<double> speed( "speed", "our speed", 110 );
        speed = 120;
        bag.add( &speed );

        Property<double> mass( "mass", "should we leave the chocolate", 70 );
        mass = 76;
        bag.add( &mass );

        Property<double> iq( "iq", "uh oh", 0 );
        bag.add( &iq );

        Property<string> address( "address", "where do we live", "Hoogbaan 11b" );
        bag.add( &address );


        PropertyBag maxima;

        Property<double> maxspeed( "maxspeed", "our speed", 120 );
        maxspeed = 120;
        maxima.add( &maxspeed );

        Property<double> maxmass( "maxmass", "our maximum mass", 80 );
        maxspeed = 80;
        maxima.add( &maxmass );

        Property<PropertyBag> psubbag( "maxima", "all the maxima", maxima );
        bag.add( &psubbag );

        std::stringstream s;
        CPFMarshaller<std::stringstream> ms( s );
        ms.serialize( bag );

        // Compare serialized stream with the expected stream.

        stringbuf *pbuf;
        pbuf = s.rdbuf();
        
        string contents = pbuf->str();


        string expected_contents = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
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
        cout <<contents << expected_contents;

       
        testAssert( contents == expected_contents, errorMarshallingAsCPF);
        
    }
    

    void TestCaseCPFMarshaller::finalize()
    {
    }

}

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

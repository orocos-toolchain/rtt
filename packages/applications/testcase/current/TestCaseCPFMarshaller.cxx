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

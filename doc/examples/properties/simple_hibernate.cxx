#include <iostream>
#include <string>
#include "Property.hpp"
#include "SimpleMarshaller.hpp"

int main()
{
	using namespace std;
	using namespace StoneHead;
	
    Property<int> i( "counter", "counts stuff", 0 );
    i = 10;
    int j = i;

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

    SimpleMarshaller<ostream> ms( cout );
    ms.serialize( bag );

    return 0;
}

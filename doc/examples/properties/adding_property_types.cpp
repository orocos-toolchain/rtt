#include <iostream>
#include <string>
#include "Property.hpp"
#include "XMLMarshaller.hpp"
#include "SimpleMarshaller.hpp"

using namespace std;
using namespace StoneHead;

struct Vector
{
    double x,y,z;
};

void decomposeProperty(Marshaller &m, const Property<Vector> &b)
{
    PropertyBag bag;
    Property<double> x("X","x",b.get().x);
    Property<double> y("Y","y",b.get().y);
    Property<double> z("Z","z",b.get().z);
    bag.add(&x);
    bag.add(&y);
    bag.add(&z);
    Property<PropertyBag> pbag("vector","Vector",bag);
    m.serialize(pbag);
}

struct Rectangle
{
    double x, y, width, height;
};

int main()
{
    PropertyBag bag;

    Property<double> maxmass( "maxmass", "our maximum mass", 80 );
    bag.add( &maxmass );

    Property<bool> ok( "ok", "okk", true );
    bag.add( &ok );

    Property<char> charac( "C", "okk", 'R' );
    bag.add( &charac );

    Vector v;
    v.x = 1;
    v.y = 2;
    v.z = 3;
    Property<Vector> pv( "pos", "position", v );
    bag.add( &pv );

    Rectangle r;
	r.x = 0;
	r.y = 0;
	r.width = 640;
	r.height = 480;
    Property<Rectangle> pr( "rec", "A rectangle", r );
    bag.add( &pr );

    cout << "Serializer\n";
    //    SimpleMarshaller<ostream> ms(cout);
    XMLMarshaller<ostream> ms( cout );
    ms.serialize( bag );
    return 0;
}

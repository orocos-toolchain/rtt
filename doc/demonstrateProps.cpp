#include <property/PropertyFactory.hpp>
#include <property/PropertySet.hpp>
#include <property/StringParser.hpp>

using namespace StoneHead;

typedef PropertyFactory PF;

int main( int argc, char *argv[] )
{
    PropertyInstance<string> *prop1 = PF::createString( "prop1", "", "string1" );
    PropertyInstance<bool> *prop2 = PF::createBool( "prop2", "", "true" );
    PropertyInstance<int> *prop3 = PF::createInt( "prop3", "", 10 );
    PropertyInstance<char> *prop4 = PF::createChar( "prop4", "", 'A' );
    PropertyInstance<float> *prop5 = PF::createFloat( "prop5", "", 1.1 );
    PropertyInstance<double> *prop6 = PF::createDouble( "prop6", "", 11.11 );
    PropertyInstance<PropertySet> *pset1 = PF::createPSet( "pset1", "" );
    PropertyInstance<PropertySet> *pset2 = PF::createPSet( "pset2", "" );

    *pset2 << prop1 << prop2 << prop3;
    *pset1 << prop4 << prop5 << prop6 << pset2;

    /**
     * pset1:
     * ------prop4
     * ------prop5
     * ------prop6
     * ------pset2:
     * ------------prop1
     * ------------prop2
     * ------------prop3
     */
}


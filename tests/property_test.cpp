 
 


#include "property_test.hpp"
#include <boost/bind.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PropertyTest );

using namespace ORO_CoreLib;
using namespace boost;

void 
PropertyTest::setUp()
{
    intref = 99;
    pi1 = new Property<int>("pi1","pi1d", 0 );
    pi2 = new Property<int>("pi2","pi2d", 0 );
    pi1ref =  dynamic_cast< Property<int>* >( pi1->clone() );
    pi2ref =  dynamic_cast< Property<int>* >( pi2->clone() );
}


void 
PropertyTest::tearDown()
{
    delete pi1;
    delete pi2;
    delete pi1ref;
    delete pi2ref;
}

void PropertyTest::testPrimitives()
{
    *pi1 = intref;
    *pi2 = 0;

    // update semantics
    pi1->update( *pi2 ); 
    CPPUNIT_ASSERT_EQUAL( pi2->get(), pi1->get() );
    CPPUNIT_ASSERT_EQUAL( pi1ref->getName(), pi1->getName() );
    CPPUNIT_ASSERT_EQUAL( pi1ref->getDescription(), pi1->getDescription() );
    *pi2 = 0;
    
    // update with PropertyBase.
    CPPUNIT_ASSERT( pi1->update( pi2 ) ); 
    CPPUNIT_ASSERT_EQUAL( pi2->get(), pi1->get() );
    CPPUNIT_ASSERT_EQUAL( pi1ref->getName(), pi1->getName() );
    CPPUNIT_ASSERT_EQUAL( pi1ref->getDescription(), pi1->getDescription() );
    *pi2 = 0;

    // copy semantics
    pi1->copy( *pi2 ); 
    CPPUNIT_ASSERT_EQUAL( pi2->get(), pi1->get() );
    CPPUNIT_ASSERT_EQUAL( pi2ref->getName(), pi1->getName() );
    CPPUNIT_ASSERT_EQUAL( pi2ref->getDescription(), pi1->getDescription() );
    pi1->copy( *pi1ref );

    // copy with PropertyBase.
    CPPUNIT_ASSERT( pi1->copy( pi2 ) ); 
    CPPUNIT_ASSERT_EQUAL( pi2->get(), pi1->get() );
    CPPUNIT_ASSERT_EQUAL( pi2ref->getName(), pi1->getName() );
    CPPUNIT_ASSERT_EQUAL( pi2ref->getDescription(), pi1->getDescription() );
    pi1->copy( pi1ref );
}
void PropertyTest::testBags()
{
}
void PropertyTest::testBagOperations()
{
}

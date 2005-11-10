/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:26 CEST 2005  property_test.cpp 

                        property_test.cpp -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
 
 


#include "property_test.hpp"
#include <boost/bind.hpp>
#include <corelib/PropertyBagIntrospector.hpp>
#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
#include <geometry/frames.h>
using namespace ORO_Geometry;
#endif

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
    PropertyBag bag;

    Property<float> pf("pf","pfd", -1.0);
    Property<double> pd("pd","pdd", +1.0);
    Property<std::string> ps("ps","psd", "std::string");
    Property<char> pc("pc","pcd", 'c');
    
    Property<PropertyBag> subbag1("s1", "s1d");
    Property<PropertyBag> subbag2("s2", "s2d");

    bag.add( pi1 );
    bag.add( pi2 );
    bag.add( &subbag1 );
    subbag1.set().add( &subbag2 );
    subbag1.set().add( &pf );
    subbag1.set().add( &pd );

    subbag2.set().add( &ps );
    subbag2.set().add( &pc );

    // non recursive search :
    CPPUNIT_ASSERT( bag.find( "pf" ) == 0 );
    CPPUNIT_ASSERT( bag.find( "s1" ) == &subbag1 );
    CPPUNIT_ASSERT( bag.find( "pi1" ) == pi1 );

    // recursive search :
    CPPUNIT_ASSERT( find( bag, "/pf", "/" ) == 0 );
    CPPUNIT_ASSERT( find( bag, "::pi1" ) == pi1 ); // default is ::
    CPPUNIT_ASSERT( find( bag, "s1" ) == &subbag1 );
    CPPUNIT_ASSERT( find( bag, "pi1" ) == pi1 );
    CPPUNIT_ASSERT( find( bag, "/s1/s2", "/" ) == &subbag2 );
    CPPUNIT_ASSERT( find( bag, "/s1/s2/ps", "/" ) == &ps );
    CPPUNIT_ASSERT( find( bag, "s1::s2::pc" ) == &pc );
                    
}
void PropertyTest::testBagOperations()
{
}

using namespace ORO_Geometry;

void PropertyTest::testMarshalling()
{
#ifdef OROPKG_GEOMETRY
    // decompose some types
    PropertyBag bag;
    PropertyBagIntrospector  inspector( bag );

    Property<Wrench> w("Wrench","A Wrench", Wrench(Vector(0.1,0.2,0.3),Vector(0.5,0.6,0.7)));

    inspector.introspect( w );

    CPPUNIT_ASSERT( bag.find("Wrench") != 0 );

    CPPUNIT_ASSERT( find( bag, "Wrench::Force::X" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Force::Y" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Force::Z" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Torque::X" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Torque::Y" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Torque::Z" ) != 0 );

    flattenPropertyBag(bag);
    deleteProperties(bag);
#endif
}

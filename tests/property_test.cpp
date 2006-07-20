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
#include <rtt/PropertyBagIntrospector.hpp>
#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
#include <geometry/frames.h>
#include <geometry/GeometryToolkit.hpp>
using namespace ORO_Geometry;
#endif

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PropertyTest );

using namespace RTT;
using namespace boost;
using namespace std;

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

bool operator==(const std::vector<double>& a, const std::vector<double>& b)
{
    if ( a.size() != b.size() )
        return false;
    for(unsigned int i =0; i != a.size(); ++i)
        {
            if (a[i] != b[i])
                return false;
        }
    return true;
}

void PropertyTest::testRepository()
{
    /**
     * Test all types: create property of type T, decompose it and compose it again.
     */
    std::vector<string> names = TypeInfoRepository::Instance()->getTypes();
    for (std::vector<string>::iterator it = names.begin(); it != names.end(); ++it) {
        PropertyBase* target;
        Property<PropertyBag> bag("Result","D");
        CPPUNIT_ASSERT( TypeInfoRepository::Instance()->type( *it ) );
        target = TypeInfoRepository::Instance()->type( *it )->buildProperty("Result", "D");
        CPPUNIT_ASSERT( target );
        if ( target->getTypeInfo()->decomposeType( target->getDataSource(), bag.value() ) )
            CPPUNIT_ASSERT( target->getTypeInfo()->composeType( bag.getDataSource() , target->getDataSource() ) );
        deletePropertyBag( bag.value() );
        delete target;
    }

}

void PropertyTest::testComposition()
{
    /**
     * test vector
     */
    std::vector<double> init(33, 1.0);
    Property<std::vector<double> > pvd("pvd","pvd desc", init);
    Property<const std::vector<double>& > pvd_cr("pvd_cr","pvd_cr desc", init);

    Property<std::vector<double> > pvd2("pvd 2","pvd desc 2");
    Property<const std::vector<double>& > pvd_cr2("pvd_cr 2","pvd desc 2");

    CPPUNIT_ASSERT( pvd.get() == init );
    CPPUNIT_ASSERT( pvd_cr.get() == init );
    CPPUNIT_ASSERT( pvd.set() == init );
    CPPUNIT_ASSERT( pvd_cr.set() == init );

    CPPUNIT_ASSERT( pvd.getTypeInfo() );
    CPPUNIT_ASSERT( pvd.getTypeInfo() == pvd_cr.getTypeInfo() );
    // Compatible-type -assignment:
    CPPUNIT_ASSERT( pvd.getTypeInfo()->composeType( pvd.getDataSource(), pvd2.getDataSource() ) );
    CPPUNIT_ASSERT( pvd.getTypeInfo()->composeType( pvd_cr.getDataSource(), pvd.getDataSource() ) );
    CPPUNIT_ASSERT( pvd.getTypeInfo()->composeType( pvd.getDataSource(), pvd_cr.getDataSource() ) );
    CPPUNIT_ASSERT( pvd.getTypeInfo()->composeType( pvd_cr.getDataSource(), pvd_cr2.getDataSource() ) );

    Property<PropertyBag> bag("Result","Rd");
    // Decompose to property bag and back:
    CPPUNIT_ASSERT( pvd.getTypeInfo()->decomposeType( pvd.getDataSource(), bag.value() ) );
    CPPUNIT_ASSERT( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd2.getDataSource() ) );
    CPPUNIT_ASSERT( pvd == pvd2);
    pvd2.value().clear();
    deletePropertyBag( bag.value() );

    CPPUNIT_ASSERT( pvd.getTypeInfo()->decomposeType( pvd_cr.getDataSource(), bag.value() ) );
    CPPUNIT_ASSERT( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd_cr2.getDataSource() ) );
    CPPUNIT_ASSERT( pvd_cr == pvd_cr2);
    pvd_cr2.value().clear();
    deletePropertyBag( bag.value() );

    // Cross composition. (const ref to value and vice versa)
    CPPUNIT_ASSERT( pvd.getTypeInfo()->decomposeType( pvd.getDataSource(), bag.value() ) );
    CPPUNIT_ASSERT( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd_cr2.getDataSource() ) );
    CPPUNIT_ASSERT( pvd == pvd_cr2);
    pvd_cr2.value().clear();
    deletePropertyBag( bag.value() );

    CPPUNIT_ASSERT( pvd.getTypeInfo()->decomposeType( pvd_cr.getDataSource(), bag.value() ) );
    CPPUNIT_ASSERT( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd2.getDataSource() ) );
    CPPUNIT_ASSERT( pvd_cr == pvd2);
    pvd2.value().clear();
    deletePropertyBag( bag.value() );
}


#ifdef OROPKG_GEOMETRY
using namespace ORO_Geometry;
#endif

void PropertyTest::testMarshalling()
{
#ifdef OROPKG_GEOMETRY
    if ( !Toolkit::hasTool("Geometry")) 
        Toolkit::Import( GeometryToolkit );

    // decompose some types
    PropertyBag bag;
    PropertyBagIntrospector  inspector( bag );

    Property<Wrench> w("Wrench","A Wrench", Wrench(Vector(0.1,0.2,0.3),Vector(0.5,0.6,0.7)));

    inspector.introspect( &w );

    CPPUNIT_ASSERT( bag.find("Wrench") != 0 );

    CPPUNIT_ASSERT( find( bag, "Wrench::Force" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Force::X" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Force::Y" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Force::Z" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Torque" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Torque::X" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Torque::Y" ) != 0 );
    CPPUNIT_ASSERT( find( bag, "Wrench::Torque::Z" ) != 0 );

    flattenPropertyBag(bag);
    deleteProperties(bag);
#endif
}

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
#include <marsh/PropertyBagIntrospector.hpp>
#include <extras/MultiVector.hpp>
#include <marsh/PropertyMarshaller.hpp>
#include <marsh/PropertyDemarshaller.hpp>
#include <internal/DataSourceTypeInfo.hpp>

#include "unit.hpp"

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

BOOST_FIXTURE_TEST_SUITE( PropertyTestSuite, PropertyTest )

BOOST_AUTO_TEST_CASE( testPrimitives )
{
    *pi1 = intref;
    *pi2 = 0;

    // update semantics
    pi1->update( *pi2 );
    BOOST_REQUIRE_EQUAL( pi2->get(), pi1->get() );
    BOOST_REQUIRE_EQUAL( pi1ref->getName(), pi1->getName() );
    BOOST_REQUIRE_EQUAL( pi1ref->getDescription(), pi1->getDescription() );
    *pi2 = 0;

    // update with PropertyBase.
    BOOST_CHECK( pi1->update( pi2 ) );
    BOOST_REQUIRE_EQUAL( pi2->get(), pi1->get() );
    BOOST_REQUIRE_EQUAL( pi1ref->getName(), pi1->getName() );
    BOOST_REQUIRE_EQUAL( pi1ref->getDescription(), pi1->getDescription() );
    *pi2 = 0;

    // copy semantics
    pi1->copy( *pi2 );
    BOOST_REQUIRE_EQUAL( pi2->get(), pi1->get() );
    BOOST_REQUIRE_EQUAL( pi2ref->getName(), pi1->getName() );
    BOOST_REQUIRE_EQUAL( pi2ref->getDescription(), pi1->getDescription() );
    pi1->copy( *pi1ref );

    // copy with PropertyBase.
    BOOST_CHECK( pi1->copy( pi2 ) );
    BOOST_REQUIRE_EQUAL( pi2->get(), pi1->get() );
    BOOST_REQUIRE_EQUAL( pi2ref->getName(), pi1->getName() );
    BOOST_REQUIRE_EQUAL( pi2ref->getDescription(), pi1->getDescription() );
    pi1->copy( pi1ref );
}


BOOST_AUTO_TEST_CASE( testBags )
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
    BOOST_CHECK( bag.find( "pf" ) == 0 );
    BOOST_CHECK( bag.find( "s1" ) == &subbag1 );
    BOOST_CHECK( bag.find( "pi1" ) == pi1 );

    // recursive search :
    BOOST_CHECK( findProperty( bag, "/pf", "/" ) == 0 );
    BOOST_CHECK( findProperty( bag, ".pi1" ) == pi1 ); // default is "."
    BOOST_CHECK( findProperty( bag, "s1" ) == &subbag1 );
    BOOST_CHECK( findProperty( bag, "pi1" ) == pi1 );
    BOOST_CHECK( findProperty( bag, "/s1/s2", "/" ) == &subbag2 );
    BOOST_CHECK( findProperty( bag, "/s1/s2/ps", "/" ) == &ps );
    BOOST_CHECK( findProperty( bag, "s1.s2.pc" ) == &pc );

}

BOOST_AUTO_TEST_CASE( testBagOperations )
{
}

bool operator==(const std::vector<double>& a, const std::vector<double>& b)
{
    if ( a.size() != b.size() ) {
        log(Error) << "Wrong vector sizes : " << a.size() <<" "<< b.size()<<endlog();
        return false;
    }
    for(unsigned int i =0; i != a.size(); ++i)
        {
            if (a[i] != b[i]) {
                log(Error) << "Wrong vector element: "<<a[i]<<" != "<<b[i]<<" i:" << i<<endlog();
                return false;
            }
        }
    return true;
}

BOOST_AUTO_TEST_CASE( testRepository )
{
    /**
     * Test all types: create property of type T, decompose it and compose it again.
     */
	BOOST_MESSAGE("----- Testing testRep");
    std::vector<string> names = TypeInfoRepository::Instance()->getTypes();
    for (std::vector<string>::iterator it = names.begin(); it != names.end(); ++it) {
    	BOOST_MESSAGE("----------- loop names: " << *it);
        PropertyBase* target;
        Property<PropertyBag> bag("Result","D");
        BOOST_REQUIRE( TypeInfoRepository::Instance()->type( *it ) );
        target = TypeInfoRepository::Instance()->type( *it )->buildProperty("Result", "D");
        if ( target && target->getTypeInfo()->decomposeType( target->getDataSource(), bag.value() ) )
            BOOST_CHECK( target->getTypeInfo()->composeType( bag.getDataSource() , target->getDataSource() ) );
        deletePropertyBag( bag.value() );
        delete target;
    }

}
#include <typeinfo>

BOOST_AUTO_TEST_CASE( testComposition )
{
    /**
     * test vector
     */
    std::vector<double> init(33, 1.0);
    Property<std::vector<double> > pvd("pvd","pvd desc", init);
    Property<const std::vector<double>& > pvd_cr("pvd_cr","pvd_cr desc", init);

    //std::cout << "\n\n\n "<< std::string( typeid(init).name() ) << "\n\n\n "<<std::endl;

    Property<std::vector<double> > pvd2("pvd 2","pvd desc 2");
    Property<const std::vector<double>& > pvd_cr2("pvd_cr 2","pvd desc 2");

    BOOST_CHECK( pvd.get() == init );
    BOOST_CHECK( pvd_cr.get() == init );
    BOOST_CHECK( pvd.set() == init );
    BOOST_CHECK( pvd_cr.set() == init );

    BOOST_REQUIRE( pvd.getTypeInfo() );
    BOOST_CHECK( pvd.getTypeInfo() != RTT::detail::DataSourceTypeInfo<RTT::detail::UnknownType>::getTypeInfo() );
    BOOST_CHECK( pvd.getTypeInfo() == pvd_cr.getTypeInfo() );

    // Compatible-type -assignment:
    BOOST_CHECK( pvd.getTypeInfo()->composeType( pvd.getDataSource(), pvd2.getDataSource() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( pvd_cr.getDataSource(), pvd.getDataSource() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( pvd.getDataSource(), pvd_cr.getDataSource() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( pvd_cr.getDataSource(), pvd_cr2.getDataSource() ) );

    Property<PropertyBag> bag("Result","Rd");
    // Decompose to property bag and back:
    BOOST_CHECK( pvd.getTypeInfo()->decomposeType( pvd.getDataSource(), bag.value() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd2.getDataSource() ) );
    BOOST_CHECK( pvd == pvd2 );
    pvd2.value().clear();
    deletePropertyBag( bag.value() );

    BOOST_CHECK( pvd.getTypeInfo()->decomposeType( pvd_cr.getDataSource(), bag.value() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd_cr2.getDataSource() ) );
    BOOST_CHECK( pvd_cr == pvd_cr2);
    pvd_cr2.value().clear();
    deletePropertyBag( bag.value() );

    // Cross composition. (const ref to value and vice versa)
    BOOST_CHECK( pvd.getTypeInfo()->decomposeType( pvd.getDataSource(), bag.value() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd_cr2.getDataSource() ) );
    BOOST_CHECK( pvd == pvd_cr2);
    pvd_cr2.value().clear();
    deletePropertyBag( bag.value() );

    BOOST_CHECK( pvd.getTypeInfo()->decomposeType( pvd_cr.getDataSource(), bag.value() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd2.getDataSource() ) );
    BOOST_CHECK( pvd_cr == pvd2);
    pvd2.value().clear();
    deletePropertyBag( bag.value() );
}


BOOST_AUTO_TEST_CASE( testInit )
{
    // See if this compiles fine:
    // detects collisions with other constructors...
    Property<unsigned int> pui("PUI","", 0 );
    Property<int> pi("PI","", 0 );
    Property<bool> pb("PB","", false );

    // Test null assignment
    PropertyBase* pbase = 0;
    Property<int> p2 = pbase;
    BOOST_CHECK( !p2.ready() );
    Property<int> p3;
    BOOST_CHECK( !p3.ready() );

    p3 = pbase;
    BOOST_CHECK( !p3.ready() );

    p2 = p3;
    BOOST_CHECK( !p2.ready() );

    p2 = pi;
    BOOST_CHECK( p2.ready() );

    BOOST_CHECK(true);
}


BOOST_AUTO_TEST_CASE( testUpdate )
{
    PropertyBag source;
    PropertyBag target;

    Property<PropertyBag> b1("b1","");
    Property<PropertyBag> b2("b2","");
    Property<int> p1("p1","",-1);

    Property<PropertyBag> b1c("b1","");
    Property<PropertyBag> b2c("b2","");
    Property<int> p1c("p1","",0);

    // setup source tree
    source.addProperty( b1 );
    b1.value().addProperty( b2 );
    b2.value().addProperty( p1 );

    // update case:
    // setup target tree
    target.addProperty( b1c );
    b1c.value().addProperty( b2c );
    b2c.value().addProperty( p1c );

    BOOST_CHECK( p1.get() != p1c.get() );

    BOOST_CHECK( updateProperty(target, source, "b1/b2/p1", "/") );

    BOOST_CHECK( p1.get() == -1 );
    BOOST_CHECK( p1c.get() == -1 );

    // creation case:
    target.removeProperty(&b1);
    BOOST_CHECK( updateProperty(target, source, "b1/b2/p1", "/") );

    Property<PropertyBag>* bag = target.getPropertyType<PropertyBag>("b1");
    BOOST_CHECK( bag );
    BOOST_CHECK( bag->getName() == "b1" );
    bag = bag->get().getPropertyType<PropertyBag>("b2");
    BOOST_CHECK( bag );
    BOOST_CHECK( bag->getName() == "b2" );

    Property<int>* res = bag->get().getPropertyType<int>("p1");
    BOOST_CHECK( res );
    BOOST_CHECK( res->getName() == "p1" );
    BOOST_CHECK( res->get() == -1 );

}


// This test does not yet test all types !
BOOST_AUTO_TEST_CASE( testPropMarsh )
{
    std::string filename = ".property_test.cpf";

    PropertyBag source; // to file
    PropertyBag target; // from file

    Property<PropertyBag> b1("b1","b1d");
    Property<PropertyBag> b2("b2","b2d");
    Property<int> p1("p1","p1d",-1);

    // setup source tree
    source.addProperty( b1 );
    b1.value().addProperty( b2 );
    b2.value().addProperty( p1 );

    {
        // scope required such that file is closed
        PropertyMarshaller pm( filename );
        pm.serialize( source );
    }

    {
        // scope required such that file is closed
        PropertyDemarshaller pd( filename );
        BOOST_REQUIRE( pd.deserialize( target ) );
    }

    Property<PropertyBag> bag = target.getProperty("b1");
    BOOST_REQUIRE( bag.ready() );
    BOOST_CHECK( bag.getDescription() == "b1d" );

    bag = bag.rvalue().getProperty("b2");
    BOOST_REQUIRE( bag.ready() );
    BOOST_CHECK( bag.getDescription() == "b2d" );

    Property<int> pi = bag.rvalue().getProperty("p1");
    BOOST_REQUIRE( pi.ready() );
    BOOST_CHECK( pi.get() == -1 );
    BOOST_CHECK( pi.getDescription() == "p1d" );
    deletePropertyBag( target );
}

BOOST_AUTO_TEST_CASE( testPropMarshVect )
{
    std::string filename = ".property_test_vect.cpf";

    PropertyBag source; // to file
    PropertyBag target; // from file

    Property<std::vector<double> >* p1 =  new Property<std::vector<double> >("p1","p1d", std::vector<double>(7, 1.234) );

    // setup source tree
    source.addProperty( *p1 );

    {
        // scope required such that file is closed
        PropertyMarshaller pm( filename );
        pm.serialize( source );
    }

    p1->set() = std::vector<double>(3, 0.234);
    {
        // scope required such that file is closed
        PropertyDemarshaller pd( filename );
        BOOST_REQUIRE( pd.deserialize( target ) );
    }

    // check bag:
    Property<PropertyBag> bag = target.getProperty("p1");
    BOOST_REQUIRE( bag.ready() );
    BOOST_CHECK( bag.getDescription() == "p1d" );
    BOOST_CHECK( bag.rvalue().size() == 7 );

    // update bag -> array.
    BOOST_CHECK( updateProperties( source, target) );

    //p1 = source.getProperty("p1");
    BOOST_REQUIRE( p1->ready() );
    BOOST_CHECK( p1->rvalue().size() == 7 );
    BOOST_CHECK( p1->rvalue()[0] == 1.234 );

    // Test legacy:
    deletePropertyBag( target );
    p1->setName("driveLimits");
    {
        // scope required such that file is closed
        PropertyDemarshaller pd( "property_test_vect.cpf" );
        BOOST_REQUIRE( pd.deserialize( target ) );
    }
    bag = target.getProperty("driveLimits");
    BOOST_REQUIRE( bag.ready() );
    BOOST_CHECK( bag.rvalue().size() == 7 );

    // update bag -> array.
    BOOST_CHECK( updateProperties( source, target) );

    //p1 = source.getProperty("p1");
    BOOST_REQUIRE( p1->ready() );
    //cout << p1 << endl;
    BOOST_CHECK( p1->rvalue().size() == 6 );
    BOOST_CHECK( p1->rvalue()[0] == 1 );

    deletePropertyBag( target );
    deletePropertyBag( source );
}

BOOST_AUTO_TEST_SUITE_END()

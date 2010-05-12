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

#include <typeinfo>
#include <marsh/PropertyBagIntrospector.hpp>
#include <internal/DataSourceTypeInfo.hpp>
#include <types/PropertyDecomposition.hpp>
#include <Property.hpp>
#include <PropertyBag.hpp>

#include "unit.hpp"

class PropertyTest
{
public:
    PropertyBag bag1;
    PropertyBag bag2;
    PropertyBase* pb;
    Property<int>* pi1;
    Property<int>* pi2;
    Property<int>* pi1ref;
    Property<int>* pi2ref;
    int intref;

    PropertyBag bag;
    Property<float> pf;
    Property<double> pd;
    Property<std::string> ps;
    Property<char> pc;

    Property<PropertyBag> subbag1;
    Property<PropertyBag> subbag2;

    PropertyTest()
        : pf("pf","pfd", -1.0),pd("pd","pdd", +1.0),
          ps("ps","psd", "std::string"),
          pc("pc","pcd", 'c'),
          subbag1("s1", "s1d"),subbag2("s2", "s2d")
    {
        intref = 99;
        pi1 = new Property<int>("pi1","pi1d", 0 );
        pi2 = new Property<int>("pi2","pi2d", 0 );
        pi1ref =  dynamic_cast< Property<int>* >( pi1->clone() );
        pi2ref =  dynamic_cast< Property<int>* >( pi2->clone() );

        bag.add( pi1 );
        bag.add( pi2 );
        bag.add( &subbag1 );
        subbag1.set().add( &subbag2 );
        subbag1.set().add( &pf );
        subbag1.set().add( &pd );

        subbag2.set().add( &ps );
        subbag2.set().add( &pc );

    }
    ~PropertyTest()
    {
        delete pi1;
        delete pi2;
        delete pi1ref;
        delete pi2ref;
    }
};

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


BOOST_AUTO_TEST_CASE( testfindProperty )
{
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

// listProperties( bag, separator )
BOOST_AUTO_TEST_CASE( testlistProperties )
{
    vector<string> result = listProperties( bag );
    BOOST_CHECK_EQUAL( result.size(), 8);
    // this test assumes this order, which is actually not
    // guaranteed by PropertyBag:
    BOOST_CHECK_EQUAL( result[0], string("pi1"));
    BOOST_CHECK_EQUAL( result[1], string("pi2"));
    BOOST_CHECK_EQUAL( result[2], string("s1"));
    BOOST_CHECK_EQUAL( result[3], string("s1.s2"));
    BOOST_CHECK_EQUAL( result[4], string("s1.s2.ps"));
    BOOST_CHECK_EQUAL( result[5], string("s1.s2.pc"));
    BOOST_CHECK_EQUAL( result[6], string("s1.pf"));
    BOOST_CHECK_EQUAL( result[7], string("s1.pd"));
}

// storeProperty( bag, path, item, separator )
BOOST_AUTO_TEST_CASE( teststoreProperty )
{
    Property<int>* int1 = new Property<int>("int1","",3);
    Property<int>* int2 = new Property<int>("int2","",6);
    Property<int>* int3 = new Property<int>("int3","",9);
    BOOST_CHECK( storeProperty(bag, "pp1", int1 ));
    BOOST_CHECK_EQUAL( findProperty(bag, "pp1.int1"), int1 );

    BOOST_CHECK( storeProperty(bag, "pp1.pp2", int2) );
    BOOST_CHECK_EQUAL( findProperty(bag, "pp1.pp2.int2"), int2 );

    BOOST_CHECK( removeProperty( bag, "pp1.pp2.int2") );
    int2 = new Property<int>("int2","",6);

    // re-add int2, but with different separator, and lots of them
    BOOST_CHECK( storeProperty(bag, "##pp1###pp3##", int2, "#") );
    BOOST_CHECK_EQUAL( findProperty(bag, "pp1#pp3#int2", "#"), int2 );

    // top level store is equivalent to ownProperty:
    BOOST_CHECK( storeProperty(bag, "", int3) );
    BOOST_CHECK( findProperty(bag, "int3") );
    BOOST_CHECK_EQUAL( bag.find("int3"), int3 );
    BOOST_CHECK( bag.ownsProperty( int3 ) );

    bag.removeProperty( int3 );
    int3 = new Property<int>("int3","",9);

    // same but with separator as path:
    BOOST_CHECK( storeProperty(bag, "#", int3,"#") );
    BOOST_CHECK( findProperty(bag, "int3") );
    BOOST_CHECK_EQUAL( bag.find("int3"), int3 );
    BOOST_CHECK( bag.ownsProperty( int3 ) );
}

// removeProperty( bag, path, item, separator )
BOOST_AUTO_TEST_CASE( testremoveProperty )
{
    // check for wrong input:
    BOOST_CHECK( removeProperty( bag, "qwerty" ) == false );
    BOOST_CHECK( removeProperty( bag, "." ) == false );

    // remove top level prop:
    BOOST_CHECK( removeProperty( bag, "pi1" ) );
    BOOST_CHECK( findProperty( bag, "pi1" ) == 0 );

    // remove a leaf prop:
    BOOST_CHECK( removeProperty( bag, "s1.s2.pc" ) );
    BOOST_CHECK( findProperty( bag, "s1.s2.pc" ) == 0 );

    // remove a bag:
    BOOST_CHECK( removeProperty( bag, "s1.s2" ) );
    BOOST_CHECK( findProperty( bag, "s1.s2" ) == 0 );
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
        if ( target && typeDecomposition( target->getDataSource(), bag.value() ) )
            BOOST_CHECK_MESSAGE( target->getTypeInfo()->composeType( bag.getDataSource() , target->getDataSource() ), "Failed composition for type "+target->getTypeInfo()->getTypeName() );
        deletePropertyBag( bag.value() );
        delete target;
    }

}

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
    BOOST_CHECK( typeDecomposition( pvd.getDataSource(), bag.value() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd2.getDataSource() ) );
    BOOST_CHECK( pvd == pvd2 );
    pvd2.value().clear();
    deletePropertyBag( bag.value() );

    BOOST_CHECK( typeDecomposition( pvd_cr.getDataSource(), bag.value() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd_cr2.getDataSource() ) );
    BOOST_CHECK( pvd_cr == pvd_cr2);
    pvd_cr2.value().clear();
    deletePropertyBag( bag.value() );

    // Cross composition. (const ref to value and vice versa)
    BOOST_CHECK( typeDecomposition( pvd.getDataSource(), bag.value() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd_cr2.getDataSource() ) );
    BOOST_CHECK( pvd == pvd_cr2);
    pvd_cr2.value().clear();
    deletePropertyBag( bag.value() );

    BOOST_CHECK( typeDecomposition( pvd_cr.getDataSource(), bag.value() ) );
    BOOST_CHECK( pvd.getTypeInfo()->composeType( bag.getDataSource(), pvd2.getDataSource() ) );
    BOOST_CHECK( pvd_cr == pvd2);
    pvd2.value().clear();
    deletePropertyBag( bag.value() );
}

//! Tests v2 property decomposition using parts API.
BOOST_AUTO_TEST_CASE( testNewDecomposition )
{
    /**
     * test vector
     */
    std::vector<double> init(33, 1.0);
    // these are the original sources:
    Property<std::vector<double> > pvd("pvd","pvd desc", init);
    Property<const std::vector<double>& > pvd_cr("pvd_cr","pvd_cr desc", init);

    //std::cout << "\n\n\n "<< std::string( typeid(init).name() ) << "\n\n\n "<<std::endl;

    // these are targets to compare the source with:
    Property<std::vector<double> > pvd2("pvd 2","pvd desc 2");
    Property<const std::vector<double>& > pvd_cr2("pvd_cr 2","pvd desc 2");

    BOOST_CHECK( pvd.get() == init );
    BOOST_CHECK( pvd_cr.get() == init );
    BOOST_CHECK( pvd.set() == init );
    BOOST_CHECK( pvd_cr.set() == init );

    BOOST_REQUIRE( pvd.getTypeInfo() );
    BOOST_CHECK( pvd.getTypeInfo() != RTT::detail::DataSourceTypeInfo<RTT::detail::UnknownType>::getTypeInfo() );
    BOOST_CHECK( pvd.getTypeInfo() == pvd_cr.getTypeInfo() );

    Property<PropertyBag> bag("Result","Rd");
    // Decompose to property bag and check refs:
    BOOST_CHECK( propertyDecomposition( &pvd, bag.value() ) );

    Property<double> pvalue = bag.value().getItem(3);
    BOOST_REQUIRE( pvalue.ready() );
    pvalue.set( 42 );

    BOOST_CHECK( pvd.rvalue()[3] == 42 );

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

BOOST_AUTO_TEST_SUITE_END()

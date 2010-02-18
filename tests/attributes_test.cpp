/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.cpp

                        generictask_test_3.cpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include <iostream>
#include <InputPort.hpp>
#include <OutputPort.hpp>

#include <TaskContext.hpp>
#include <extras/SlaveActivity.hpp>
#include <extras/SequentialActivity.hpp>
#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>

#include <boost/function_types/function_type.hpp>
#include <Method.hpp>

using namespace std;
using namespace RTT;
using namespace RTT::detail;


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


/**
 * Fixture.
 */
class AttributesFixture
{
public:
    TaskContext* tc;
    ActivityInterface* tsim;
public:
    AttributesFixture()
    {
        tc =  new TaskContext( "root", TaskContext::Stopped );
        tc->setActivity( new SimulationActivity(0.001) );
        tsim = tc->getActivity();
        SimulationThread::Instance()->stop();
    }

    ~AttributesFixture()
    {
        tsim->stop();
        delete tc;
    }
};


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  AttributesSuite,  AttributesFixture )


BOOST_AUTO_TEST_CASE( testProperties)
{
    Property<double> d1("d1", "desc1", 1.234);
    tc->properties()->addProperty( &d1);

    BOOST_CHECK_EQUAL( double(1.234), d1.get() );
    BOOST_CHECK_EQUAL( double(1.234), tc->properties()->getProperty<double>("d1")->get() );

    // test setup of mirror:
    Property<string> s1;
    BOOST_CHECK( !s1.ready() );
    Property<string> s2("hello","description", "world");
    BOOST_CHECK( s2.ready() );

    BOOST_CHECK(tc->properties()->addProperty( &s1 ) == false);
    BOOST_CHECK(tc->properties()->addProperty( &s2 ) );
    s1 = tc->properties()->getProperty<string>("hello");
    BOOST_CHECK( s1.ready() );

    BOOST_CHECK_EQUAL(std::string("hello"), s1.getName() );
    BOOST_CHECK_EQUAL(std::string("description"), s1.getDescription() );
    BOOST_CHECK_EQUAL(std::string("world"), s1.get() );

    // Test mirroring of 'set' and 'get':
    s1.set("universe");
    BOOST_CHECK_EQUAL(std::string("universe"), s2.get() );

#if 0
    BOOST_CHECK(tc->writeProperties("Generic_TaskTest_3_Properties.cpf"));
    BOOST_CHECK( tc->readProperties("Generic_TaskTest_3_Properties.cpf"));
#endif
}

BOOST_AUTO_TEST_CASE( testAttributes)
{
    // test attribute repository:
    Attribute<int> i1("i1");
    Attribute<double> d1("d1", 1.234);
    BOOST_CHECK( i1.ready() );
    BOOST_CHECK( d1.ready() );
    BOOST_CHECK(tc->addAttribute( &d1 ));
    BOOST_CHECK(tc->addAttribute( &i1 ));

    i1.set( 3 );
    BOOST_CHECK_EQUAL( double(1.234), d1.get() );
    BOOST_CHECK_EQUAL( int(3), i1.get() );

    BOOST_CHECK_EQUAL( double(1.234), tc->getAttribute<double>("d1")->get() );
    BOOST_CHECK_EQUAL( int(3),        tc->getAttribute<int>("i1")->get() );

    // test setup of mirror:
    Attribute<string> s1;
    BOOST_CHECK( !s1.ready() );
    Attribute<string> s2("hello","world");
    BOOST_CHECK( s2.ready() );

    BOOST_CHECK(tc->addAttribute( &s1 ) == false);
    BOOST_CHECK(tc->addAttribute( &s2 ) );
    s1 = tc->getAttribute<string>("hello");
    BOOST_CHECK( s1.ready() );

    BOOST_CHECK_EQUAL(std::string("hello"), s1.getName() );
    BOOST_CHECK_EQUAL(std::string("world"), s1.get() );

    // Test mirroring of 'set' and 'get':
    s1.set("universe");
    BOOST_CHECK_EQUAL(std::string("universe"), s2.get() );

}
BOOST_AUTO_TEST_SUITE_END()


#include "unit.hpp"

#include <boost/serialization/vector.hpp>
#include <boost/array.hpp>

#include <rtt-fwd.hpp>
#include <internal/DataSources.hpp>
#include <types/type_discovery.hpp>
#include <os/fosi.h>
#include <boost/lambda/lambda.hpp>

#include "datasource_fixture.hpp"
#include "types/StructTypeInfo.hpp"
#include "types/CArrayTypeInfo.hpp"
#include "types/SequenceTypeInfo.hpp"
#include "types/BoostArrayTypeInfo.hpp"

using namespace boost::lambda;
using namespace boost::archive;
using namespace boost::serialization;

class SequenceTypeTest
{
public:
    SequenceTypeTest() {  }
    ~SequenceTypeTest() {  }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  SequenceTypeInfoTestSuite,  SequenceTypeTest )

// Test the CArrayTypeInfo for ints
BOOST_AUTO_TEST_CASE( testCTypeArray )
{
    Types()->addType( new CArrayTypeInfo< carray<int> >("cints") );
    int tester[3] = { 3, 2, 1 };

    AssignableDataSource< carray<int> >::shared_ptr atype = new ValueDataSource< carray<int> >( carray<int>(tester, 3) );

    BOOST_REQUIRE( Types()->type("cints") );

    // check the part names lookup:
    vector<string> names = atype->getMemberNames();
    BOOST_CHECK_EQUAL( atype->getMemberNames().size(), 2 ); // capacity,size

//    for_each( names.begin(), names.end(), cout << lambda::_1 <<", " );
//    cout <<endl;

    BOOST_REQUIRE_EQUAL( names.size(), 2);
    BOOST_REQUIRE( atype->getMember("0") );

    // Check individual part lookup by index:
    AssignableDataSource<int>::shared_ptr a0 = AssignableDataSource<int>::narrow( atype->getMember("0").get() );
    AssignableDataSource<int>::shared_ptr a1 = AssignableDataSource<int>::narrow( atype->getMember("1").get() );
    AssignableDataSource<int>::shared_ptr a2 = AssignableDataSource<int>::narrow( atype->getMember("2").get() );

    BOOST_REQUIRE( a0 );
    BOOST_REQUIRE( a1 );
    BOOST_REQUIRE( a2 );

    BOOST_CHECK( !atype->getMember("zort") );

    // Check reading parts (must equal parent)
    BOOST_CHECK_EQUAL( a0->get(), tester[0] );
    BOOST_CHECK_EQUAL( a1->get(), tester[1] );
    BOOST_CHECK_EQUAL( a2->get(), tester[2] );

    // Check writing a part (must change in parent too).
    a0->set(30);
    a1->set(20);
    a2->set(10);
    BOOST_CHECK_EQUAL( a0->get(), tester[0] );
    BOOST_CHECK_EQUAL( a1->get(), tester[1] );
    BOOST_CHECK_EQUAL( a2->get(), tester[2] );
}

// Test the SequenceTypeInfo for ints
BOOST_AUTO_TEST_CASE( testContainerType )
{
    Types()->addType( new SequenceTypeInfo< std::vector<int> >("ints") );
    vector<int> tester;
    tester.push_back( 3 );
    tester.push_back( 2 );
    tester.push_back( 1 );

    AssignableDataSource< vector<int> >::shared_ptr atype = new ReferenceDataSource< vector<int> >( tester );

    BOOST_REQUIRE( Types()->type("ints") == atype->getTypeInfo() );

    // check the part names lookup:
    vector<string> names = atype->getMemberNames();
    BOOST_CHECK_EQUAL( atype->getMemberNames().size(), 2 ); // capacity,size

//    for_each( names.begin(), names.end(), cout << lambda::_1 <<", " );
//    cout <<endl;

    BOOST_REQUIRE_EQUAL( names.size(), 2);
    BOOST_REQUIRE( atype->getMember("0") );

    // Check individual part lookup by index:
    AssignableDataSource<int>::shared_ptr a0 = dynamic_pointer_cast< AssignableDataSource<int> >( atype->getMember("0") );
    AssignableDataSource<int>::shared_ptr a1 = dynamic_pointer_cast< AssignableDataSource<int> >( atype->getMember("1") );
    AssignableDataSource<int>::shared_ptr a2 = dynamic_pointer_cast< AssignableDataSource<int> >( atype->getMember("2") );
    DataSource<int>::shared_ptr siz = dynamic_pointer_cast< DataSource<int> >( atype->getMember("size") );
    DataSource<int>::shared_ptr cap = dynamic_pointer_cast< DataSource<int> >( atype->getMember("capacity") );

    BOOST_REQUIRE( a0 );
    BOOST_REQUIRE( a1 );
    BOOST_REQUIRE( a2 );
    BOOST_REQUIRE( siz );
    BOOST_REQUIRE( cap );

    BOOST_CHECK( !atype->getMember("zort") );

    // Check reading parts (must equal parent)
    BOOST_CHECK_EQUAL( a0->get(), tester[0] );
    BOOST_CHECK_EQUAL( a1->get(), tester[1] );
    BOOST_CHECK_EQUAL( a2->get(), tester[2] );

    // Check modifying size/capacity.
    tester.reserve(33);
    BOOST_CHECK_EQUAL( cap->get(), tester.capacity() );

    tester.push_back(4);
    tester.push_back(5);
    tester.push_back(6);
    BOOST_CHECK_EQUAL( siz->get(), tester.size() );


    // Check writing a part (must change in parent too).
    a0->set(30);
    a1->set(20);
    a2->set(10);
    BOOST_CHECK_EQUAL( a0->get(), tester[0] );
    BOOST_CHECK_EQUAL( a1->get(), tester[1] );
    BOOST_CHECK_EQUAL( a2->get(), tester[2] );
}

// Test the SequenceTypeInfo for chars (std::string)
BOOST_AUTO_TEST_CASE( testStringContainerType )
{
    Types()->addType( new SequenceTypeInfo< std::string >("chars") );
    string tester = "tester";

    AssignableDataSource< string >::shared_ptr atype = new ReferenceDataSource< string >( tester );

    BOOST_REQUIRE( Types()->type("chars") == atype->getTypeInfo() );

    // check the part names lookup:
    vector<string> names = atype->getMemberNames();
    BOOST_CHECK_EQUAL( atype->getMemberNames().size(), 2 ); // capacity,size

//    for_each( names.begin(), names.end(), cout << lambda::_1 <<", " );
//    cout <<endl;

    BOOST_REQUIRE_EQUAL( names.size(), 2);
    BOOST_REQUIRE( atype->getMember("0") );

    // test use of 'getAssignable()' to narrow:
    DataSourceBase::shared_ptr ds0 = atype->getMember("0");
    BOOST_REQUIRE( ds0 );
    AssignableDataSource<char>::shared_ptr a0 = AssignableDataSource<char>::narrow( ds0->getTypeInfo()->getAssignable( ds0 ).get() );
    BOOST_REQUIRE( a0 );

    // Check individual part lookup by index:
    AssignableDataSource<char>::shared_ptr a1 = dynamic_pointer_cast< AssignableDataSource<char> >( atype->getMember("1") );
    AssignableDataSource<char>::shared_ptr a2 = dynamic_pointer_cast< AssignableDataSource<char> >( atype->getMember("2") );
    DataSource<int>::shared_ptr siz = dynamic_pointer_cast< DataSource<int> >( atype->getMember("size") );
    DataSource<int>::shared_ptr cap = dynamic_pointer_cast< DataSource<int> >( atype->getMember("capacity") );

    BOOST_REQUIRE( a1 );
    BOOST_REQUIRE( a2 );
    BOOST_REQUIRE( siz );
    BOOST_REQUIRE( cap );

    BOOST_CHECK( !atype->getMember("zort") );

    // Check reading parts (must equal parent)
    BOOST_CHECK_EQUAL( a0->get(), tester[0] );
    BOOST_CHECK_EQUAL( a1->get(), tester[1] );
    BOOST_CHECK_EQUAL( a2->get(), tester[2] );

    // Check modifying size/capacity.
    tester.reserve(33);
    BOOST_CHECK_EQUAL( cap->get(), tester.capacity() );

    tester.push_back(4);
    tester.push_back(5);
    tester.push_back(6);
    BOOST_CHECK_EQUAL( siz->get(), tester.size() );


    // Check writing a part (must change in parent too).
    a0->set(30);
    a1->set(20);
    a2->set(10);
    BOOST_CHECK_EQUAL( a0->get(), tester[0] );
    BOOST_CHECK_EQUAL( a1->get(), tester[1] );
    BOOST_CHECK_EQUAL( a2->get(), tester[2] );
}

BOOST_AUTO_TEST_SUITE_END()


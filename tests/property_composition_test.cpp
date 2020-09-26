#include <typeinfo>
#include "rtt/internal/DataSourceTypeInfo.hpp"
#include "rtt/types/PropertyComposition.hpp"
#include "rtt/Property.hpp"
#include "rtt/PropertyBag.hpp"
#include "rtt/Logger.hpp"

#include "tests/unit.hpp"

class PropertyCompositionTest
{
public:
    PropertyBag source;
    PropertyBag target;
    PropertyBag result;
    Property<vector<double> > pv;

    PropertyCompositionTest()
    :pv("pv","pvd", vector<double>(10,5.0))
    {
    }
    ~PropertyCompositionTest()
    {
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

namespace std {
std::ostream& operator<<(std::ostream& os, const std::vector<double>& vect) {
    os << '[';
    for(unsigned int i= 0; i != vect.size(); ++i)
        os << vect[i] << (i+1 == vect.size() ? "]" : ", ");
    return os;
}
}
namespace RTT {
    // todo
bool operator==(const PropertyBag& a, const PropertyBag& b) {
    if( a.size() == b.size() ) {
        PropertyBag::const_iterator ita = a.begin();
//        PropertyBag::const_iterator itb = b.begin();
        while ( ita != a.end() ) {
            //if ( (*ita)->equals(*itb) )
                return false;
        }
    }
    return false;
}
}


BOOST_FIXTURE_TEST_SUITE( PropertyTestSuite, PropertyCompositionTest )

BOOST_AUTO_TEST_CASE( testDecomposeComposeEmptyBag )
{
    BOOST_CHECK( decomposePropertyBag(source, target) );
    BOOST_CHECK_EQUAL( target.size(), 0);
    BOOST_CHECK( composePropertyBag(target, result) );
    BOOST_CHECK_EQUAL( result.size(), 0);
}

BOOST_AUTO_TEST_CASE( testDecomposeComposeWrongTargetBag )
{
    target.addProperty(pv);
    BOOST_CHECK( decomposePropertyBag(source, target) == false );
    BOOST_CHECK_EQUAL( target.size(), 1);
    result.addProperty(pv);
    BOOST_CHECK( composePropertyBag(target, result) == false );
    BOOST_CHECK_EQUAL( result.size(), 1);
}

BOOST_AUTO_TEST_CASE( testDecomposeComposeVector )
{
    source.addProperty(pv);

    BOOST_CHECK( decomposePropertyBag(source, target) );
    BOOST_CHECK_EQUAL( target.size(), 1);

    Property<PropertyBag> decomposed = target.getPropertyType<PropertyBag>("pv");
    BOOST_REQUIRE( decomposed.ready() );
    BOOST_CHECK_EQUAL( decomposed.value().getType(), "array" );

    // check that target bag has equal number of decomposed elements:
    BOOST_CHECK_EQUAL( decomposed.getDescription(), pv.getDescription() );
    BOOST_CHECK_EQUAL( decomposed.value().size(), pv.value().size() );

    BOOST_CHECK( composePropertyBag(target, result) );

    Property<vector<double> > pr;
    pr = result.getProperty("pv");
    BOOST_REQUIRE( pr.ready() );
    BOOST_CHECK_EQUAL( pr.value().size(), pv.value().size() );
    BOOST_CHECK_EQUAL( pr.value(), pv.value() );
}

BOOST_AUTO_TEST_SUITE_END()

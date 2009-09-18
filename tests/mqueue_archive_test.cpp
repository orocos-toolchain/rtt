#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <transports/serialization/binary_object_archive.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;
using namespace boost::archive;
namespace io = boost::iostreams;

class MQueueArchiveTest
{
public:
    MQueueArchiveTest() { this->setUp(); }
    ~MQueueArchiveTest() { this->tearDown(); }

    void setUp();
    void tearDown();
};



void
MQueueArchiveTest::setUp()
{
}


void
MQueueArchiveTest::tearDown()
{
}

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MQueueArchiveTestSuite,  MQueueArchiveTest )

BOOST_AUTO_TEST_CASE( testBinaryObjectArchive )
{
    char sink[1000];
    memset( sink, 0, 1000);
    double d = 3.0;
    vector<double> c(10, 9.99);

    io::stream<io::array_sink>  outbuf(sink,1000);
    binary_object_oarchive out( outbuf ); // +1 alloc
    out << d; // +0 alloc
    out << c; // +2 alloc

    d = 0.0;
    c.resize(10,0.0);

    io::stream<io::array_source>  inbuf(sink,1000);
    binary_object_iarchive in( inbuf ); // +1 alloc
    in >> d; // +0 alloc
    in >> c; // +2 alloc

    BOOST_CHECK_CLOSE( d, 3.0, 0.01);
    BOOST_CHECK_EQUAL( c.size(), 10);
    for(int i=0; i != 10; ++i)
        BOOST_CHECK_CLOSE( c[i], 9.99, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()


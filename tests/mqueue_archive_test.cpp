#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <rtt-fwd.hpp>
#include <marsh/binary_data_archive.hpp>

using namespace std;
using namespace boost::archive;
using namespace RTT::detail;
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

using namespace boost::serialization;

// Test writing a data sample + a vector into a binary data archive
// this must be a complete real-time operation. Use valgrind to check
// allocs.
BOOST_AUTO_TEST_CASE( testBinaryDataArchive )
{
    char sink[1000];
    memset( sink, 0, 1000);
    double d = 3.0;
    vector<double> c(10, 9.99);

    io::stream<io::array_sink>  outbuf(sink,1000);
    binary_data_oarchive out( outbuf ); // +0 alloc
    out << d; // +0 alloc
    out << c; // +0 alloc

    int stored = out.getArchiveSize();
    BOOST_CHECK( stored > 10*sizeof(double) );
    d = 0.0;
    c.clear();
    c.resize(20,0.0);

    io::stream<io::array_source>  inbuf(sink,1000);
    binary_data_iarchive in( inbuf ); // +0 alloc
    in >> d; // +0 alloc
    in >> c; // +0 alloc

#if 0
    // this code was for testing the compile errors I got on BOOST_SERIALIZATION_NVP(count) use in the serialization library.
    int count;
    nvp<int> n = BOOST_SERIALIZATION_NVP(count);
    in >> n;

    stringstream ss;
    binary_iarchive bin( ss );

    bin >> BOOST_SERIALIZATION_NVP(count);

    in >> BOOST_SERIALIZATION_NVP(count);
#endif

    BOOST_CHECK_CLOSE( d, 3.0, 0.01);
    BOOST_CHECK_EQUAL( c.size(), 10);
    for(int i=0; i != 10; ++i) {
        BOOST_CHECK_CLOSE( c[i], 9.99, 0.01);
    }
    BOOST_CHECK_EQUAL( stored, in.getArchiveSize() );
}

BOOST_AUTO_TEST_SUITE_END()


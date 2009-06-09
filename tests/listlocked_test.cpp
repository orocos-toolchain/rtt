
#include <ListLocked.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/bind/protect.hpp>

using namespace std;
using namespace RTT;
using namespace boost;

void foo(double d)
{
}


struct ListTest
{
    struct Cont :  public boost::intrusive::list_base_hook<> {
        double data;
    };
    typedef boost::intrusive::list<Cont> BufferType;

    BufferType mlist;

    void mfoo(double d)
    {}

    double mcfoo(Cont& c)
    {
        return c.data;
    }

    template<class Function>
    static void apply_item(Function func, Cont& c)
    {
        //func( c->data );
    }

    template<class Function>
    void apply( Function func )
    {
        //std::for_each (mlist.begin(), mlist.end(), boost::bind( &ListTest::apply_item<Function>, boost::protect(func), _1) );
        for (BufferType::iterator it = mlist.begin(); it != mlist.end(); ++it)
            func( it->data );
    }
};

void cfoo(ListTest::Cont& c)
{
}

bool iffoo(double d)
{
    return true;
}


BOOST_FIXTURE_TEST_SUITE( ListTestSuite, ListTest )

BOOST_AUTO_TEST_CASE( test_instatiation )
{

    ListLocked<double> lld(10);

    typedef shared_ptr<double*> dsp;

    ListLocked<dsp> lldsp(10);
}

BOOST_AUTO_TEST_CASE( test_apply )
{

    std::for_each (mlist.begin(), mlist.end(), boost::bind( &ListTestSuite::test_apply::mcfoo, this, _1) );

    //std::for_each (mlist.begin(), mlist.end(), boost::bind( &ListTest::apply_item, this, boost::protect(boost::bind(foo, _1)), _1) );
    //std::for_each (mlist.begin(), mlist.end(), boost::bind( boost::mem_fn(&ListTestSuite::test_apply::apply_item<void(double)>), this, boost::bind(foo, _1), _1) );

    Cont c;
    //boost::bind( &ListTestSuite::test_apply::apply_item, this, boost::protect( boost::bind(foo,_1)), _1)(c);

    apply( boost::bind(&foo,_1) );

#if 1
    ListLocked<double> lld(10);

    typedef shared_ptr<double*> dsp;

    ListLocked<dsp> lldsp(10);

    lld.apply( boost::bind( foo, _1 ) );
#endif

    lld.clear();

    lldsp.clear();

    lld.find_if(&iffoo);
}

BOOST_AUTO_TEST_SUITE_END()



#include <iostream>
#include <TaskContext.hpp>
#include <Method.hpp>
#include <Operation.hpp>
#include <internal/RemoteMethod.hpp>
#include <interface/ServiceProvider.hpp>
#include <internal/DataSourceGenerator.hpp>

#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

class FunctorsTest {
public:
    FunctorsTest()
    {
        tc =  new TaskContext( "root" );
        tc->provides()->addService( this->createMethodFactory() );
        caller = new TaskContext("caller");
        caller->start();
        tc->start();
    }

    TaskContext* tc, *caller;

    // ref/const-ref tests:
    double ret;
    double& m0r() { return ret; }
    const double& m0cr() { return ret; }

    // test const std::string& argument for command_ds
    bool comstr(const std::string& cs) { return !cs.empty(); }

    double m1r(double& a) { a = 2*a; return a; }
    double m1cr(const double& a) { return a; }

    // plain argument tests:
    double m0() { return -1.0; }
    double m1(int i) { if (i ==1) return -2.0; else return 2.0; }
    double m2(int i, double d) { if ( i == 1 && d == 2.0 ) return -3.0; else return 3.0; }
    double m3(int i, double d, bool c) { if ( i == 1 && d == 2.0 && c == true) return -4.0; else return 4.0; }
    double m4(int i, double d, bool c, std::string s) { if ( i == 1 && d == 2.0 && c == true && s == "hello") return -5.0; else return 5.0;  }

    bool assertBool(bool b) { return b; }

    ~FunctorsTest()
    {
        //     if ( tc->getPeer("programs") )
        //         delete tc->getPeer("programs");
        tc->stop();
        SimulationThread::Instance()->stop();
        delete tc;
        delete  caller;
    }

    ServiceProvider::shared_ptr createMethodFactory()
        {
        ServiceProvider::shared_ptr to = ServiceProvider::Create("methods");

        to->addOperation("assert", &FunctorsTest::assertBool, this).doc("assert").arg("b", "bd");

        // ClientThread
        to->addOperation("m0r", &FunctorsTest::m0r, this).doc("M0r");
        to->addOperation("m0cr", &FunctorsTest::m0cr, this).doc("M0cr");
        to->addOperation("m1r", &FunctorsTest::m1r, this).doc("M1r");
        to->addOperation("m1cr", &FunctorsTest::m1cr, this).doc("M1cr");

        to->addOperation("m0", &FunctorsTest::m0, this).doc("M0");
        to->addOperation("m1", &FunctorsTest::m1, this).doc("M1").arg("a", "ad");
        to->addOperation("m2", &FunctorsTest::m2, this).doc("M2").arg("a", "ad").arg("a", "ad");
        to->addOperation("m3", &FunctorsTest::m3, this).doc("M3").arg("a", "ad").arg("a", "ad").arg("a", "ad");
        to->addOperation("m4", &FunctorsTest::m4, this).doc("M4").arg("a", "ad").arg("a", "ad").arg("a", "ad").arg("a", "ad");

        // OwnThread
        to->addOperation("o0r", &FunctorsTest::m0r, this,OwnThread).doc("M0r");
        to->addOperation("o0cr", &FunctorsTest::m0cr, this,OwnThread).doc("M0cr");
        to->addOperation("o1r", &FunctorsTest::m1r, this,OwnThread).doc("M1r");
        to->addOperation("o1cr", &FunctorsTest::m1cr, this,OwnThread).doc("M1cr");

        to->addOperation("o0", &FunctorsTest::m0, this,OwnThread).doc("M0");
        to->addOperation("o1", &FunctorsTest::m1, this,OwnThread).doc("M1").arg("a", "ad");
        to->addOperation("o2", &FunctorsTest::m2, this,OwnThread).doc("M2").arg("a", "ad").arg("a", "ad");
        to->addOperation("o3", &FunctorsTest::m3, this,OwnThread).doc("M3").arg("a", "ad").arg("a", "ad").arg("a", "ad");
        to->addOperation("o4", &FunctorsTest::m4, this,OwnThread).doc("M4").arg("a", "ad").arg("a", "ad").arg("a", "ad").arg("a", "ad");
        return to;
        }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  FunctorsTestSuite,  FunctorsTest )

BOOST_AUTO_TEST_CASE(testClientThreadFunctor)
{
    // Test the produce() method, which maps to call()

    std::vector<base::DataSourceBase::shared_ptr> args;
    DataSourceBase::shared_ptr dsb = tc->provides("methods")->produce("m0", args, caller->engine());
    DataSource<double>::shared_ptr dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -1.0, dsd->get());

    args = GenerateDataSource().operator()<int>(1);
    dsb  = tc->provides("methods")->produce("m1", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -2.0, dsd->get());

    args = GenerateDataSource().operator()<int,double>(1, 2.0);
    dsb  = tc->provides("methods")->produce("m2", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -3.0, dsd->get());

    args = GenerateDataSource().operator()<int,double,bool>(1,2.0,true);
    dsb  = tc->provides("methods")->produce("m3", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -4.0, dsd->get());

    args = GenerateDataSource().operator()<int,double,bool,std::string>(1,2.0,true,"hello");
    dsb  = tc->provides("methods")->produce("m4", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -5.0, dsd->get());

    double d = 10.0;
    args = GenerateDataSource().operator()<double&>(d);
    dsb  = tc->provides("methods")->produce("m1r", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( 20.0, dsd->get()); // 2*a
    BOOST_CHECK_EQUAL( 20.0, d ); // 2* &a
}

BOOST_AUTO_TEST_CASE(testOwnThreadMethodCall)
{
    // Test the produce() method, which maps to call() for OwnThread case:

    std::vector<base::DataSourceBase::shared_ptr> args;
    DataSourceBase::shared_ptr dsb = tc->provides("methods")->produce("o0", args, caller->engine());
    DataSource<double>::shared_ptr dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -1.0, dsd->get());

    args = GenerateDataSource().operator()<int>(1);
    dsb  = tc->provides("methods")->produce("o1", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -2.0, dsd->get());

    args = GenerateDataSource().operator()<int,double>(1, 2.0);
    dsb  = tc->provides("methods")->produce("o2", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -3.0, dsd->get());

    args = GenerateDataSource().operator()<int,double,bool>(1,2.0,true);
    dsb  = tc->provides("methods")->produce("o3", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -4.0, dsd->get());

    args = GenerateDataSource().operator()<int,double,bool,std::string>(1,2.0,true,"hello");
    dsb  = tc->provides("methods")->produce("o4", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -5.0, dsd->get());
}


BOOST_AUTO_TEST_CASE(testOwnThreadMethodSend)
{
    BOOST_REQUIRE( tc->isRunning() );
    BOOST_REQUIRE( caller->isRunning() );
    SendHandle<double(void)> h0;
    SendHandle<double(int)> h1;
    SendHandle<double(int,double)> h2;
    SendHandle<double(int,double,bool)> h3;
    SendHandle<double(int,double,bool,std::string)> h4;
    SendHandle<double(double&)> h1r;

    // Test the produceSend() and produceHandle() methods, which maps to send() for OwnThread case:

    std::vector<base::DataSourceBase::shared_ptr> args;
    ValueDataSource<double>::shared_ptr dsr = new ValueDataSource<double>();
    DataSourceBase::shared_ptr dsb; // send
    DataSourceBase::shared_ptr dsh; // handle
    DataSourceBase::shared_ptr dsc; // collect
    ActionInterface* update;

    // Get send and handle functors:
    dsb = tc->provides("methods")->produceSend("o0", args, caller->engine());
    dsh= tc->provides("methods")->produceHandle("o0");
    args.clear();
    args.push_back( dsh );
    args.push_back( dsr );
    DataSourceBase::shared_ptr dsc0 = tc->provides("methods")->produceCollect("o0",args,true);

    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateCommand( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;

    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(void)> >* >( dsh.get() ) );
    h0 = dynamic_cast<DataSource<SendHandle<double(void)> >* >( dsh.get() )->get();
    BOOST_CHECK( h0.ready() );

    args = GenerateDataSource().operator()<int>(1);
    // Get send and handle functors:
    dsb = tc->provides("methods")->produceSend("o1", args, caller->engine());
    dsh= tc->provides("methods")->produceHandle("o1");
    args.clear();
    args.push_back( dsh );
    args.push_back( dsr );
    DataSourceBase::shared_ptr dsc1 = tc->provides("methods")->produceCollect("o1",args,true);

    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateCommand( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;

    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(int)> >* >( dsh.get() ) );
    h1 = dynamic_cast<DataSource<SendHandle<double(int)> >* >( dsh.get() )->get();
    BOOST_CHECK( h1.ready() );


    args = GenerateDataSource().operator()<int,double>(1, 2.0);
    // Get send and handle functors:
    dsb = tc->provides("methods")->produceSend("o2", args, caller->engine());
    dsh= tc->provides("methods")->produceHandle("o2");

    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateCommand( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;

    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(int,double)> >* >( dsh.get() ) );
    h2 = dynamic_cast<DataSource<SendHandle<double(int,double)> >* >( dsh.get() )->get();
    BOOST_CHECK( h2.ready() );

    args = GenerateDataSource().operator()<int,double,bool>(1,2.0,true);
    // Get send and handle functors:
    dsb = tc->provides("methods")->produceSend("o3", args, caller->engine());
    dsh= tc->provides("methods")->produceHandle("o3");

    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateCommand( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;
    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(int,double,bool)> >* >( dsh.get() ) );
    h3 = dynamic_cast<DataSource<SendHandle<double(int,double,bool)> >* >( dsh.get() )->get();
    BOOST_CHECK( h3.ready() );

    args = GenerateDataSource().operator()<int,double,bool,std::string>(1,2.0,true,"hello");
    // Get send and handle functors:
    dsb = tc->provides("methods")->produceSend("o4", args, caller->engine());
    dsh= tc->provides("methods")->produceHandle("o4");
    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateCommand( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;
    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string)> >* >( dsh.get() ) );
    h4 = dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string)> >* >( dsh.get() )->get();
    BOOST_CHECK( h4.ready() );

    double d = 10.0;
    args = GenerateDataSource().operator()<double&>(d);
    dsb = tc->provides("methods")->produceSend("o1r", args, caller->engine());
    dsh = tc->provides("methods")->produceHandle("o1r");
    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateCommand( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;
    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(double&)> >* >( dsh.get() ) );
    h1r = dynamic_cast<DataSource<SendHandle<double(double&)> >* >( dsh.get() )->get();
    BOOST_CHECK( h1r.ready() );

    // Now check all receptions all-in-once:
    double retn=0;
    double rarg=0;
    dsc0->evaluate();
    BOOST_CHECK_EQUAL( dsr->get(), -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h0.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1r.collect(retn, rarg) );
    BOOST_CHECK_EQUAL( retn, 20.0 );
    BOOST_CHECK_EQUAL( rarg, 20.0 );

    retn = rarg = 0;
    // collectIfDone will certainly succeed after collect
    BOOST_CHECK_EQUAL( SendSuccess, h0.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1r.collectIfDone(retn,rarg) );
    BOOST_CHECK_EQUAL( retn, 20.0 );
    BOOST_CHECK_EQUAL( rarg, 20.0 );

    // the return value api.
    BOOST_CHECK_EQUAL( -1.0, h0.ret() );
    BOOST_CHECK_EQUAL( -2.0, h1.ret(1) );
    BOOST_CHECK_EQUAL( -3.0, h2.ret(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );
    BOOST_CHECK_EQUAL( 20.0, h1r.ret() );
}
#if 0
#if 0
BOOST_AUTO_TEST_CASE(testRemoteMethod)
{
    Method<double(void)> m0;
    boost::shared_ptr<ActionInterface> implementation( new detail::RemoteMethod<double(void)>(tc->provides("methods"),"m0") );
    m0 = implementation;
    BOOST_CHECK( m0.ready() );

    Method<double(int)> m1;
    implementation.reset( new detail::RemoteMethod<double(int)>(tc->provides("methods"),"m1") );
    m1 = implementation;
    BOOST_CHECK( m1.ready() );

    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -1.0, m0() );
}
#endif

BOOST_AUTO_TEST_CASE(testMethodsC)
{
    MethodC mc;
    double r = 0.0;
    mc = tc->provides("methods")->create("m0").ret( r );
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -1.0 );

    mc = tc->provides("methods")->create("m2").argC(1).argC(1.0).ret( r );
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -3.0 );

//    mc = tc->provides("methods")->create("m3").ret( r ).argC(1).argC(1.0).argC(true);
//    BOOST_CHECK( mc.execute() );
//    BOOST_CHECK( r == -4.0 );

#if 0
        +" set r = methods.m0()\n"
        +" do methods.assert( r == -1.0 )\n"
        +" set r = methods.m1(1)\n"
        +" do methods.assert( r == -2.0 )\n"
        +" set r = methods.m2(1, 1.0)\n"
        +" do methods.assert( r == -3.0 )\n"
        +" set r = methods.m3(1, 1.0, true)\n"
        +" do methods.assert( r == -4.0 )\n"
        +" set r = methods.m4(1, 1.0, true, \"hello\")\n"
        +" do methods.assert( r == -5.0 )\n"
#endif
}

BOOST_AUTO_TEST_CASE(testMethodFactory)
{
    // Test the addition of 'simple' methods to the operation interface,
    // and retrieving it back in a new Method object.

    Operation<double(void)> m0("m0");
    m0.calls(&FunctorsTest::m0, this);
    Operation<double(int)> m1("m1");
    m1.calls(&FunctorsTest::m1, this);
    Operation<double(int,double)> m2("m2");
    m2.calls(&FunctorsTest::m2, this);

    ServiceProvider to("task");

    BOOST_CHECK( !to.addOperation(m0).ready() );
    to.setOwner(tc);
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m1).ready() );
    BOOST_CHECK( to.addOperation(m2).ready() );

    // test constructor
    Method<double(void)> mm0 = to.getOperation<double(void)>("m0");
    BOOST_CHECK( mm0.getMethodImpl() );
    BOOST_CHECK( mm0.ready() );

    // test operator=()
    Method<double(int)> mm1;
    mm1 = to.getOperation<double(int)>("m1");
    BOOST_CHECK( mm1.getMethodImpl() );
    BOOST_CHECK( mm1.ready() );

    Method<double(int,double)> mm2 = to.getOperation<double(int,double)>("m2");
    BOOST_CHECK( mm2.getMethodImpl() );
    BOOST_CHECK( mm2.ready() );

    // execute methods and check status:
    BOOST_CHECK_EQUAL( -1.0, mm0() );

    BOOST_CHECK_EQUAL( -2.0, mm1(1) );
    BOOST_CHECK_EQUAL( -3.0, mm2(1, 2.0) );

    // test error cases:
    // Add uninitialised op:
    Operation<void(void)> ovoid("voidm");
    BOOST_CHECK(to.addOperation( ovoid ).ready() == false);
    ovoid = Operation<void(void)>("voidm");
    BOOST_CHECK(to.addOperation( ovoid ).ready() == false);

    // wrong type 1:
    Method<void(void)> mvoid;
    mvoid = to.getOperation<void(void)>("m1");
    BOOST_CHECK( mvoid.ready() == false );
    // wrong type 2:
    mvoid = to.getOperation<void(bool)>("m1");
    // wrong type 3:
    mvoid = to.getOperation<double(void)>("m0");
    BOOST_CHECK( mvoid.ready() == false );
    // non existing
    mvoid = to.getOperation<void(void)>("voidm");
    BOOST_CHECK( mvoid.ready() == false );

    // this line may not crash:
    mvoid();

}

BOOST_AUTO_TEST_CASE(testCRMethod)
{
    this->ret = -3.3;

    Method<double&(void)> m0r("m0r", &FunctorsTest::m0r, this);
    Method<const double&(void)> m0cr("m0cr", &FunctorsTest::m0cr, this);

    Method<double(double&)> m1r("m1r", &FunctorsTest::m1r, this);
    Method<double(const double&)> m1cr("m1cr", &FunctorsTest::m1cr, this);

    BOOST_CHECK_EQUAL( -3.3, m0r() );
    BOOST_CHECK_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    BOOST_CHECK_EQUAL( 5.3*2, m1r(value) );
    BOOST_CHECK_EQUAL( 5.3*2, value );
    BOOST_CHECK_EQUAL( 5.3, m1cr(5.3) );
}

#if 0
BOOST_AUTO_TEST_CASE(testMethodFromDS)
{
    ServiceProvider to("task");

    Method<double(void)> m0("m0", &FunctorsTest::m0, this);
    Method<double(int)> m1("m1", &FunctorsTest::m1, this);
    Method<double(int,double)> m2("m2", &FunctorsTest::m2, this);
    Method<double(int,double,bool)> m3("m3", &FunctorsTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &FunctorsTest::m4, this);

    to.addOperation( &m0, "desc");
    to.addOperation( &m1, "desc", "a1", "d1");
    to.addOperation( &m2, "desc", "a1", "d1", "a2","d2");
    to.addOperation( &m3, "desc", "a1", "d1", "a2","d2","a3","d3");
    to.addOperation( &m4, "desc", "a1", "d1", "a2","d2","a3","d3", "a4","d4");

    double ret;
    MethodC mc0( to.methods(), "m0");
    mc0.ret(ret);
    MethodC mc1( to.methods(), "m1");
    mc1.argC(1).ret(ret);
    MethodC mc2( to.methods(), "m2");
    mc2.argC(1).argC(2.0).ret(ret);
    MethodC mc3( to.methods(), "m3");
    mc3.argC(1).argC(2.0).argC(false).ret(ret);
    MethodC mc4( to.methods(), "m4");
    mc4.argC(1).argC(2.0).argC(false).argC(std::string("hello")).ret(ret);

    BOOST_CHECK( mc0.execute() );
    BOOST_CHECK_EQUAL(-1.0, ret);
    BOOST_CHECK( mc1.execute() );
    BOOST_CHECK_EQUAL(-2.0, ret);
    BOOST_CHECK( mc2.execute() );
    BOOST_CHECK_EQUAL(-3.0, ret);
    BOOST_CHECK( mc3.execute() );
    BOOST_CHECK_EQUAL(-4.0, ret);
    BOOST_CHECK( mc4.execute() );
    BOOST_CHECK_EQUAL(-5.0, ret);
}
#endif

BOOST_AUTO_TEST_CASE(testDSMethod)
{
    ServiceProvider to("task", tc);

    // A method of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    Operation<double(FunctorsTest*)> meth0("m0");
    meth0.calls( boost::bind(&FunctorsTest::m0, _1));

    //method_ds("m0", &FunctorsTest::m0);

    Operation<double(FunctorsTest*,int)> meth1("m1");
    meth1.calls(boost::bind(&FunctorsTest::m1, _1,_2));

    //method_ds("m1", &FunctorsTest::m1);
    //method_ds("ms",&FunctorsTest::comstr );

    boost::shared_ptr<FunctorsTest> ptr( new FunctorsTest() );
    ValueDataSource<boost::weak_ptr<FunctorsTest> >::shared_ptr wp = new ValueDataSource<boost::weak_ptr<FunctorsTest> >( ptr );
    BOOST_CHECK( to.addOperationDS( wp.get(), meth0).doc("desc" ).ready() );
    BOOST_CHECK( to.addOperationDS( wp.get(), meth1).doc("desc").arg("a1", "d1" ).ready() );

    // this actually works ! the method will detect the deleted pointer.
    //ptr.reset();

    double ret;
    MethodC c0  = to.create("m0").ret(ret);
    BOOST_CHECK( c0.execute() );
    BOOST_CHECK_EQUAL( -1.0, ret );
    MethodC c1  = to.create("m1").argC(1).ret(ret);
    BOOST_CHECK( c1.execute() );
    BOOST_CHECK_EQUAL( -2.0, ret );

}
#endif

BOOST_AUTO_TEST_SUITE_END()

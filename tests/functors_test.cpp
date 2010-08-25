#include <iostream>
#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include <Operation.hpp>
#ifdef ORO_REMOTING
#include <internal/RemoteOperationCaller.hpp>
#endif
#include <Service.hpp>
#include <internal/DataSourceGenerator.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

/**
 * This test suite tests the functors (FusedMCall, FusedMCollect, FusedMSend,...)
 * indirectly by testing the produce API of OperationInterfacePart.
 */
BOOST_FIXTURE_TEST_SUITE(  FunctorsTestSuite,  OperationsFixture )

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

    args = GenerateDataSource().operator()<int,double,bool,std::string,float>(1,2.0,true,"hello", 5.0f);
    dsb  = tc->provides("methods")->produce("m5", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -6.0, dsd->get());

    args = GenerateDataSource().operator()<int,double,bool,std::string,float>(1,2.0,true,"hello", 5.0f, 'a');
    dsb  = tc->provides("methods")->produce("m6", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -7.0, dsd->get());

    args = GenerateDataSource().operator()<int,double,bool,std::string,float>(1,2.0,true,"hello", 5.0f, 'a', (unsigned int)7);
    dsb  = tc->provides("methods")->produce("m7", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -8.0, dsd->get());

    double d = 10.0;
    args = GenerateDataSource().operator()<double&>(d);
    dsb  = tc->provides("methods")->produce("m1r", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( 20.0, dsd->get()); // 2*a
    BOOST_CHECK_EQUAL( 20.0, d ); // 2* &a
}

BOOST_AUTO_TEST_CASE(testOwnThreadOperationCallerCall)
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

    args = GenerateDataSource().operator()<int,double,bool,std::string,float>(1,2.0,true,"hello", 5.0f);
    dsb  = tc->provides("methods")->produce("o5", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -6.0, dsd->get());

    args = GenerateDataSource().operator()<int,double,bool,std::string,float,char>(1,2.0,true,"hello", 5.0f, 'a');
    dsb  = tc->provides("methods")->produce("o6", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -7.0, dsd->get());

    args = GenerateDataSource().operator()<int,double,bool,std::string,float,char,unsigned int>(1,2.0,true,"hello", 5.0f, 'a', (unsigned int)7);
    dsb  = tc->provides("methods")->produce("o7", args, caller->engine());
    dsd = dynamic_pointer_cast<DataSource<double> >(dsb);
    BOOST_CHECK_EQUAL( -8.0, dsd->get());
}


BOOST_AUTO_TEST_CASE(testOwnThreadOperationCallerSend)
{
    BOOST_REQUIRE( tc->isRunning() );
    BOOST_REQUIRE( caller->isRunning() );
    SendHandle<double(void)> h0;
    SendHandle<double(int)> h1;
    SendHandle<double(int,double)> h2;
    SendHandle<double(int,double,bool)> h3;
    SendHandle<double(int,double,bool,std::string)> h4;
    SendHandle<double(int,double,bool,std::string,float)> h5;
    SendHandle<double(int,double,bool,std::string,float,char)> h6;
    SendHandle<double(int,double,bool,std::string,float,char,unsigned int)> h7;
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
    DataSourceBase::shared_ptr dsc0 = tc->provides("methods")->produceCollect("o0",args,new ValueDataSource<bool>(true));

    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateAction( dsb.get() );
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
    DataSourceBase::shared_ptr dsc1 = tc->provides("methods")->produceCollect("o1",args, new ValueDataSource<bool>(true));

    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateAction( dsb.get() );
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
    update = dsh->updateAction( dsb.get() );
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
    update = dsh->updateAction( dsb.get() );
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
    update = dsh->updateAction( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;
    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string)> >* >( dsh.get() ) );
    h4 = dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string)> >* >( dsh.get() )->get();
    BOOST_CHECK( h4.ready() );

    args = GenerateDataSource().operator()<int,double,bool,std::string,float>(1,2.0,true,"hello",5.0f);
    // Get send and handle functors:
    dsb = tc->provides("methods")->produceSend("o5", args, caller->engine());
    dsh= tc->provides("methods")->produceHandle("o5");
    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateAction( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;
    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string,float)> >* >( dsh.get() ) );
    h5 = dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string,float)> >* >( dsh.get() )->get();
    BOOST_CHECK( h5.ready() );

    args = GenerateDataSource().operator()<int,double,bool,std::string,float,char>(1,2.0,true,"hello",5.0f,'a');
    // Get send and handle functors:
    dsb = tc->provides("methods")->produceSend("o6", args, caller->engine());
    dsh= tc->provides("methods")->produceHandle("o6");
    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateAction( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;
    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string,float,char)> >* >( dsh.get() ) );
    h6 = dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string,float,char)> >* >( dsh.get() )->get();
    BOOST_CHECK( h6.ready() );

    args = GenerateDataSource().operator()<int,double,bool,std::string,float,char,unsigned int>(1,2.0,true,"hello",5.0f,'a',(unsigned int)7);
    // Get send and handle functors:
    dsb = tc->provides("methods")->produceSend("o7", args, caller->engine());
    dsh= tc->provides("methods")->produceHandle("o7");
    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateAction( dsb.get() );
    update->readArguments();
    update->execute();
    delete update;
    // get the handle:
    BOOST_REQUIRE( dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string,float,char,unsigned int)> >* >( dsh.get() ) );
    h7 = dynamic_cast<DataSource<SendHandle<double(int,double,bool,std::string,float,char,unsigned int)> >* >( dsh.get() )->get();
    BOOST_CHECK( h7.ready() );


    double d = 10.0;
    args = GenerateDataSource().operator()<double&>(d);
    dsb = tc->provides("methods")->produceSend("o1r", args, caller->engine());
    dsh = tc->provides("methods")->produceHandle("o1r");
    // Assign the handle of produceSend to the one of produceHandle:
    update = dsh->updateAction( dsb.get() );
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
    BOOST_CHECK_EQUAL( SendSuccess, h5.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -6.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h6.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -7.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h7.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -8.0 );
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
    BOOST_CHECK_EQUAL( SendSuccess, h5.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -6.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h6.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -7.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h7.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -8.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1r.collectIfDone(retn,rarg) );
    BOOST_CHECK_EQUAL( retn, 20.0 );
    BOOST_CHECK_EQUAL( rarg, 20.0 );

    // the return value api.
    BOOST_CHECK_EQUAL( -1.0, h0.ret() );
    BOOST_CHECK_EQUAL( -2.0, h1.ret(1) );
    BOOST_CHECK_EQUAL( -3.0, h2.ret(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -6.0, h5.ret(1, 2.0, true,"hello", 5.0f) );
    BOOST_CHECK_EQUAL( -7.0, h6.ret(1, 2.0, true,"hello", 5.0f, 'a') );
    BOOST_CHECK_EQUAL( -8.0, h7.ret(1, 2.0, true,"hello", 5.0f, 'a', (unsigned int)7) );

    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );
    BOOST_CHECK_EQUAL( -6.0, h5.ret() );
    BOOST_CHECK_EQUAL( -7.0, h6.ret() );
    BOOST_CHECK_EQUAL( -8.0, h7.ret() );
    BOOST_CHECK_EQUAL( 20.0, h1r.ret() );
}
#ifdef ORO_REMOTING_OLD

BOOST_AUTO_TEST_CASE(testOperationCallerFactory)
{
    // Test the addition of 'simple' methods to the operation interface,
    // and retrieving it back in a new OperationCaller object.

    Operation<double(void)> m0("m0");
    m0.calls(&FunctorsTest::m0, this);
    Operation<double(int)> m1("m1");
    m1.calls(&FunctorsTest::m1, this);
    Operation<double(int,double)> m2("m2");
    m2.calls(&FunctorsTest::m2, this);

    Service to("task");

    BOOST_CHECK( !to.addOperation(m0).ready() );
    to.setOwner(tc);
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m1).ready() );
    BOOST_CHECK( to.addOperation(m2).ready() );

    // test constructor
    OperationCaller<double(void)> mm0 = to.getOperation<double(void)>("m0");
    BOOST_CHECK( mm0.getOperationCallerImpl() );
    BOOST_CHECK( mm0.ready() );

    // test operator=()
    OperationCaller<double(int)> mm1;
    mm1 = to.getOperation<double(int)>("m1");
    BOOST_CHECK( mm1.getOperationCallerImpl() );
    BOOST_CHECK( mm1.ready() );

    OperationCaller<double(int,double)> mm2 = to.getOperation<double(int,double)>("m2");
    BOOST_CHECK( mm2.getOperationCallerImpl() );
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
    OperationCaller<void(void)> mvoid;
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

BOOST_AUTO_TEST_CASE(testCROperationCaller)
{
    this->ret = -3.3;

    OperationCaller<double&(void)> m0r("m0r", &FunctorsTest::m0r, this);
    OperationCaller<const double&(void)> m0cr("m0cr", &FunctorsTest::m0cr, this);

    OperationCaller<double(double&)> m1r("m1r", &FunctorsTest::m1r, this);
    OperationCaller<double(const double&)> m1cr("m1cr", &FunctorsTest::m1cr, this);

    BOOST_CHECK_EQUAL( -3.3, m0r() );
    BOOST_CHECK_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    BOOST_CHECK_EQUAL( 5.3*2, m1r(value) );
    BOOST_CHECK_EQUAL( 5.3*2, value );
    BOOST_CHECK_EQUAL( 5.3, m1cr(5.3) );
}

BOOST_AUTO_TEST_CASE(testOperationCallerFromDS)
{
    Service to("task");

    OperationCaller<double(void)> m0("m0", &FunctorsTest::m0, this);
    OperationCaller<double(int)> m1("m1", &FunctorsTest::m1, this);
    OperationCaller<double(int,double)> m2("m2", &FunctorsTest::m2, this);
    OperationCaller<double(int,double,bool)> m3("m3", &FunctorsTest::m3, this);
    OperationCaller<double(int,double,bool,std::string)> m4("m4", &FunctorsTest::m4, this);

    to.addOperation( &m0, "desc");
    to.addOperation( &m1, "desc", "a1", "d1");
    to.addOperation( &m2, "desc", "a1", "d1", "a2","d2");
    to.addOperation( &m3, "desc", "a1", "d1", "a2","d2","a3","d3");
    to.addOperation( &m4, "desc", "a1", "d1", "a2","d2","a3","d3", "a4","d4");

    double ret;
    OperationCallerC mc0( to.methods(), "m0");
    mc0.ret(ret);
    OperationCallerC mc1( to.methods(), "m1");
    mc1.argC(1).ret(ret);
    OperationCallerC mc2( to.methods(), "m2");
    mc2.argC(1).argC(2.0).ret(ret);
    OperationCallerC mc3( to.methods(), "m3");
    mc3.argC(1).argC(2.0).argC(false).ret(ret);
    OperationCallerC mc4( to.methods(), "m4");
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

BOOST_AUTO_TEST_CASE(testDSOperationCaller)
{
    Service to("task", tc);

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
    OperationCallerC c0  = to.create("m0").ret(ret);
    BOOST_CHECK( c0.execute() );
    BOOST_CHECK_EQUAL( -1.0, ret );
    OperationCallerC c1  = to.create("m1").argC(1).ret(ret);
    BOOST_CHECK( c1.execute() );
    BOOST_CHECK_EQUAL( -2.0, ret );

}
#endif // defined(ORO_REMOTING)

BOOST_AUTO_TEST_SUITE_END()

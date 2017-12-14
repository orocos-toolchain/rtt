/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "unit.hpp"
#include <signal.h>

#include <base/ChannelElement.hpp>

using namespace std;
using namespace RTT;
using namespace RTT::base;

// Registers the test suite into the 'registry'
BOOST_AUTO_TEST_SUITE(  ChannelElementsTestSuite )

BOOST_AUTO_TEST_CASE( testChannelElementConnection )
{
    ChannelElement<int>::shared_ptr out(new ChannelElement<int>());
    ChannelElement<int>::shared_ptr in(new ChannelElement<int>());

    BOOST_CHECK( !out->connected() );
    BOOST_CHECK( !in->connected() );

    // connect from out to in
    out->connectTo(in);
    BOOST_CHECK_EQUAL( out->getOutput(), in );
    BOOST_CHECK_EQUAL( in->getInput(), out );
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( in->connected() );

    // disconnect out
    out->disconnect(true);
    BOOST_CHECK( !out->getOutput() );
    BOOST_CHECK( !in->getInput() );
    BOOST_CHECK( !out->connected() );
    BOOST_CHECK( !in->connected() );

    // connect from in to out
    in->connectFrom(out);
    BOOST_CHECK_EQUAL( out->getOutput(), in );
    BOOST_CHECK_EQUAL( in->getInput(), out );
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( in->connected() );

    // disconnect in
    in->disconnect(false);
    BOOST_CHECK( !out->getOutput() );
    BOOST_CHECK( !in->getInput() );
    BOOST_CHECK( !out->connected() );
    BOOST_CHECK( !in->connected() );
}

BOOST_AUTO_TEST_CASE( testMultipleOutputsChannelElementConnection )
{
    ChannelElement<int>::shared_ptr out(new MultipleOutputsChannelElement<int>());
    ChannelElement<int>::shared_ptr in1(new ChannelElement<int>());
    ChannelElement<int>::shared_ptr in2(new ChannelElement<int>());

    BOOST_CHECK( !out->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK( !in2->connected() );

    // connect from output to inputs
    BOOST_CHECK( out->connectTo(in1) );
    BOOST_CHECK( out->connectTo(in2) );
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( in1->connected() );
    BOOST_CHECK( in2->connected() );
    BOOST_CHECK( !out->getOutput() );
    BOOST_CHECK_EQUAL( in1->getInput(), out );
    BOOST_CHECK_EQUAL( in2->getInput(), out );

    // disconnect output
    out->disconnect(true);
    BOOST_CHECK( !out->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK( !in2->connected() );
    BOOST_CHECK( !in1->getInput() );
    BOOST_CHECK( !in2->getInput() );

    // connect from inputs to output
    in1->connectFrom(out);
    in2->connectFrom(out);
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( in1->connected() );
    BOOST_CHECK( in2->connected() );
    BOOST_CHECK( !out->getOutput() );
    BOOST_CHECK_EQUAL( in1->getInput(), out );
    BOOST_CHECK_EQUAL( in2->getInput(), out );

    // add another output that writes into out
    ChannelElement<int>::shared_ptr before_out(new ChannelElement<int>());
    before_out->connectTo(out);

    // disconnect from inputs
    in1->disconnect(out, false);
    BOOST_CHECK( before_out->connected() );
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK( in2->connected() );
    in2->disconnect(out, false);
    BOOST_CHECK( !out->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK( !in2->connected() );

    // check that the disconnect call was forwarded to out's input after the last disconnect
    BOOST_CHECK( !before_out->connected() );
}

BOOST_AUTO_TEST_CASE( testMultipleInputsChannelElementConnection )
{
    ChannelElement<int>::shared_ptr out1(new ChannelElement<int>());
    ChannelElement<int>::shared_ptr out2(new ChannelElement<int>());
    MultipleInputsChannelElement<int>::shared_ptr in(new MultipleInputsChannelElement<int>());

    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( !out2->connected() );
    BOOST_CHECK( !in->connected() );

    // connect from input to outputs
    BOOST_CHECK( in->connectFrom(out1) );
    BOOST_CHECK( in->connectFrom(out2) );
    BOOST_CHECK( in->connected() );
    BOOST_CHECK( out1->connected() );
    BOOST_CHECK( out2->connected() );
    BOOST_CHECK( !in->getInput() );
    BOOST_CHECK_EQUAL( out1->getOutput(), in );
    BOOST_CHECK_EQUAL( out2->getOutput(), in );

    // disconnect input
    in->disconnect(false);
    BOOST_CHECK( !in->connected() );
    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( !out2->connected() );
    BOOST_CHECK( !out1->getOutput() );
    BOOST_CHECK( !out2->getOutput() );

    // connect from outputs to input
    out1->connectTo(in);
    out2->connectTo(in);
    BOOST_CHECK( out1->connected() );
    BOOST_CHECK( out2->connected() );
    BOOST_CHECK( in->connected() );
    BOOST_CHECK( !in->getInput() );
    BOOST_CHECK_EQUAL( out1->getOutput(), in );
    BOOST_CHECK_EQUAL( out2->getOutput(), in );

    // add another input that reads from in
    ChannelElement<int>::shared_ptr after_in(new ChannelElement<int>());
    after_in->connectFrom(in);

    // disconnect from inputs
    out1->disconnect(in, true);
    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( out2->connected() );
    BOOST_CHECK( in->connected() );
    BOOST_CHECK( after_in->connected() );
    out2->disconnect(in, true);
    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( !out2->connected() );
    BOOST_CHECK( !in->connected() );

    // check that the disconnect call was forwarded to in's output after the last disconnect
    BOOST_CHECK( !after_in->connected() );
}

BOOST_AUTO_TEST_CASE( testMultipleInputsMultipleOutputsChannelElementConnection )
{
    ChannelElement<int>::shared_ptr out1(new ChannelElement<int>());
    ChannelElement<int>::shared_ptr out2(new ChannelElement<int>());
    MultipleInputsMultipleOutputsChannelElement<int>::shared_ptr mimo(new MultipleInputsMultipleOutputsChannelElement<int>());
    ChannelElement<int>::shared_ptr in1(new ChannelElement<int>());
    ChannelElement<int>::shared_ptr in2(new ChannelElement<int>());

    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( !out2->connected() );
    BOOST_CHECK( !mimo->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK( !in2->connected() );

    // connect from mimo to inputs
    BOOST_CHECK( mimo->connectFrom(out1) );
    BOOST_CHECK( mimo->connectFrom(out2) );
    BOOST_CHECK( !mimo->connected() ); // no outputs yet
    BOOST_CHECK( out1->connected() );
    BOOST_CHECK( out2->connected() );
    BOOST_CHECK( !mimo->getInput() );
    BOOST_CHECK_EQUAL( out1->getOutput(), mimo );
    BOOST_CHECK_EQUAL( out2->getOutput(), mimo );

    // connect from mimo to outputs
    BOOST_CHECK( mimo->connectTo(in1) );
    BOOST_CHECK( mimo->connected() );
    BOOST_CHECK( mimo->connectTo(in2) );
    BOOST_CHECK( in1->connected() );
    BOOST_CHECK( in2->connected() );
    BOOST_CHECK( !mimo->getOutput() );
    BOOST_CHECK_EQUAL( in1->getInput(), mimo );
    BOOST_CHECK_EQUAL( in2->getInput(), mimo );

    // disconnect inputs
    mimo->disconnect(false);
    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( !out2->connected() );
    BOOST_CHECK( !mimo->connected() );
    BOOST_CHECK( !mimo->MultipleInputsChannelElementBase::connected() );
    BOOST_CHECK( mimo->MultipleOutputsChannelElementBase::connected() );
    BOOST_CHECK( in1->connected() );
    BOOST_CHECK( in2->connected() );

    // disconnect outputs
    mimo->disconnect(true);
    BOOST_CHECK( !mimo->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK( !in2->connected() );

    // connect from outputs and inputs to mimo
    out1->connectTo(mimo);
    out2->connectTo(mimo);
    BOOST_CHECK( !mimo->connected() ); // no outputs yet
    in1->connectFrom(mimo);
    BOOST_CHECK( mimo->connected() );
    in2->connectFrom(mimo);
    BOOST_CHECK( out1->connected() );
    BOOST_CHECK( out2->connected() );
    BOOST_CHECK( in1->connected() );
    BOOST_CHECK( in2->connected() );
    BOOST_CHECK_EQUAL( out1->getOutput(), mimo );
    BOOST_CHECK_EQUAL( out2->getOutput(), mimo );
    BOOST_CHECK_EQUAL( in1->getInput(), mimo );
    BOOST_CHECK_EQUAL( in2->getInput(), mimo );

    // disconnect from inputs and outputs
    out1->disconnect(mimo, true);
    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( out2->connected() );
    BOOST_CHECK( mimo->connected() );
    BOOST_CHECK( in1->connected() );
    BOOST_CHECK( in2->connected() );
    in1->disconnect(mimo, false);
    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( out2->connected() );
    BOOST_CHECK( mimo->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK( in2->connected() );
    out2->disconnect(mimo, true);
    BOOST_CHECK( !out1->connected() );
    BOOST_CHECK( !out2->connected() );
    BOOST_CHECK( !mimo->connected() );

    // check that the disconnect call was forwarded to mimo's outputs after the last input has been disconnected
    BOOST_CHECK( !in2->connected() );
}

BOOST_AUTO_TEST_CASE( testDisconnectOnInvalidWrite )
{
    ChannelElement<int>::shared_ptr out(new MultipleOutputsChannelElement<int>());
    ChannelElement<int>::shared_ptr in1(new ChannelElement<int>());
    ChannelElement<int>::shared_ptr in2(new ChannelElement<int>());
    ChannelElement<int>::shared_ptr data1(new internal::ChannelDataElement<int>(base::DataObjectInterface<int>::shared_ptr(new base::DataObject<int>())));
    ChannelElement<int>::shared_ptr data2(new internal::ChannelDataElement<int>(base::DataObjectInterface<int>::shared_ptr(new base::DataObject<int>())));
    int x = 0;

    // 1. NON-MANDATORY connections: out->in1->data1 and out->in2->data2
    BOOST_CHECK( out->connectTo(in1, /* mandatory = */ false) );
    BOOST_CHECK( out->connectTo(in2, /* mandatory = */ false) );
    BOOST_CHECK( in1->connectTo(data1) );
    BOOST_CHECK( in2->connectTo(data2) );
    BOOST_CHECK_EQUAL( out->write(1), WriteSuccess );
    BOOST_CHECK( data1->read(x) == NewData && x == 1 );
    BOOST_CHECK( data2->read(x) == NewData && x == 1 );

    // disconnect in2 without backward propagation. This invalidates in2.
    in2->disconnect(true);
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( in1->connected() );
    BOOST_CHECK( !in2->connected() );
    BOOST_CHECK_EQUAL( out->write(2), WriteSuccess ); // in2 is not mandatory
    BOOST_CHECK( data1->read(x) == NewData && x == 2 );
    BOOST_CHECK( data2->read(x) == OldData && x == 1 );
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( in1->connected() );

    // additionally disconnect in1 without backward propagation. This also invalidates in1.
    in1->disconnect(true);
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK_EQUAL( out->write(3), NotConnected ); // in1 is not mandatory, but it was the last connected output
    BOOST_CHECK( data1->read(x) == OldData && x == 2 );
    BOOST_CHECK( data2->read(x) == OldData && x == 1 );
    BOOST_CHECK( !out->connected() );

    // 2. MANDATORY connections: out->in1->data1 and out->in2->data2
    BOOST_CHECK( out->connectTo(in1, /* mandatory = */ true) );
    BOOST_CHECK( out->connectTo(in2, /* mandatory = */ true) );
    BOOST_CHECK( in1->connectTo(data1) );
    BOOST_CHECK( in2->connectTo(data2) );
    BOOST_CHECK_EQUAL( out->write(1), WriteSuccess );
    BOOST_CHECK( data1->read(x) == NewData && x == 1 );
    BOOST_CHECK( data2->read(x) == NewData && x == 1 );

    // disconnect in2 without backward propagation. This invalidates in2.
    in2->disconnect(true);
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( in1->connected() );
    BOOST_CHECK( !in2->connected() );
    BOOST_CHECK_EQUAL( out->write(2), NotConnected );
    BOOST_CHECK( data1->read(x) == NewData && x == 2 );
    BOOST_CHECK( data2->read(x) == OldData && x == 1 );
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( in1->connected() );

    // additionally disconnect in1 without backward propagation. This also invalidates in1.
    in1->disconnect(true);
    BOOST_CHECK( out->connected() );
    BOOST_CHECK( !in1->connected() );
    BOOST_CHECK_EQUAL( out->write(3), NotConnected );
    BOOST_CHECK( data1->read(x) == OldData && x == 2 );
    BOOST_CHECK( data2->read(x) == OldData && x == 1 );
    BOOST_CHECK( !out->connected() );
}

BOOST_AUTO_TEST_SUITE_END()

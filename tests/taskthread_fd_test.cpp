/***************************************************************************
  tag: S Roderick taskthread_fd_test.cpp

                        taskthread_fd_test.cpp -  description
                           -------------------
    copyright            : (C) 2012 S Roderick
    email                : kiwi.net@mac.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "taskthread_test.hpp"

#include <iostream>
#include <errno.h>

#include <TaskContext.hpp>
#include <extras/FileDescriptorActivity.hpp>
#include <os/MainThread.hpp>
#include <Logger.hpp>
#include <rtt-config.h>

using namespace std;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


using namespace RTT;


struct TestFileDescriptor
	: public TaskContext
{
	TestFileDescriptor(std::string name) :
			TaskContext(name, TaskContext::PreOperational),
			countError(0),
			countTimeout(0),
			countRead(0),
			countUpdate(0)
	{
		fd[0] = fd[1] = -1;
	}

	virtual ~TestFileDescriptor()
	{
	}

	virtual bool configureHook()
	{
		int rc = pipe(fd);
		assert(0 == rc);
        (void)rc;       // avoid compiler warning

        extras::FileDescriptorActivity* fd_activity =
			dynamic_cast<extras::FileDescriptorActivity*>(getActivity());
        assert(0 != fd_activity);

		fd_activity->watch(fd[0]);
        // set no timeout - leave that for test code

		return true;
	}

	virtual bool startHook()
	{
		countError		= 0;
		countTimeout	= 0;
		countRead		= 0;
		countUpdate		= 0;
		return true;
	}

	virtual void updateHook()
	{
		extras::FileDescriptorActivity* fd_activity =
			dynamic_cast<extras::FileDescriptorActivity*>(getActivity());
		assert(0 != fd_activity);

		++countUpdate;
		if (fd_activity->hasError())
		{
			++countError;
		}
		else if (fd_activity->hasTimeout())
		{
			++countTimeout;
		}
		else
		{
			if (fd_activity->isUpdated(fd[0]))
			{
				++countRead;
				// remove pipe contents
				char ch;
				int rc = read(fd[0], &ch, sizeof(ch));
				if (0 >= rc)
				{
//					std::cerr << "Failed read: rc=" << rc << " errno=" << errno << ":" << strerror(errno) << std::endl;
				}
			}
		}
	}

	virtual void cleanupHook()
	{
		if (-1 != fd[0]) close(fd[0]);
		if (-1 != fd[1]) close(fd[1]);
		fd[0] = fd[1] = -1;
	}

	int countError;
	int countTimeout;
	int countRead;
	int countUpdate;
	int fd[2];	// from pipe()
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( ActivitiesThreadTestSuite, ActivitiesThreadTest )

BOOST_AUTO_TEST_CASE(testFileDescriptor )
{
    FileDescriptorActivity mtask( 15 );
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( !mtask.thread()->isRunning() );
    BOOST_CHECK_EQUAL( 0.0, mtask.thread()->getPeriod() );

    // Adapt priority levels to OS.
    int bprio = 15, rtsched = ORO_SCHED_RT;
    os::CheckPriority( rtsched, bprio );

    BOOST_CHECK_EQUAL( bprio, mtask.thread()->getPriority() );
    BOOST_CHECK_EQUAL( rtsched, mtask.thread()->getScheduler() );

    FileDescriptorActivity m2task( 15 );
    BOOST_CHECK( mtask.thread() != m2task.thread() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( m2task.isRunning() == false );
    BOOST_CHECK( m2task.start() == true );
    BOOST_CHECK( m2task.isRunning() == false );

    usleep(1000000/4);

    // stopping...
    BOOST_CHECK( mtask.stop() == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( m2task.isRunning() == false );
    BOOST_CHECK( m2task.stop() == true );
    BOOST_CHECK( m2task.isRunning() == false );

    // Starting thread if thread not running
    BOOST_CHECK( mtask.thread()->stop() == false );
    BOOST_CHECK( mtask.thread()->isRunning() == false );
    BOOST_CHECK( mtask.start() );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.thread()->isRunning() == false);
}

BOOST_AUTO_TEST_CASE(testFileDescriptor_Write )
{
	TestFileDescriptor		mcomp("Comp");
    mcomp.setActivity( new FileDescriptorActivity( 15 ) );
    FileDescriptorActivity* mtask = dynamic_cast<FileDescriptorActivity*>( mcomp.getActivity() );
	char					ch='a';
	int						rc;

    BOOST_CHECK( mtask->hasError() == false );
    BOOST_CHECK( mtask->hasTimeout() == false );

    BOOST_CHECK( mcomp.configure() == true );
    BOOST_CHECK( mtask->isWatched(mcomp.fd[0]) == true );

    BOOST_CHECK( mcomp.start() == true );
    BOOST_CHECK( mtask->hasError() == false );
    BOOST_CHECK( mtask->hasTimeout() == false );

	// no activity
    usleep(1000000/4);
    BOOST_CHECK_EQUAL( 0, mcomp.countError );
    BOOST_CHECK_EQUAL( 0, mcomp.countTimeout );
    BOOST_CHECK_EQUAL( 0, mcomp.countRead );
    BOOST_CHECK_LE( 0, mcomp.countUpdate );

	// write to fd
	rc = write(mcomp.fd[1], &ch, sizeof(ch));
	if (1 != rc) std::cerr << "rc=" << rc << " errno=" << errno << ":" << strerror(errno) << std::endl;
    BOOST_CHECK_EQUAL( 1, rc );
    usleep(1000000/10);
    BOOST_CHECK_EQUAL( 0, mcomp.countError );
    BOOST_CHECK_EQUAL( 0, mcomp.countTimeout );
    BOOST_CHECK_EQUAL( 1, mcomp.countRead );
    BOOST_CHECK_LE( 0, mcomp.countUpdate );

	++ch;
	rc = write(mcomp.fd[1], &ch, sizeof(ch));
	if (1 != rc) std::cerr << "rc=" << rc << " errno=" << errno << ":" << strerror(errno) << std::endl;
    BOOST_CHECK_EQUAL( 1, rc );
    usleep(1000000/10);
    BOOST_CHECK_EQUAL( 0, mcomp.countError );
    BOOST_CHECK_EQUAL( 0, mcomp.countTimeout );
    BOOST_CHECK_EQUAL( 2, mcomp.countRead );
    BOOST_CHECK_LE( 0, mcomp.countUpdate );

	// unwatch
    BOOST_CHECK( mtask->isWatched(mcomp.fd[0]) == true );
	mtask->unwatch(mcomp.fd[0]);
    BOOST_CHECK( mtask->isWatched(mcomp.fd[0]) == false );

	++ch;
	rc = write(mcomp.fd[1], &ch, sizeof(ch));
	if (1 != rc) std::cerr << "rc=" << rc << " errno=" << errno << ":" << strerror(errno) << std::endl;
    BOOST_CHECK_EQUAL( 1, rc );
    usleep(1000000/10);
    BOOST_CHECK_EQUAL( 0, mcomp.countError );
    BOOST_CHECK_EQUAL( 0, mcomp.countTimeout );
    BOOST_CHECK_EQUAL( 2, mcomp.countRead );	// no change
    BOOST_CHECK_LE( 0, mcomp.countUpdate );

#if 0
	// close pipe => cause error
	(void)close(mcomp.fd[1]);
    usleep(1000000/100);

    BOOST_CHECK_EQUAL( 1, mcomp.countError );
    BOOST_CHECK_EQUAL( 0, mcomp.countTimeout );
    BOOST_CHECK_EQUAL( 2, mcomp.countRead );
    BOOST_CHECK_LE( 0, mcomp.countUpdate );
#endif

    // Note: normally not allowed once loaded in a TC:
    BOOST_CHECK( mtask->stop() == true );
}

BOOST_AUTO_TEST_CASE(testFileDescriptor_Timeout )
{
    if(std::getenv("CI") != NULL) {
        BOOST_TEST_MESSAGE("Skipping testFileDescriptor_Timeout because it can fail on integration servers.");
        return;
    }

	TestFileDescriptor		mcomp("Comp");
    mcomp.setActivity( new FileDescriptorActivity( 15 ) );
    FileDescriptorActivity* mtask = dynamic_cast<FileDescriptorActivity*>( mcomp.getActivity() );
	char					ch='a';
	int						rc;
	static const int		RATE = 10;
	static const int		timeout_ms = 1000 / RATE;

    BOOST_CHECK( mcomp.configure() == true );
	mtask->setTimeout( timeout_ms );
	BOOST_CHECK( timeout_ms == mtask->getTimeout() );
    BOOST_CHECK( mcomp.start() == true );

	// no activity
    usleep(1000000 / RATE * 4.5);    // ~4.5 timeout periods
    BOOST_CHECK_EQUAL( 0, mcomp.countError );
    BOOST_CHECK_EQUAL( 4, mcomp.countTimeout );
    BOOST_CHECK_EQUAL( 0, mcomp.countRead );
    BOOST_CHECK_LE( 0, mcomp.countUpdate );

	// write to fd
	rc = write(mcomp.fd[1], &ch, sizeof(ch));
	if (1 != rc) std::cerr << "rc=" << rc << " errno=" << errno << ":" << strerror(errno) << std::endl;
    BOOST_CHECK_EQUAL( 1, rc );
    usleep(1000000 / RATE * 1.5);    // ~1.5 timeout periods
    BOOST_CHECK_EQUAL( 0, mcomp.countError );
    BOOST_CHECK_EQUAL( 5, mcomp.countTimeout );
    BOOST_CHECK_EQUAL( 1, mcomp.countRead );
    BOOST_CHECK_LE( 0, mcomp.countUpdate );

	// no activity
    usleep(1000000 / RATE * 3);    // ~3 timeout periods
    BOOST_CHECK_EQUAL( 0, mcomp.countError );
    BOOST_CHECK_EQUAL( 5 + 3, mcomp.countTimeout );
    BOOST_CHECK_EQUAL( 1, mcomp.countRead );
    BOOST_CHECK_LE( 0, mcomp.countUpdate );
}

BOOST_AUTO_TEST_SUITE_END()


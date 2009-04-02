#include "specialized_activities.hpp"
#include <FileDescriptorActivity.hpp>
#include <iostream>

using namespace std;
using namespace RTT;

CPPUNIT_TEST_SUITE_REGISTRATION( SpecializedActivities );

struct TestFDActivity : public FileDescriptorActivity
{
    int count;
    bool do_read;
    TestFDActivity()
        : FileDescriptorActivity(0), count(0), do_read(false) {}

    void step()
    {
        cerr << "stepping " << count << endl;
        char buffer;
        if (do_read)
            read(getFileDescriptor(), &buffer, 1);
        count += 1; 
    };
};

void SpecializedActivities::testFileDescriptorActivity()
{
    auto_ptr<TestFDActivity> activity(new TestFDActivity);

    int pipe_fds[2];
    pipe(pipe_fds);
    int reader = pipe_fds[0];
    int writer = pipe_fds[1];

    CPPUNIT_ASSERT( !activity->isRunning() && !activity->isActive() );
    CPPUNIT_ASSERT( !activity->start() );
    CPPUNIT_ASSERT( !activity->isRunning() && !activity->isActive() );
    activity->setFileDescriptor(reader);
    CPPUNIT_ASSERT( activity->start() );
    usleep(250000);
    CPPUNIT_ASSERT( !activity->isRunning() && activity->isActive() );
    CPPUNIT_ASSERT( activity->count == 0 );

    // Check trigger()
    activity->do_read = false;
    CPPUNIT_ASSERT( activity->trigger() );
    usleep(250000);
    CPPUNIT_ASSERT_EQUAL(1, activity->count);
    CPPUNIT_ASSERT( !activity->isRunning() && activity->isActive() );

    // Check normal operations
    activity->do_read = true;
    int buffer;
    write(writer, &buffer, 2);
    usleep(250000);
    CPPUNIT_ASSERT_EQUAL(3, activity->count);
    CPPUNIT_ASSERT( !activity->isRunning() && activity->isActive() );

    // Check breakLoop()
    CPPUNIT_ASSERT( activity->stop() );
    usleep(250000);
    CPPUNIT_ASSERT( !activity->isRunning() && !activity->isActive() );

    // Check that the activity did not close the FD
    write(writer, &buffer, 1);
    CPPUNIT_ASSERT( read(reader, &buffer, 1) == 1 );

    // Now test timeout and autoclose
    activity->do_read = false;
    activity->setTimeout(100);
    CPPUNIT_ASSERT_EQUAL(100, activity->getTimeout());
    activity->setFileDescriptor(reader, true);
    CPPUNIT_ASSERT( activity->start() );
    sleep(1);
    CPPUNIT_ASSERT( activity->count >= 10 );
    CPPUNIT_ASSERT( activity->stop() );
    usleep(250000);
    CPPUNIT_ASSERT( read(reader, &buffer, 1) == -1 );
}


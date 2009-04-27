#include "specialized_activities.hpp"
#include <FileDescriptorActivity.hpp>
#include <iostream>

using namespace std;
using namespace RTT;

CPPUNIT_TEST_SUITE_REGISTRATION( SpecializedActivities );

struct TestFDActivity : public FileDescriptorActivity
{
    int step_count, count, other_count;
    int fd, other_fd;

    bool do_read;
    TestFDActivity()
        : FileDescriptorActivity(0), step_count(0), count(0), other_count(0), do_read(false) {}

    void step()
    {
        char buffer;
        if (isUpdated(fd))
        {
            ++count;
            if (do_read)
                read(fd, &buffer, 1);
        }
        if (isUpdated(other_fd))
        {
            ++other_count;
            if (do_read)
                read(other_fd, &buffer, 1);
        }
        ++step_count;
    };
};

void SpecializedActivities::testFileDescriptorActivity()
{
    auto_ptr<TestFDActivity> activity(new TestFDActivity);
    static const int USLEEP = 250000;

    int pipe_fds[2];
    pipe(pipe_fds);
    int reader = pipe_fds[0];
    int writer = pipe_fds[1];

    int other_pipe[2];
    pipe(other_pipe);
    int other_reader = other_pipe[0];
    int other_writer = other_pipe[1];

    activity->fd = reader;
    activity->other_fd = other_reader;

    // The activity should refuse starting if there is nothing to watch
    CPPUNIT_ASSERT( !activity->isRunning() && !activity->isActive() );
    CPPUNIT_ASSERT( !activity->start() );
    CPPUNIT_ASSERT( !activity->isRunning() && !activity->isActive() );

    // Add something to watch and check that it does start
    activity->watch(reader);
    activity->watch(other_reader);
    CPPUNIT_ASSERT( activity->start() );
    usleep(USLEEP);
    CPPUNIT_ASSERT( !activity->isRunning() && activity->isActive() );
    CPPUNIT_ASSERT_EQUAL(0, activity->step_count);

    // Check trigger(). Disable reading as there won't be any data on the FD
    activity->do_read = false;
    CPPUNIT_ASSERT( activity->trigger() );
    usleep(USLEEP);
    CPPUNIT_ASSERT_EQUAL(1, activity->step_count);
    CPPUNIT_ASSERT_EQUAL(0, activity->count);
    CPPUNIT_ASSERT_EQUAL(0, activity->other_count);
    CPPUNIT_ASSERT( !activity->isRunning() && activity->isActive() );

    // Check normal operations. Re-enable reading.
    activity->do_read = true;
    int buffer;
    write(writer, &buffer, 2);
    usleep(USLEEP);
    CPPUNIT_ASSERT_EQUAL(3, activity->step_count);
    CPPUNIT_ASSERT_EQUAL(2, activity->count);
    CPPUNIT_ASSERT_EQUAL(0, activity->other_count);
    CPPUNIT_ASSERT( !activity->isRunning() && activity->isActive() );

    write(other_writer, &buffer, 2);
    usleep(USLEEP);
    CPPUNIT_ASSERT_EQUAL(5, activity->step_count);
    CPPUNIT_ASSERT_EQUAL(2, activity->count);
    CPPUNIT_ASSERT_EQUAL(2, activity->other_count);
    CPPUNIT_ASSERT( !activity->isRunning() && activity->isActive() );

    // Check breakLoop()
    CPPUNIT_ASSERT( activity->stop() );
    usleep(USLEEP);
    CPPUNIT_ASSERT( !activity->isRunning() && !activity->isActive() );

    // Now test timeout
    activity->do_read = false;
    activity->setTimeout(100);
    CPPUNIT_ASSERT_EQUAL(100, activity->getTimeout());
    CPPUNIT_ASSERT( activity->start() );
    sleep(1);
    CPPUNIT_ASSERT( activity->step_count >= 10 );
    CPPUNIT_ASSERT_EQUAL(2, activity->count);
    CPPUNIT_ASSERT_EQUAL(2, activity->other_count);
}


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "specialized_activities.hpp"
#include <extras/FileDescriptorActivity.hpp>
#include <iostream>

using namespace std;
using namespace RTT;

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

BOOST_FIXTURE_TEST_SUITE(SecializedActivitiesSuite,SpecializedActivities)

BOOST_AUTO_TEST_CASE( testFileDescriptorActivity )
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
    BOOST_CHECK( !activity->isRunning() && !activity->isActive() );
    BOOST_CHECK( !activity->start() );
    BOOST_CHECK( !activity->isRunning() && !activity->isActive() );

    // Add something to watch and check that it does start
    activity->watch(reader);
    activity->watch(other_reader);
    BOOST_CHECK( activity->start() );
    usleep(USLEEP);
    BOOST_CHECK( !activity->isRunning() && activity->isActive() );
    BOOST_CHECK_EQUAL(0, activity->step_count);

    // Check trigger(). Disable reading as there won't be any data on the FD
    activity->do_read = false;
    BOOST_CHECK( activity->trigger() );
    usleep(USLEEP);
    BOOST_CHECK_EQUAL(1, activity->step_count);
    BOOST_CHECK_EQUAL(0, activity->count);
    BOOST_CHECK_EQUAL(0, activity->other_count);
    BOOST_CHECK( !activity->isRunning() && activity->isActive() );

    // Check normal operations. Re-enable reading.
    activity->do_read = true;
    int buffer;
    write(writer, &buffer, 2);
    usleep(USLEEP);
    BOOST_CHECK_EQUAL(3, activity->step_count);
    BOOST_CHECK_EQUAL(2, activity->count);
    BOOST_CHECK_EQUAL(0, activity->other_count);
    BOOST_CHECK( !activity->isRunning() && activity->isActive() );

    write(other_writer, &buffer, 2);
    usleep(USLEEP);
    BOOST_CHECK_EQUAL(5, activity->step_count);
    BOOST_CHECK_EQUAL(2, activity->count);
    BOOST_CHECK_EQUAL(2, activity->other_count);
    BOOST_CHECK( !activity->isRunning() && activity->isActive() );

    // Check breakLoop()
    BOOST_CHECK( activity->stop() );
    usleep(USLEEP);
    BOOST_CHECK( !activity->isRunning() && !activity->isActive() );

    // Now test timeout
    activity->do_read = false;
    activity->setTimeout(100);
    BOOST_CHECK_EQUAL(100, activity->getTimeout());
    BOOST_CHECK( activity->start() );
    sleep(1);
    BOOST_CHECK( activity->step_count >= 10 );
    BOOST_CHECK_EQUAL(2, activity->count);
    BOOST_CHECK_EQUAL(2, activity->other_count);
}


BOOST_AUTO_TEST_SUITE_END()


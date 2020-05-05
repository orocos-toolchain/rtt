#ifdef WIN32
  #include <io.h>
  #include <fcntl.h>
  #define pipe(X) _pipe((X), 1024, _O_BINARY)
  #define close _close
  #define write _write
#endif

#include "unit.hpp"

#include "specialized_activities.hpp"
#include <extras/FileDescriptorActivity.hpp>
#include <iostream>
#include <memory>

#include <rtt-detail-fwd.hpp>
using namespace RTT::detail;

using namespace std;
using namespace RTT;

struct TestFDActivity : public FileDescriptorActivity
{
    int step_count, count, other_count;
    int fd, other_fd, result;

    bool do_read;
    base::RunnableInterface::WorkReason work_reason;

    RTT::os::Mutex mutex;

    TestFDActivity()
        : FileDescriptorActivity(0), step_count(0), count(0), other_count(0), do_read(false) {}

    void work(base::RunnableInterface::WorkReason reason)
    {
        work_reason = reason;
    }
    void step()
    {
        RTT::os::MutexLock lock(mutex);

        char buffer;
        if (isUpdated(fd))
        {
            ++count;
            if (do_read)
                result = read(fd, &buffer, 1);
        }
        if (isUpdated(other_fd))
        {
            ++other_count;
            if (do_read)
                result = read(other_fd, &buffer, 1);
        }
        ++step_count;
    }
};

BOOST_FIXTURE_TEST_SUITE(SecializedActivitiesSuite,SpecializedActivities)

BOOST_AUTO_TEST_CASE( testFileDescriptorActivity )
{
#if __cplusplus > 199711L
    unique_ptr<TestFDActivity>
#else
    auto_ptr<TestFDActivity>
#endif
            activity(new TestFDActivity);
    static const int USLEEP = 250000;

    int pipe_fds[2];
    int piperet;
    piperet = pipe(pipe_fds);
    BOOST_REQUIRE(piperet == 0);
    int reader = pipe_fds[0];
    int writer = pipe_fds[1];

    int other_pipe[2];
    piperet = pipe(other_pipe);
    BOOST_REQUIRE(piperet == 0);
    int other_reader = other_pipe[0];
    int other_writer = other_pipe[1];

    activity->fd = reader;
    activity->other_fd = other_reader;

    BOOST_CHECK( activity->start() );

    // Add something to watch and check that it does start
    activity->watch(reader);
    activity->watch(other_reader);
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
    BOOST_CHECK_EQUAL(base::RunnableInterface::Trigger, activity->work_reason);

    // Check timeout(). Disable reading as there won't be any data on the FD
    activity->do_read = false;
    BOOST_CHECK( activity->timeout() );
    usleep(USLEEP);
    BOOST_CHECK_EQUAL(2, activity->step_count);
    BOOST_CHECK_EQUAL(0, activity->count);
    BOOST_CHECK_EQUAL(0, activity->other_count);
    BOOST_CHECK( !activity->isRunning() && activity->isActive() );
    BOOST_CHECK_EQUAL(base::RunnableInterface::TimeOut, activity->work_reason);

    // Check normal operations. Re-enable reading.
    activity->do_read = true;
    int buffer, result;
    result = write(writer, &buffer, 2);
    BOOST_CHECK( result == 2 );
    usleep(USLEEP);
    BOOST_CHECK_EQUAL(4, activity->step_count);
    BOOST_CHECK_EQUAL(2, activity->count);
    BOOST_CHECK_EQUAL(0, activity->other_count);
    BOOST_CHECK( !activity->isRunning() && activity->isActive() );
    BOOST_CHECK_EQUAL(base::RunnableInterface::IOReady, activity->work_reason);

    result = write(other_writer, &buffer, 2);
    BOOST_CHECK( result == 2 );
    usleep(USLEEP);
    BOOST_CHECK_EQUAL(6, activity->step_count);
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
    BOOST_CHECK( activity->stop() );
    activity->setTimeout(0);

    // Test triggering while step is running
    activity->step_count = 0;
    activity->mutex.lock();
    BOOST_CHECK( activity->start() );
    activity->trigger();
    sleep(1);
    // step is blocking now
    // trigger another 65537 times
    for(std::size_t i = 0; i < 65537; ++i) activity->trigger();
    BOOST_CHECK_EQUAL(base::RunnableInterface::TimeOut, activity->work_reason);
    activity->mutex.unlock();
    sleep(1);
    BOOST_CHECK_EQUAL(2, activity->step_count);
    BOOST_CHECK( activity->stop() );
}


BOOST_AUTO_TEST_SUITE_END()


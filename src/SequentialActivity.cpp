#include "SequentialActivity.hpp"
#include "os/MainThread.hpp"
#include "os/MutexLock.hpp"

namespace RTT
{

    SequentialActivity::SequentialActivity( RunnableInterface* run /*= 0*/ )
        : ActivityInterface(run), running(false), active(false)
    {
    }

    SequentialActivity::~SequentialActivity()
    {
        stop();
    }

    Seconds SequentialActivity::getPeriod() const
    {
        return 0.0;
    }

    OS::ThreadInterface* SequentialActivity::thread()
    {
        return OS::MainThread::Instance();
    }

    bool SequentialActivity::initialize()
    {
        return true;
    }

    void SequentialActivity::step()
    {
    }

    bool SequentialActivity::breakLoop()
    {
        return false;
    }


    void SequentialActivity::finalize()
    {
    }

    bool SequentialActivity::start()
    {
        if (active == true )
            return false;

        active = true;

        if ( runner ? runner->initialize() : this->initialize() ) {
        } else {
            active = false;
        }
        return active;
    }


    bool SequentialActivity::stop()
    {
        if ( !active )
            return false;

        if (runner)
            runner->finalize();
        else
            this->finalize();
        active = false;
        return true;
    }

    bool SequentialActivity::isRunning() const
    {
        return running;
    }

    bool SequentialActivity::isPeriodic() const
    {
        return false;
    }

    bool SequentialActivity::isActive() const
    {
        return active;
    }

    bool SequentialActivity::trigger()
    {
        // this guards against recursive triggers.
        if (running)
            return false;
        if ( active ) {
            bool did_step = false;
            do {
                OS::MutexTryLock lock(execution_lock);
                if ( lock.isSuccessful() ) {
                    running = true;
                    if (runner) runner->step(); else this->step();
                    running = false;
                    did_step = true;
                } else {
                    // nop: step() is already being executed and
                    // should notice that new data is there.
                    // race: step() returns and trigger is called->
                    // trigger is ignored.
                    return true;
                }
                // mutex unlocked here.
            } // if the runner signals work again (ie a race occured),
              // rety to lock.
            while ( did_step && runner->hasWork() );
            return true;
        }
        return false;
    }

    bool SequentialActivity::execute()
    {
        return false;
    }


}

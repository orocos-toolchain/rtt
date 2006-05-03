#include "corelib/SlaveActivity.hpp"
#include "corelib/CompletionProcessor.hpp"

namespace ORO_CoreLib
{
    
    SlaveActivity::SlaveActivity( ActivityInterface* master, RunnableInterface* run /*= 0*/ )
        :mmaster(master), mperiod( master->getPeriod() ), runner(run), running(false), active(false)
    {
        if (runner)
            runner->setActivity(this);
    }

    SlaveActivity::SlaveActivity( double period, RunnableInterface* run /*= 0*/ )
        :mmaster(0), mperiod(period), runner(run), running(false), active(false)
    {
        if (runner)
            runner->setActivity(this);
    }

    SlaveActivity::SlaveActivity( RunnableInterface* run /*= 0*/ )
        :mmaster(0), mperiod(0.0), runner(run), running(false), active(false)
    {
        if (runner)
            runner->setActivity(this);
    }

    Seconds SlaveActivity::getPeriod() const
    {
        return mperiod;
    }

    EventProcessor* SlaveActivity::getEventProcessor() const
    {
        return mmaster ? mmaster->getEventProcessor() : CompletionProcessor::Instance()->getEventProcessor();
    }

    ORO_OS::ThreadInterface* SlaveActivity::thread()
    {
        // todo: a MainThread::Instance() seems more appropriate...
        return mmaster ? mmaster->thread() : CompletionProcessor::Instance()->thread();
    }

    bool SlaveActivity::initialize()
    {
        if (runner)
            return runner->initialize();
        return true;
    }

    void SlaveActivity::step()
    {
        if (runner)
            runner->step();
    }

    void SlaveActivity::loop()
    {
        if (runner)
            runner->loop();
        else
            this->step();
    }

    bool SlaveActivity::breakLoop()
    {
        if (runner)
            return runner->breakLoop();
        return false;
    }


    void SlaveActivity::finalize()
    {
        if (runner)
            runner->finalize();
    }

    bool SlaveActivity::start()
    {
        if ( (mmaster && !mmaster->isActive()) || active == true )
            return false;

        active = true;

        if ( this->initialize() ) {
            running = this->isPeriodic();
        } else {
            active = false;
        }
        return active;
    }
        

    bool SlaveActivity::stop()
    {
        if ( !active )
            return false;

        // use breakLoop if not periodic and within loop
        if ( this->isPeriodic() == false) {
            if ( running && this->breakLoop() == false )
                return false;
        }

        running = false;
        this->finalize();
        active = false;
        return true;
    }

    bool SlaveActivity::isRunning() const
    {
        return running;
    }

    bool SlaveActivity::run(RunnableInterface* _r)
    {
        if (active)
            return false;
        if (runner)
            runner->setActivity(0);
        runner = _r;
        if (runner)
            runner->setActivity(this);
        return true;
    }

    bool SlaveActivity::isPeriodic() const
    {
        return mperiod != 0.0;
    }
    bool SlaveActivity::isActive() const
    {
        return active;
    }

    bool SlaveActivity::trigger()
    {
        // non periodic case.
        if ( this->isPeriodic() == false) {
            if ( !active || running )
                return false;
            running = true;
            this->loop();
            running = false;
            return true;
        }

        if ( running )
            this->step();
        return running;
    }


}

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

    SlaveActivity::~SlaveActivity()
    {
        stop();
        if (runner)
            runner->setActivity(0);
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
        return true;
    }

    void SlaveActivity::step()
    {
    }

    void SlaveActivity::loop()
    {
        this->step();
    }

    bool SlaveActivity::breakLoop()
    {
        return false;
    }


    void SlaveActivity::finalize()
    {
    }

    bool SlaveActivity::start()
    {
        if ( (mmaster && !mmaster->isActive()) || active == true )
            return false;

        active = true;

        if ( runner ? runner->initialize() : this->initialize() ) {
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
            if ( running && (runner ? (runner->breakLoop() == false): (this->breakLoop() == false) ) )
                return false;
        }

        running = false;
        if (runner)
            runner->finalize();
        else
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
        if ( mperiod == 0.0 ) {
            if ( !active || running )
                return false;
            running = true;
            if (runner)
                runner->loop();
            else
                this->loop();
            running = false;
            return true;
        }

        if ( running )
            if (runner) runner->step(); else this->step();
        return running;
    }


}


#pragma implementation
#include "corelib/TaskEventDriven.hpp"
#include <boost/bind.hpp>


namespace ORO_CoreLib
{

    TaskEventDriven::TaskEventDriven( Event<void(void)>* _event, TaskThreadInterface* thread, RunnableInterface* _r  )
        : event(_event), runner(_r), running(false), mthread(thread)
    {}

    bool TaskEventDriven::start()
    {
        if ( !running && event )
            if ( runner ? runner->initialize() : this->initialize() )
                {
                    // handler is executed asynchronously
                    h = event->connect( boost::bind(&TaskEventDriven::handler, this), mthread );
                    return true;
                }
        return false;
    }

    bool TaskEventDriven::stop()
    {
        if ( running && event )
            {
                h.disconnect();
                if (runner)
                    runner->finalize() ;
                else 
                    this->finalize();
                return true;
            }
        else 
            return false;
    }

     bool TaskEventDriven::run(RunnableInterface* _r)
    {
        if ( running )
            return false;
        runner = _r;
        return true;
    }

    bool TaskEventDriven::setEvent( Event<void(void)>* _event)
    {
        if ( running )
            return false;
            
        event = _event;
        return true;
    }


    void TaskEventDriven::handler()
    {
        if (runner)
            runner->step() ;
        else 
            this->step();
    }
}

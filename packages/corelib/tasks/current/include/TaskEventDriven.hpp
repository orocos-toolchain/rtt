#ifndef TASK_EVENT_DRIVEN_HPP
#define TASK_EVENT_DRIVEN_HPP

#include "RunnableTaskInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * This class represents a Task which can be attached
     * to an event and execute its functionality each time
     * the event is fired. By default, only the listener
     * action triggers step(), but one can configure it to
     * do otherwise.
     */
    class TaskEventDriven
        :public RunnableTaskInterface
    {
    public:
        TaskEvenDriven( EventRegistrationInterface* _event, RunnableInterface _r = 0 )
            : onListener(true), onCompleter(false), event(_event), runner(_r), running(false),
              listener( new StandardListener(TaskEventDriven::handler, this) ),
              completer( new StandardCompleter(TaskEventDriven::completer, this) )
        {}

        virtual Seconds periodGet() { return 0; }

        bool start()
        {
            if ( !running && event )
                if ( runner ? runner->initialise() : this->initialise() )
                    {
                        event.addHandler( listener, completer );
                        return true;
                    }
            return false;
        }

        bool stop()
        {
            if ( running && event )
                {
                    event.removeHandler( listener, completer );
                    if (runner)
                        runner->initialise() ;
                    else 
                        this->initialise();
                    return true;
                }
            else 
                return false;
        }

        bool isRunning() { return running; }

        bool run(RunnableInterface* _r)
        {
            if ( running )
                return false;
            runner = _r;
            return true;
        }

        void setEvent( EventRegistrationInterface* _event)
        {
            if ( running )
                return false;
            
            event = _event;
            return true;
        }

        void runOnListener(bool doit)
        {
            onListener = doit;
        }

        void runOnCompleter(bool doit)
        {
            onCompleter = doit;
        }

    protected:

        void handler( CallBackInterface* cb )
        {
            callback = cb;
            if ( onListener )
                if (runner)
                    runner->step() ;
                else 
                    this->step();
            if ( onCompleter )
                cb->complete();
        }

        void completer()
        {
            if ( onCompleter)
                if (runner)
                    runner->step() ;
                else 
                    this->step();
        }

    private:
        bool onListener;
        bool onCompleter;
        EventRegistrationInterface* event;
        RunnableInterface*          runner;
        bool running;
        EventListenerInterface*     listener;
        EventCompleterInterface*    completer;
        CallBackInterface*          callback;


};



}


#endif

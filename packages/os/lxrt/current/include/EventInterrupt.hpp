#ifndef EVENTINTERRUPT_HPP
#define EVENTINTERRUPT_HPP


#include "EventInterruptInterface.hpp"
#include <corelib/Event.hpp>

struct rt_tasklet_struct;

namespace ORO_OS
{
    using ORO_CoreLib::Event;
    using ORO_CoreLib::CallbackInterface;
    using ORO_CoreLib::EventListenerInterface;
    using ORO_CoreLib::EventCompleterInterface;

    class ISRInterface;
    class DSRInterface;

    /**
     * An Event for interrupts which implements the 
     * EventInterruptInterface and extends Event.
     *
     */
    class EventInterrupt 
        : public Event, public EventInterruptInterface
    {
        struct rt_tasklet_struct *tasklet;
    public:
        EventInterrupt();

        virtual ~EventInterrupt();

        virtual void addHandler( ISRInterface* isr, DSRInterface * dsr);

        virtual void addHandler( EventListenerInterface* eli, EventCompleterInterface* eci);

        virtual void removeHandler( EventListenerInterface* eli, EventCompleterInterface* eci);

        virtual void removeHandler( ISRInterface* isr, DSRInterface* dsr);

        virtual int IRQRequest();

        virtual void enable();

        virtual void disable();

        virtual void IRQFree();

        virtual void complete( ISRInterface* ethi );

    protected:
        EventInterrupt( const EventInterrupt& );
        
        virtual void interrupt( int irq );

        virtual void complete( EventListenerInterface* cb);
        /**
         * An interrupt handler as RTAI Tasklet.
         */
        static void EventInterruptTasklet( unsigned long data );
    private:

        /**
         * A map where we keep the ISRs and DSRs, which are only
         * notified when a real interrupt occurs
         */
        std::map<ISRInterface*, DSRInterface*> myTable;

        /**
         * Manual enable flag.
         */
        bool enable_f;
    };

} // namespace ORO_OS

#endif

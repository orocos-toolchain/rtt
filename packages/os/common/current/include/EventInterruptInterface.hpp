#ifndef EVENTINTERRUPTINTERFACE_HPP
#define EVENTINTERRUPTINTERFACE_HPP


#include <vector>

namespace ORO_CoreLib
{
    class EventCompleterInterface;
    class EventListenerInterface;
}

namespace ORO_OS
{
    class ISRInterface;
    class DSRInterface;
    class EventInterrupt;

    using ORO_CoreLib::EventCompleterInterface;
    using ORO_CoreLib::EventListenerInterface;

    /**
     * An EventInterface for interrupts.
     * Top Half : the EventListener - ISR
     * Bottom Half : The EventCompleter - DSR
     *
     * The workings are pretty simple : 
     * * If you register a Listener,Completer pair,they will be notified, when 
     *   an interrupt occurs.
     *   
     * * If you register an ISR,DSR pair,they get notified when 
     *   an interrupt occurs
     *   
     * * When you fire(), only the Listener,Completer are notified (since this is
     *   software firing, not an IRQ)
     * 
     * @invar There is only one instance of EventInterruptInterface per IRQ
     *         foreach j,k in EventInterruptInterface,j != k : j.getIRQ() != k.getIRQ()
     */
    class EventInterruptInterface
    {

    public:
        /**
         * Default Initialisation.
         */
        EventInterruptInterface() : irq(0), initHardWare(0)
        {
            EventInterruptInterface::InstanceAdd(this);
        }

        /**
         * Virtual destructor.
         */
        virtual ~EventInterruptInterface() 
        {
            EventInterruptInterface::InstanceRemove(this);
        }

        /**
         * Set the IRQ this event reacts to
         */
        void IRQSet( unsigned int i ) { irq = i; }

        /**
         * Read the IRQ this events reacts to
         */
        unsigned int IRQGet() const { return irq; }

        /**
         * Assign a C function for hardware initialisation
         * This will override any default behaviour implemented
         * in EventInterruptInterface itself
         */
        void hardwareInitSet( void ( *init ) () ) { initHardWare = init; }

        /**
         * Register ISR and DSR like functions
         */
        virtual void addHandler( ISRInterface* isr, DSRInterface * dsr) = 0;

        virtual void addHandler( EventListenerInterface* eli, EventCompleterInterface* eci) = 0;

        virtual void removeHandler( EventListenerInterface* eli, EventCompleterInterface* eci) = 0;

        virtual void removeHandler( ISRInterface* isr, DSRInterface* dsr) = 0;

        /**
         * Request the set IRQ
         */
        virtual int IRQRequest() = 0;

        /**
         * Enable this interrupt
         */
        virtual void enable() = 0;

        /**
         * Disable this interrupt
         */
        virtual void disable() = 0;

        /**
         * Release this interrupt, as opposed to IRQrequest()
         */
        virtual void IRQFree() = 0;

    protected:
        friend class EventInterrupt;
        /**
         * An interrupt occured. Calls all handlers.
         */
        virtual void interrupt( int irq ) = 0;

        /**
         * Instructs the EventInterruptInterface to complete the ISR with
         * the appropriate registered DSR.
         */
        virtual void complete( ISRInterface* ethi ) = 0;

        /**
         * A dummy function which can get reimplemented in a derived class
         * for default initalisation of hardware.
         * Default, nothing is done.
         * This can be overriden by hardwareInitSet().
         */
        virtual void hardwareInit() {  }

        /**
         * The irq this EventInterruptInterface reacts to.
         */
        unsigned int irq;

        /**
         * A C function to be executed for hardware initialisation
         * When not null, this will override the default hardwareInit()
         */
        void ( *initHardWare ) ();

        /**
         * Get the instance of EventInterruptInterface that handles a certain irq
         */
        static EventInterruptInterface* InstanceGet( unsigned int irq );

        /**
         * Add an instance of EventInterruptInterface
         */
        static void InstanceAdd( EventInterruptInterface* );

        /**
         * Remove an instance of EventInterruptInterface
         */
        static void InstanceRemove( EventInterruptInterface* );

    private:
        /**
         * A vector of all existing EventInterruptInterface instances
         */
        static std::vector<EventInterruptInterface*> _instances;

    };

} // namespace ORO_OS

#endif

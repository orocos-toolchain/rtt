#ifndef SYNCWRITER_HPP
#define SYNCWRITER_HPP

namespace CAN
{
    /**
     * A class which writes a sync message to the bus when
     * it is run.
     */
    class SyncWriter 
        : public ORO_OS::RunnableInterface 
    {
        CANOpenBus* bus;
    public:
        Syncer( CANOpenBus* _bus) : bus(_bus) {}
        bool initialize() { return true; }
        void step() { bus->sync(); }
        void finalize() {}
    };
}

#endif

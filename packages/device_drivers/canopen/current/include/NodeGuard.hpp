#ifndef NODEGUARD_HPP
#define NODEGUARD_HPP

namespace CAN
{

    /**
     * A class which encapsulates node guarding when it runs.
     */
    class NodeGuard
        : public ORO_OS::RunnableInterface ,
          public CANListenerInterface
    {
        CANOpenBus* bus;
        int nodeId;
        CANMessage* rtr;
        unsigned int toggle;
        bool toggle_ok;
        char status;

    public:

        /**
         * Create a NodeGuard for a node on a bus.
         *
         * @param _bus The bus the node is on.
         * @param node The node number on that bus.
         */
        NodeGuard( CANOpenBus* _bus, int node ) : bus(_bus), nodeId(node), toggle(2) {}

        /**
         * Returns the status bits of the Remote Transmission Request.
         */
        unsigned int getStatus() { return status; }

        /**
         * Returns true if the node is preoperational.
         */
        bool isPreOperational() {  return status == 0x7F; }

        /**
         * Returns true if the node is operational.
         */
        bool isOperational() {  return status == 0x05; }

        /**
         * Returns true if the node is stopped.
         */
        bool isStopped() {  return status == 0x04; }

        /**
         * Returns true if the toggle flag switching
         * is valid (indicates that no messages were lost.
         */
        bool validToggle() { return toggle_ok; }


        bool initialize() 
        { 
            CANMessage guardtime; // 50 ms...
            guardtime.setStdId(0x600 + nodeId );
            CANMessage::Data d[] = { 0x22, 0x0c, 0x10, 0x00, 0x32, 0x00, 0x00, 0x00 };
            guardtime.setDataDLC( d, 8);

            CANMessage lifetime; // = 3 times
            lifetime.setStdId(0x600 + nodeId );
            CANMessage::Data dl[] = { 0x22, 0x0d, 0x10, 0x00, 0x03, 0x00, 0x00, 0x00 };
            lifetime.setDataDLC( dl, 8);

            rtr = CANMessage::createStdRemote( 0, 0x700+nodeId, 0, 0 );

            bus->addListener(this);
            bus->write( &guardtime );
            //sleep(1); // XXX
            bus->write( &lifetime );
            return true; 
        }

        void step() 
        { 
            bus->write(rtr);
        }
                                
        void finalize() {
            bus->removeListener(this);
            delete rtr;
        }
        
        void process(const CANMessage* msg)
        {
            if ( msg->getStdId() == 0x700 + nodeId)
                {
                    toggle_ok = false;
                    status = msg->getData(0) & 0x7F;
                    switch (toggle) {
                    case 0:
                        if ( msg->getData(0) >> 7 == 1) 
                            toggle_ok = true;
                        break;
                    case 1:
                        if ( msg->getData(0) >> 7 == 0)
                            toggle_ok = true;
                        break;
                    case 2:
                        toggle_ok = true;
                        break;
                    }
                    toggle = msg->getData(0) >> 7;
                }
        }
    };

}

#endif

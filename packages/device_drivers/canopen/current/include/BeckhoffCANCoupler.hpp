#ifndef BECKHOFFCANCOUPLER_HPP
#define BECKHOFFCANCOUPLER_HPP

#include "CANDeviceInterface.hpp"
#include <corelib/ConfigurationInterface.hpp>

#include "BeckhoffDOut.hpp"
#include "BeckhoffDIn.hpp"
#include "BeckhoffAOut.hpp"
#include "BeckhoffSSI.hpp"

#include <vector>

namespace Beckhoff
{
    using ORO_CoreLib::ConfigurationInterface;
    using std::vector;
    using CAN::CANDeviceInterface;
    
	/**
     * TODO : add dout functions + init of terminals.
     *        Add Analog outputs (drives)
     *        Reaction to SYNC / caching of dout/aout bits.
	 */
	class BeckhoffCANCoupler
        : public CANDeviceInterface, 
          public ConfigurationInterface
    {
        vector<CANRequest*> requests;

        public:
        BeckhoffCANCoupler(CANBusInterface* _bus, unsigned int _node_id)
            : analogOutputs(),
              digitalInputs(this),
              digitalOutputs(),
              ssiTerminals(this,_bus), 
              bus(_bus), node_id(_node_id)
        {
            requests.reserve(10);
            
            CANMessage* msg = new CANMessage(this);
            msg->setStdId(0x00);
            msg->setDLC(0x02);
            msg->setData(0,0x01);
            msg->setData(1,0x00);

            bus->write(msg);

            delete msg;

            digitalInputs.addTerminal(1,2);
            digitalInputs.addTerminal(2,2);
            digitalOutputs.addTerminal(3,2);
            digitalOutputs.addTerminal(4,2);
            analogOutputs.addTerminal(5,2);
            ssiTerminals.addTerminal(6);
            ssiTerminals.addTerminal(7);
        }

        virtual ~BeckhoffCANCoupler()
        {
        }

        void configInit()
        {
            ssiTerminals.configInit();
        }

        bool configStep()
        {
            return ssiTerminals.configStep();
        }

        void configCleanup()
        {
            ssiTerminals.configCleanup();
        }

        bool isFinished()
        {
            return ssiTerminals.isFinished();
        }

        virtual void process(const CANMessage* msg)
        {
            if ( msg->getStdId() == 0x180 + nodeId() )
                {
                    digitalInputs.update( msg );
                } 
            else
            if ( msg->getStdId() == 0x380 + nodeId() )
                {
                    ssiTerminals.update( msg );
                } 
            else
             if ( msg->getStdId() == 0x80 )
                 {
                     /*
                     // write douts on sync.
                     CANMessage msg(this, 0x200 + nodeId(), 0, 1);
                     msg.setData(0, digitalOutputs.outputStatusGet() );
                     bus->write(&msg);
                     // write aouts on sync.
                     msg.setStdId( 0x300 + nodeId() );
                     msg.setDLC(4);
                     msg.setData(0, analogOutputs.value(0) );
                     msg.setData(1, analogOutputs.value(0) >> 8 );
                     msg.setData(2, analogOutputs.value(1) );
                     msg.setData(3, analogOutputs.value(1) >> 8 );
                     bus->write(&msg);
                     */
                 }
        }

        virtual unsigned int nodeId() const { return node_id;}

        BeckhoffDOut* getDigitalOut() { return &digitalOutputs; }
        BeckhoffDIn* getDigitalIn()   { return &digitalInputs; }
        BeckhoffAOut* getAnalogOut()  { return &analogOutputs; }
        BeckhoffSSI*       getSSI()   { return &ssiTerminals;}
        
    protected:
        BeckhoffAOut analogOutputs;
        BeckhoffDIn digitalInputs;
        BeckhoffDOut digitalOutputs;
        BeckhoffSSI ssiTerminals;

        CANBusInterface* bus;
        unsigned int node_id;
    };
			
}


#endif




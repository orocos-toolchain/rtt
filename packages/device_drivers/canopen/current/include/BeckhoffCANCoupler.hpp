/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  BeckhoffCANCoupler.hpp 

                        BeckhoffCANCoupler.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
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
            digitalInputs.addTerminal(1,2);
            digitalInputs.addTerminal(2,2);
            digitalOutputs.addTerminal(3,2);
            digitalOutputs.addTerminal(4,2);
            ssiTerminals.addTerminal(5);
            ssiTerminals.addTerminal(6);
            analogOutputs.addTerminal(7,2);
            resetNode();
        }

        virtual ~BeckhoffCANCoupler()
        {
            stopNode();
        }

        void configInit()
        {
            enterPreop();
            ssiTerminals.configInit();
        }

        bool configStep()
        {
            return ssiTerminals.configStep();
        }

        void configCleanup()
        {
            ssiTerminals.configCleanup();
            //startNode();
        }

        bool isFinished() const
        {
            return ssiTerminals.isFinished();
        }

        void startNode() 
        {
            bus->addDevice(this);
            cout << "Sending StartUp..."<<endl;
            CANMessage StartUpmsg;
            StartUpmsg.setStdId(0x00);
            StartUpmsg.setDLC(0x02);
            StartUpmsg.setData(0,0x01);
            StartUpmsg.setData(1,0x00);
            bus->write(&StartUpmsg);

            /**
             * Initialisation.
             */
            bus->sync();
            writeOutputs();
            sleep(1);
        }

        void resetNode()
        {
            cout << "Sending Reset..."<<endl;
            // XXX move this to configStep and remove sleep(1)
            CANMessage Resetmsg;
            Resetmsg.setStdId(0x00);
            Resetmsg.setDLC(0x02);
            Resetmsg.setData(0,0x81);
            Resetmsg.setData(1,0x00);
            bus->write(&Resetmsg);
            sleep(2); // give bus time to reset
        }

        void stopNode()
        {
            cout << "Sending stop..."<<endl;
            CANMessage Resetmsg;
            Resetmsg.setStdId(0x00);
            Resetmsg.setDLC(0x02);
            Resetmsg.setData(0,0x02);
            Resetmsg.setData(1,0x00);
            bus->write(&Resetmsg);
            sleep(1); // give bus time to reset
        }


        void enterPreop()
        {
            bus->removeDevice(this);
            cout << "Sending Preop..."<<endl;
            // XXX move this to configStep and remove sleep(1)
            CANMessage preopmsg;
            preopmsg.setStdId(0x00);
            preopmsg.setDLC(0x02);
            preopmsg.setData(0,0x80);
            preopmsg.setData(1,0x00);
            bus->write(&preopmsg);
            sleep(1); // give bus time to reset
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
        }

        void writeOutputs()
        {
            // write douts on sync.
            CANMessage msg(this, 0x200 + nodeId(), 0, 1);
            msg.setData(0, digitalOutputs.checkSequence(0,8) );
            bus->write(&msg);
            //write aouts on sync.
            
            msg.setStdId( 0x300 + nodeId() );
            msg.setDLC(4);
            msg.setData(0, analogOutputs.value(0) );
            msg.setData(1, analogOutputs.value(0) >> 8 );
            msg.setData(2, analogOutputs.value(1) );
            msg.setData(3, analogOutputs.value(1) >> 8 );
            bus->write(&msg);
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




/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  BeckhoffCANCoupler.cxx 

                        BeckhoffCANCoupler.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 

#include "can/BeckhoffCANCoupler.hpp"

namespace Beckhoff
{
    BeckhoffCANCoupler::BeckhoffCANCoupler(CANBusInterface* _bus, unsigned int _node_id)
        : analogOutputs(),
          digitalInputs(this),
          digitalOutputs(),
          ssiTerminals(this,_bus), 
          bus(_bus), node_id(_node_id),
          hb ( HeartBeatGenerator::Instance() )
    {
        // Reset this node message :
        Resetmsg.setStdId(0x00);
        Resetmsg.setDLC(0x02);
        Resetmsg.setData(0,0x81);
        Resetmsg.setData(1, node_id);

        // Start this node message :
        StartUpmsg.setStdId(0x00);
        StartUpmsg.setDLC(0x02);
        StartUpmsg.setData(0,0x01);
        StartUpmsg.setData(1, node_id);

        // Enter stopped :
        Stopmsg.setStdId(0x00);
        Stopmsg.setDLC(0x02);
        Stopmsg.setData(0,0x02);
        Stopmsg.setData(1, node_id);

        // Enter preop :
        Preopmsg.setStdId(0x00);
        Preopmsg.setDLC(0x02);
        Preopmsg.setData(0,0x80);
        Preopmsg.setData(1, node_id);

        bus->addDevice(this);
        requests.reserve(10);
    }

    BeckhoffCANCoupler::~BeckhoffCANCoupler()
    {
        stopNode();
    }

    void BeckhoffCANCoupler::configInit()
    {
        stamp = hb->ticksGet();
        resetCount = 0;
        this->resetNode();
        ssiTerminals.configInit();
    }

    bool BeckhoffCANCoupler::configStep()
    {
        // give node time to reset ( 2 seconds )
        if ( this->status == Initialisation
             && hb->secondsSince( stamp ) > 5.0 ) {
            this->resetNode();
            stamp = hb->ticksGet();
            ++resetCount;
        }
        // if after 5 resets, still no preop, return false.
        if ( this->status == Initialisation && resetCount < 5 )
            return true;
        else if ( this->status == PreOperational )
            return ssiTerminals.configStep();
        return false;
    }

    void BeckhoffCANCoupler::configCleanup()
    {
        ssiTerminals.configCleanup();
    }

    bool BeckhoffCANCoupler::isFinished() const
    {
        return ssiTerminals.isFinished();
    }

    bool BeckhoffCANCoupler::startNode() 
    {
        if ( !(this->status == PreOperational ||
               this->status == Stopped) )
            return false;
        cout << "Sending StartUp..."<<endl;
        bus->write(&StartUpmsg);

        /**
         * Initialisation.
         */
        bus->sync();
        writeOutputs();
        this->status = Operational;
        return true;
    }

    void BeckhoffCANCoupler::resetNode()
    {
        cout << "Sending Reset..."<<endl;
        this->status = Initialisation;
        bus->write(&Resetmsg);
    }

    bool BeckhoffCANCoupler::stopNode()
    {
        if ( !(this->status == PreOperational ||
               this->status == Operational) )
            return false;
        cout << "Sending stop..."<<endl;
        bus->write(&Stopmsg);
        this->status = Stopped;
        return true;
    }

    void BeckhoffCANCoupler::process(const CANMessage* msg)
    {
        if ( msg->getStdId() == 0x180 + nodeId() )
            {
                digitalInputs.update( msg );
            } 
        else if ( msg->getStdId() == 0x380 + nodeId() )
            {
                ssiTerminals.update( msg );
            }
        else if ( msg->getStdId() == 0x700 + nodeId() )
            {
                this->status = PreOperational;
            }
    }

    void BeckhoffCANCoupler::writeOutputs()
    {
        // write douts on sync.
        CANMessage msg(this, 0x200 + nodeId(), 0, 1);
        msg.setData(0, digitalOutputs.checkSequence(0,7) );
        bus->write(&msg);
        //rtos_printf("%i\n", digitalOutputs.checkSequence(0,7));
        //write aouts on sync.
            
        msg.setStdId( 0x300 + nodeId() );
        msg.setDLC(4);
        msg.setData(0, analogOutputs.value(0) );
        msg.setData(1, analogOutputs.value(0) >> 8 );
        msg.setData(2, analogOutputs.value(1) );
        msg.setData(3, analogOutputs.value(1) >> 8 );
        bus->write(&msg);
    }

}





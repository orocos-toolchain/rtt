/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  NodeGuard.hpp 

                        NodeGuard.hpp -  description
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
 
#ifndef NODEGUARD_HPP
#define NODEGUARD_HPP

#include <corelib/RunnableInterface.hpp>
#include <can/CANConfigurator.hpp>
#include <can/CANMessage.hpp>

namespace CAN
{

    /**
     * A class which encapsulates CANOpen node guarding when it runs.
     */
    class NodeGuard
        : public ORO_CoreLib::RunnableInterface ,
          public CANListenerInterface
    {
        CANOpenBus* bus;
        unsigned int nodeId;
        CANMessage* rtr;
        unsigned int toggle;
        bool toggle_ok;
        char status;
        bool cres;
        CANConfigurator config;
    public:

        /**
         * Create a NodeGuard for a node on a bus.
         *
         * @param _bus The bus the node is on.
         * @param node The node number on that bus.
         */
        NodeGuard( CANOpenBus* _bus, CANDeviceInterface* node )
            : bus(_bus), nodeId( node->nodeId() ), toggle(2), toggle_ok(false), cres(false),
              config( _bus) {}

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

        bool configure() {
            CANMessage *guardtime = new CANMessage(); // 50 ms
            CANMessage *answer = new CANMessage(); // 50 ms
            CANMessage *lifetime  = new CANMessage(); // 3 times

            guardtime->setStdId(0x600 + nodeId );
            CANMessage::Data d[] = { 0x22, 0x0c, 0x10, 0x00, 0x32, 0x00, 0x00, 0x00 };
            guardtime->setDataDLC( d, 8);

            answer->setStdId(0x580 + nodeId );
            fillData(d,  0x60, 0x0c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 );
            answer->setDataDLC( d, 8);

            config.addRequest( new CANRequest( guardtime, answer, 0.5 ) );

            lifetime->setStdId(0x600 + nodeId );
            fillData(d, 0x22, 0x0d, 0x10, 0x00, 0x03, 0x00, 0x00, 0x00 );
            lifetime->setDataDLC( d, 8);

            answer = new CANMessage(); // 50 ms
            answer->setStdId(0x580 + nodeId );
            fillData(d, 0x60, 0x0d, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 );
            answer->setDataDLC( d, 8);

            config.addRequest( new CANRequest( lifetime, answer, 0.5 ) );

            config.configInit();

            while ( !config.isFinished() && config.configStep() )
                sleep(1);

            cres = config.isFinished();
            config.configCleanup();
            return cres;
        }

        bool initialize() 
        { 
            if (cres) {
                rtr = CANMessage::createStdRemote( 0, 0x700+nodeId, 0, 0 );
                bus->addListener(this);
            }

            return cres; 
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
            if ( msg->getStdId() == 0x700 + nodeId && ! msg->isRemote() )
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

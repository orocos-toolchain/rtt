/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  NodeGuard.hpp 

                        NodeGuard.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef NODEGUARD_HPP
#define NODEGUARD_HPP

#include <corelib/RunnableInterface.hpp>

namespace CAN
{

    /**
     * A class which encapsulates node guarding when it runs.
     */
    class NodeGuard
        : public ORO_CoreLib::RunnableInterface ,
          public CANListenerInterface
    {
        CANOpenBus* bus;
        int nodeId;
        CANMessage* rtr;
        unsigned int toggle;
        bool toggle_ok;
        char status;
        TaskInterface* _t;
    public:

        /**
         * Create a NodeGuard for a node on a bus.
         *
         * @param _bus The bus the node is on.
         * @param node The node number on that bus.
         */
        NodeGuard( CANOpenBus* _bus, int node ) : bus(_bus), nodeId(node), toggle(2), toggle_ok(false) {}

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

        virtual TaskInterface* getTask() const {
            return _t;
        }
        virtual void setTask( TaskInterface* task ) {
            _t = task;
        }

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

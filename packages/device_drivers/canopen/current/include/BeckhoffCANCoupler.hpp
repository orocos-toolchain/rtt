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
#include <rtt/ConfigurationInterface.hpp>
#include <rtt/TimeService.hpp>

#include "BeckhoffDOut.hpp"
#include "BeckhoffDIn.hpp"
#include "BeckhoffAOut.hpp"
#include "BeckhoffSSI.hpp"

#include <vector>

namespace RTT
{namespace CAN
{
    
    using std::vector;

	/**
     * An implementation of a CANDeviceInterface, which
     * represents a Beckhoff CAN Coupler.
	 */
	class BeckhoffCANCoupler
        : public CANDeviceInterface, 
          public ConfigurationInterface
    {
        vector<CANRequest*> requests;
        CANMessage Resetmsg;
        CANMessage StartUpmsg;
        CANMessage Stopmsg;
        CANMessage Preopmsg;
        TimeService::ticks stamp;
        int resetCount;
    public:
        BeckhoffCANCoupler(CANBusInterface* _bus, unsigned int _node_id);

        virtual ~BeckhoffCANCoupler();

        void configInit();

        bool configStep();

        void configCleanup();

        bool isFinished() const;

        /**
         * Send the CANOpen startup node message to the CAN bus.
         */
        bool startNode();

        /**
         * Send the CANOpen reset node message to the CAN bus.
         */
        void resetNode();

        /**
         * Send the CANOpen stop node message to the CAN bus.
         */
        bool stopNode();

        /**
         * Send the CANOpen Enter_Pre-Operational node message to the CAN bus.
         */
        bool preopNode();

        virtual void process(const CANMessage* msg);

        void writeOutputs();

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

        TimeService* hb;
    };
			
}}


#endif




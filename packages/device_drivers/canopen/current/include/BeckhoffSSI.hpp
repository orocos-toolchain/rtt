/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  BeckhoffSSI.hpp 

                        BeckhoffSSI.hpp -  description
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
 
#ifndef BECKHOFFSSI_HPP
#define BECKHOFFSSI_HPP

#include "DigitalInInterface.hpp"
#include "EncoderInterface.hpp"
#include <rtt/ConfigurationInterface.hpp>
#include "CANRequest.hpp"

// rt_std :
#include <rtt/os/rtstreams.hpp>

#include <vector>
#include <iostream>
#include <rtt/os/rtstreams.hpp>

namespace Beckhoff
{
    
    using namespace CAN;
    using std::vector;
    using namespace std;
    

	/**
     * TODO : extend so that # of terminals can be larger than 2.
	 */
	class BeckhoffSSI
        : public DigitalInInterface, 
          //public EncoderInterface, // leave out because of channel issue
          // Counter... or 
          public ConfigurationInterface
    {
        static const unsigned int SSI_TERMINALS = 2;
        int ssi_value[SSI_TERMINALS];
        int ssi_turn[SSI_TERMINALS];
        unsigned int ref[SSI_TERMINALS];
        unsigned int terminals[SSI_TERMINALS];

        vector<CANRequest*> requests;
        
        public:
        /**
         * Create an SSI Terminal with number <_nr> on CAN node <_node>
         */
        BeckhoffSSI( CANDeviceInterface* _node, CANBusInterface* _bus)
            : coupler(_node), bus(_bus), i(-1), status(false), totalTerminals(0)
        {
            requests.reserve(SSI_TERMINALS*10);
            for ( unsigned int j=0; j < SSI_TERMINALS; ++j)
                {
                    ssi_turn[j] = 0;
                    ssi_value[j] = 0;
                }
        }

        int resolution() const
        {
            return 4096;
        }


        bool upcounting() const
        {
            return true;
        }

        void update( const CANMessage* msg )
        {
            //rt_std::cout << "Update !" << rt_std::endl;
            unsigned long data = 0;
            long new_ssi_value;
            typedef unsigned long ul;
            if (msg->getDLC() == 4 || msg->getDLC() == 8)
            {
                data = ul(msg->getData(0)) + (ul(msg->getData(1)) << 8) + (ul(msg->getData(2)) <<16) + (ul(msg->getData(3)) << 24);
                new_ssi_value = ul( ( data >> 12 ) & 0xFFF );
                if ( new_ssi_value - ssi_value[0] > resolution()/2 )
                    --ssi_turn[0];
                if ( ssi_value[0] - new_ssi_value > resolution()/2 )
                    ++ssi_turn[0];
                ssi_value[0] = new_ssi_value;
                ref[0]       = ( data >> 11) & 0x1;
            }
            
            if (msg->getDLC() == 8)
            {
                data = ul(msg->getData(4)) + (ul(msg->getData(5)) << 8) + (ul(msg->getData(6)) <<16) + (ul(msg->getData(7)) << 24);
                new_ssi_value = ul( ( data >> 12 ) & 0xFFF );
                if ( new_ssi_value - ssi_value[1] > resolution()/2 )
                    --ssi_turn[1];
                if ( ssi_value[1] - new_ssi_value > resolution()/2 )
                    ++ssi_turn[1];
                ssi_value[1] = new_ssi_value;
                ref[1]       = ( data >> 11) & 0x1;
            }
        }

        void addTerminal(unsigned int _nr)
        {
            terminals[totalTerminals] = _nr;
            ++totalTerminals;
        }

        int positionGet( unsigned int channel, int& counter) const
        {
            if (channel <0 || channel > SSI_TERMINALS)
                return -1;
            counter = ssi_value[channel];
            return 0;
        }

        int positionSet( unsigned int channel, int counter)
        {
            if (channel <0 || channel > SSI_TERMINALS)
                return -1;
            ssi_value[channel] = counter;
            return 0;
        }

        int turnGet( unsigned channel, int& turn) const
        {
            if (channel <0 || channel > SSI_TERMINALS)
                return -1;
            turn = ssi_turn[channel];
            return 0;
        }
            
        int turnSet( unsigned channel, int turn)
        {
            if (channel <0 || channel > SSI_TERMINALS)
                return -1;
            ssi_turn[channel] = turn;
            return 0;
        }
            
        virtual bool isOn( unsigned int bit = 0) const
        { return ref[bit]; }

        virtual bool isOff( unsigned int bit = 0) const
        { return !ref[bit]; }

        virtual bool readBit( unsigned int bit = 0) const
        { return ref[bit]; }

        virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const
        { 
            unsigned int result = 0;
            for (unsigned int i=start_bit; i<= stop_bit; ++i)
                result +=  ref[i] << (i-start_bit);
            return result;
        }

        virtual unsigned int nbOfInputs() const
        { return totalTerminals; }
                                                            
        virtual unsigned int nbOfChannels() const
        { return totalTerminals; }
        
        virtual void configInit()
        {
            //
            // configuration of the terminals.
            // 
            for (unsigned int i=0; i < totalTerminals; ++i)
            {
                unsigned int nr = terminals[i];
                // create request 1.

                CANMessage* cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                CANMessage::Data d[8] = { 0x23, 0x01, 0x45, nr, 0x35, 0x12, 0x1f, 0x80 };
                cm_write->setDataDLC(d, 8);

                // create answer 1
                CANMessage* cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x60, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00);
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 2
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x40, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_write->setDataDLC(d, 8);

                // create answer 2
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x43, 0x01, 0x45, nr, 0x35, 0x12, 0x1f, 0x80 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 3
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x23, 0x01, 0x45, nr, 0x12, 0x00, 0x20, 0x80 ); // 0x12 was 0x08
                cm_write->setDataDLC(d, 8);

                // create answer 3
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x60, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 4
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x40, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_write->setDataDLC(d, 8);

                // create answer 4
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x43, 0x01, 0x45, nr, 0x08, 0x00, 0x20, 0x80 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 5
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x23, 0x01, 0x45, nr, 0x02, 0x00, 0x21, 0x80 );
                cm_write->setDataDLC(d, 8);

                // create answer 5
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x60, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 6
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x40, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_write->setDataDLC(d, 8);

                // create answer 6
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x43, 0x01, 0x45, nr, 0x02, 0x00, 0x21, 0x80 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 7
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x23, 0x01, 0x45, nr, 0x18, 0x00, 0x22, 0x80 );
                cm_write->setDataDLC(d, 8);

                // create answer 7
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x60, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 8
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x40, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_write->setDataDLC(d, 8);

                // create answer 8
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x43, 0x01, 0x45, nr, 0x18, 0x00, 0x22, 0x80 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 9
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x23, 0x01, 0x45, nr, 0x00, 0x12, 0x1f, 0x80 );
                cm_write->setDataDLC(d, 8);

                // create answer 9
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x60, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));

                // create request 10
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x40, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_write->setDataDLC(d, 8);

                // create answer 10
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x43, 0x01, 0x45, nr, 0x00, 0x12, 0x1f, 0x80 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));
            }
            
            /**
             * Config Transmit PDO3 (for SSI data)
             */
            // create request 1.
            CANMessage* cm_write = new CANMessage(coupler);
            cm_write->setStdId( 0x600 + coupler->nodeId() );
            //CANMessage::Data d[8] = { 0x23, 0x00, 0x18,0x01, 0x82, 0x01, 0x00, 0x00 };
            CANMessage::Data d[8] = { 0x23, 0x02, 0x18,0x01, 0x82, 0x03, 0x00, 0x00 };
            cm_write->setDataDLC(d, 8);
            
            // create answer 1
            CANMessage* cm_expect = new CANMessage(coupler);
            cm_expect->setStdId(0x580 + coupler->nodeId() );
            //fillData(d, 0x60, 0x00, 0x18,0x01, 0x00, 0x00, 0x00, 0x00 );
            fillData(d, 0x60, 0x02, 0x18,0x01, 0x00, 0x00, 0x00, 0x00 );
            cm_expect->setDataDLC(d, 8);
            
            requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));
            
            // create request 2
            cm_write = new CANMessage(coupler);
            cm_write->setStdId( 0x600 + coupler->nodeId() );
            //fillData(d, 0x2f, 0x00, 0x18,0x02, 0x01, 0x00, 0x00, 0x00 );
            fillData(d, 0x2f, 0x02, 0x18,0x02, 0x01, 0x00, 0x00, 0x00 );
            cm_write->setDataDLC(d, 8);
            
            // create answer 2
            cm_expect = new CANMessage(coupler);
            cm_expect->setStdId(0x580 + coupler->nodeId() );
            //fillData(d, 0x60, 0x00, 0x18,0x02, 0x00, 0x00, 0x00, 0x00 );
            fillData(d, 0x60, 0x02, 0x18,0x02, 0x00, 0x00, 0x00, 0x00 );
            cm_expect->setDataDLC(d, 8);
            
            requests.push_back(new CANRequest(cm_write, cm_expect, 1.0));
            
        }
            
        /**
         * A stepwise configuration of the device.
         * returns false if it fatally failed.
         */
        virtual bool configStep()
        {
            if ( i == -1 || requests[i]->isReceived() )// && requests[i]->isExactMatch() )
            {
                if (i == int(requests.size()) - 1 )
                    status = true;
                else
                    requests[++i]->sendTo(bus);
            }
            
            if ( i != -1 && requests[i]->isExpired() )
                {
                    rt_std::cout << "Request " <<i<<" expired!"<<rt_std::endl;
                    return false;
                }
            
            return true;
        }

        virtual bool isFinished() const { return status; }

        virtual void configCleanup()
        {
            for (unsigned int j=0; j<requests.size(); ++j)
                delete requests[j];
            i = -1;
            status = false;
        }

    protected:

        CANDeviceInterface* coupler;
        CANBusInterface* bus;
        /**
         * The number of the pending request
         */
        int i;
        
        /**
         * True if the config is finished.
         */
        bool status;
        
        /**
         * The number of registered terminals.
         */
        unsigned int totalTerminals;
	};
			
}


#endif



#ifndef BECKHOFFSSI_HPP
#define BECKHOFFSSI_HPP

#include <device_interface/DigitalInInterface.hpp>
#include <device_interface/EncoderAbsoluteInterface.hpp>
#include <corelib/ConfigurationInterface.hpp>
#include "CANRequest.hpp"

#include <vector>

namespace Beckhoff
{
    using namespace ORO_CoreLib;
    using namespace CAN;
    using std::vector;

	/**
     * TODO : extend so that # of terminals can be larger than 2.
	 */
	class BeckhoffSSI
        : public DigitalInInterface, 
          //public EncoderAbsoluteInterface, // leave out because of channel issue
          public ConfigurationInterface
    {
        static const unsigned int SSI_TERMINALS = 2;
        unsigned int ssi_value[SSI_TERMINALS];
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
        }

        void update( const CANMessage* msg )
        {
            unsigned long data = 0;
            if (msg->getDLC() == 4 || msg->getDLC() == 8)
            {
                 data = lower_u32(msg->v_MsgData);
                 // we get 24 bits per ssi module
                 ssi_value[0] = data & 0xFFF;
                 ref[0]       = data >> 12 & 0x1;
            }
            
            if (msg->getDLC() == 8)
            {
                data = higher_u32(msg->v_MsgData);
                // we get 24 bits per ssi module
                ssi_value[1] = data & 0xFFF;
                ref[1]       = data >> 12 & 0x1;
            }
        }

        void addTerminal(unsigned int _nr)
        {
            terminals[totalTerminals] = _nr;
            ++totalTerminals;
        }

        int read( unsigned int channel, unsigned int& counter)
        {
            if (channel <0 || channel >= SSI_TERMINALS)
                return -1;
            counter = ssi_value[channel];
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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

                // create request 3
                cm_write = new CANMessage(coupler);
                cm_write->setStdId( 0x600 + coupler->nodeId() );
                fillData(d, 0x23, 0x01, 0x45, nr, 0x08, 0x00, 0x20, 0x80 );
                cm_write->setDataDLC(d, 8);

                // create answer 3
                cm_expect = new CANMessage(coupler);
                cm_expect->setStdId(0x580 + coupler->nodeId() );
                fillData(d, 0x60, 0x01, 0x45, nr, 0x00, 0x00, 0x00, 0x00 );
                cm_expect->setDataDLC(d, 8);

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));

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

                requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));
            }
            
            /**
             * Config Transmit PDO3 (for SSI data)
             */
            // create request 1.
            CANMessage* cm_write = new CANMessage(coupler);
            cm_write->setStdId( 0x600 + coupler->nodeId() );
            CANMessage::Data d[8] = { 0x23, 0x02, 0x18,0x01, 0x82, 0x03, 0x00, 0x00 };
            cm_write->setDataDLC(d, 8);
            
            // create answer 1
            CANMessage* cm_expect = new CANMessage(coupler);
            cm_expect->setStdId(0x580 + coupler->nodeId() );
            fillData(d, 0x60, 0x02, 0x18,0x01, 0x00, 0x00, 0x00, 0x00 );
            cm_expect->setDataDLC(d, 8);
            
            requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));
            
            // create request 2
            cm_write = new CANMessage(coupler);
            cm_write->setStdId( 0x600 + coupler->nodeId() );
            fillData(d, 0x2f, 0x02, 0x18,0x02, 0x01, 0x00, 0x00, 0x00 );
            cm_write->setDataDLC(d, 8);
            
            // create answer 2
            cm_expect = new CANMessage(coupler);
            cm_expect->setStdId(0x580 + coupler->nodeId() );
            fillData(d, 0x60, 0x02, 0x18,0x02, 0x00, 0x00, 0x00, 0x00 );
            cm_expect->setDataDLC(d, 8);
            
            requests.push_back(new CANRequest(cm_write, cm_expect, 0.1));
            
        }
            
        /**
         * A stepwise configuration of the device.
         * returns false if it fatally failed.
         */
        virtual bool configStep()
        {
            if ( i == -1 || requests[i]->isReceived() && requests[i]->isExactMatch() )
            {
                if (i == int(requests.size()) - 1 )
                    status = true;
                else
                    requests[++i]->sendTo(bus);
            }
            
            if ( requests[i]->isExpired() )
                return false;
            
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



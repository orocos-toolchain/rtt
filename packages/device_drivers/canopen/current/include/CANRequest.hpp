#ifndef CANREQUEST_HPP
#define CANREQUEST_HPP

#include "CANMessage.hpp"
#include "CANBusInterface.hpp"

#include <corelib/HeartBeatGenerator.hpp>
#include <corelib/Time.hpp>
#include <os/rtstreams.hpp>

namespace CAN
{

    using namespace ORO_CoreLib;

    class CANRequest
        :public CANDeviceInterface
    {
    public:
        /**
         * Write a message to the bus and expect a repsonse with a given CobId
         * and possibly data.
         * */
        CANRequest(CANMessage* _write, CANMessage* _expect, Seconds _timeout = 0 )
            : write(_write), expected(_expect), result(0), timestamp(0), timeout(_timeout)
        {
        }

        /**
         * Sends the request to a bus and listens for the answer containing the expected cobId.
         */
        bool sendTo(CANBusInterface* _bus)
        {
            if ( bus != 0 )
                return false;

            _bus->addDevice(this);
            bus = _bus;
            result = 0;
            bus->write(write);
            timestamp = HeartBeatGenerator::Instance()->ticksGet();
            return true;
        }
 
        void process(const CANMessage* msg)
        {
            if (result != 0)
                return;
            // update status
            if ( (msg->isStandard() == expected->isStandard() &&  msg->getStdId() == expected->getStdId() )
                 || ( msg->isExtended() == expected->isExtended() && msg->getExtId() == expected->getExtId() ) )
                {
                    result_cache = *msg;
                    result = &result_cache;
                    abort();
                } else if ( isExpired() )
                    {
                        abort();
                    }
        }

        unsigned int nodeId() const
        {
            return write->origin->nodeId();
        }

        /**
         * Abort the reception of an answer.
         */
        void abort()
        {
            if (bus != 0)
                {
                    bus->removeDevice(this);
                    bus = 0;
                }
        }        
    
        /**
         * Returns true if a message with the expected CobId was received.
         */
        bool isReceived() const
        {
            return result != 0;
        }

        /**
         * Returns true if the timeout expired.
         */
        bool isExpired() const
        {
            if ( timeout == 0 )
                return false;
            else
                return HeartBeatGenerator::Instance()->secondsSince(timestamp) > timeout;
        }
    
        /**
         * Returns true if a message was received with an exact match
         * of the expected CANMessage.
         * An Exact match is when the CobId, DLC and data are equal to
         * the expected values.
         */
        bool isExactMatch() const
        {
            if (result != 0)
                return *expected == *result;
            else
                return false;
        }

        /**
         * Returns true if databyte <pos> matches the expected value.
         */
        bool matchDataByte(unsigned int pos) const
        {
            if (result != 0 && pos < expected->getDLC() && pos < result->getDLC() )
                return expected->getData(pos) == result->getData(pos);
            return false;
        }

        /**
         * Returns true if databytes <start> to <end> matches the expected value.
         * <start> and <end> should be numbers from 0 to 7,  denoting the byte
         * positions.
         */
        bool matchDataByte(unsigned int start, unsigned int end) const
        {
            if (result != 0 && start <= end && end < expected->getDLC() && end < result->getDLC() )
                {
                    for (unsigned int i=start; i <= end; ++i)
                        if ( expected->getData(i) != result->getData(i) )
                            return false;
                    return true;
                }
            return false;
        }

        /**
         * Returns the received message.
         */
        CANMessage* getReceivedMessage() { return result; }

    protected:
        CANBusInterface* bus;
        CANMessage* write;
        CANMessage* expected;
        CANMessage* result;
        CANMessage result_cache;
        HeartBeatGenerator::ticks timestamp;
        Seconds timeout;
    };

}

#endif

#ifndef CANCONFIGURATOR_HPP
#define CANCONFIGURATOR_HPP

#include <corelib/ConfigurationInterface.hpp>
#include "CANRequest.hpp"

// rt_std :
#include <os/rtstreams.hpp>

#include <vector>
#include <utility>
#include <iostream>
#include <os/rtstreams.hpp>

namespace CAN
{
    using namespace ORO_CoreLib;
    using namespace std;

	/**
     * A Generic configuration class for sending
     * multiple \a CANRequest 's on a bus and waiting
     * for each answer.
	 */
	class CANConfigurator
        : public ConfigurationInterface
    {
        std::vector< std::pair<CANRequest*, bool> > requests;
        
        public:
        /**
         * Create a CAN Device Configurator
         */
        CANConfigurator( CANBusInterface* _bus)
            : bus(_bus), status(false)
        {
        }

        /**
         * Add a CANRequest for configuration.
         * @param req It is owned by the CANConfigurator.
         * @param weakcheck Set to true for non strict checking of
         * received message. Only the COBId is checked 
         * to match the expected value in that case.
         */
        void addRequest( CANRequest* req, bool weakcheck = false){
            requests.push_back( std::make_pair(req, weakcheck) );
        }

        virtual void configInit()
        {
            it = requests.begin();
            if ( it != requests.end() )
                it->first->sendTo(bus);
        }
            
        /**
         * A stepwise configuration of the device.
         * returns false if it fatally failed.
         */
        virtual bool configStep()
        {
            if ( it == requests.end() )
                status = true;
            else if ( ( it->first->isReceived() && it->second )
                      || (! it->second && it->first->isExactMatch() ) )
                {
                    ++it;
                    if ( it != requests.end() )
                        it->first->sendTo(bus);
                }
            else if ( it->first->isExpired() )
                {
                    rt_std::cout << "Request " << ( it - requests.begin() ) <<" expired!"<<rt_std::endl;
                    return false;
                }
            
            return true;
        }

        virtual bool isFinished() const { return status; }

        virtual void configCleanup()
        {
            for (unsigned int j=0; j<requests.size(); ++j)
                delete requests[j].first;
            requests.clear();
            status = false;
        }

    protected:

        CANBusInterface* bus;

        std::vector< std::pair<CANRequest*, bool> >::iterator it;

        /**
         * True if the config is finished.
         */
        bool status;

	};
			
}


#endif



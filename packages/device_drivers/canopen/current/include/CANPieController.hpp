/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CANPieController.hpp 

                        CANPieController.hpp -  description
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
 
#ifndef CANPIECONTROLLER_HPP
#define CANPIECONTROLLER_HPP

#include "CANControllerInterface.hpp"
#include "CANBusInterface.hpp"
#include <iostream>

#include <pkgconf/system.h>
#ifdef OROPKG_CORELIB_TASKS
#include <corelib/TaskNonPreemptible.hpp>
#endif

// the functions are implemented 
// in another object file of libtarget.a
#include <can/cpuser.h>

#include <os/fosi.h>

//extern struct module __this_module;

namespace CAN
{
    using std::cout;
    using std::endl;
#ifdef OROPKG_CORELIB_TASKS
    using namespace ORO_CoreLib;
#endif
	/**
     * A Controller which interacts with the CANPie platform to
     * address the physical controller.
	 */
	class CANPieController
        : public CANControllerInterface
#ifdef OROPKG_CORELIB_TASKS
          ,public TaskNonPreemptible
#endif
    {
    public:
        /**
         * Create a CANPie Controller on channel <channel> and
         * indicate if received messages are put on the bus with
         * an interrupt or if they are polled.
         */
        CANPieController( double period,  bool interrupt = false) :
#ifdef OROPKG_CORELIB_TASKS
             TaskNonPreemptible( period ), 
#endif
             CANPieChannel(0),  process_in_int(interrupt), 
             total_recv(0), total_trns(0), failed_recv(0), failed_trns(0)
        {
        }
            
        virtual ~CANPieController()
        {
            CpUserAppDeInit(0);
            cout << "CANPie Controller Statistics :"<<endl;
            cout << " Total Received    : "<<total_recv<< ".  Failed to Receive (FIFO empty) : "<< failed_recv<<endl;
            cout << " Total Transmitted : "<<total_trns<< ".  Failed to Transmit (FIFO full) : "<< failed_trns<<
                ".  Generic Transmit Errors: "<<generic_trns<<endl<<endl;
        }

#ifdef OROPKG_CORELIB_TASKS
        bool initialise() 
        {
            return (CANPieStatus == CpErr_OK);
        }

        void step() 
        {
            while ( readFromBuffer(CANmsg) )
                {
                    CANmsg.origin = this;
                    bus->write(&CANmsg); // we own CANmsg;
                }
        }
        
        void finalise() {}
#endif

        virtual void addBus( unsigned int chan, CANBusInterface* _bus)
        {
            CANPieChannel = chan;
            bus = _bus;
            controller[CANPieChannel] = this;
            CANPieStatus = CpUserAppInit(CANPieChannel, 32, 64, 10);
            cout << "Bus status :"<<CANPieStatus<<endl;
            CpUserIntFunctions( CANPieChannel, ReceiveIntHandler, 0 , 0);
            bus->setController( this );
        }

        virtual void process(const CANMessage* msg)
        {
            writeToBuffer(*msg);
        }

        virtual unsigned int nodeId() const
        {
            return 0;
        }

        static unsigned int ReceiveIntHandler(unsigned int channel, CpStruct_CAN* msg)
        {
            return controller[int(channel)]->receive(msg);
            //return CP_CALLBACK_PUSH_FIFO; // let the object itself decide.
        }

        bool readFromBuffer( CANMessage& msg)
        {
            int status = CpErr_FIFO_EMPTY;

            if ( (status = CpUserMsgRead(CANPieChannel, &msg) ) == CpErr_OK )
                {
                    msg.origin = this;
                    ++total_recv;
                }
            else
                ++failed_recv;

            return (status == CpErr_OK);
        }

        bool writeToBuffer(const CANMessage& msg)
        {
            int status = CpUserMsgWrite(CANPieChannel, &msg);
            if (status == CpErr_OK)
                ++total_trns;
            else if (status == CpErr_FIFO_FULL)
                ++failed_trns;
            else 
                ++generic_trns;
            return (status == CpErr_OK);
        }
    protected:
        /**
         * This method is called by the ReceiveIntHandler in response to an
         * interrupt.
         */
        int receive(CpStruct_CAN* msg)
        {
            if (process_in_int)
                {
                    CANmsg = *msg;
                    CANmsg.origin = this;
                    ++total_recv;

                    bus->write(&CANmsg);

                    return CP_CALLBACK_PROCESSED;
                }
            //rt_sem_signal(&new_message);  // inform kernel thread that message is there.
            return CP_CALLBACK_PUSH_FIFO;
        }
            
        static CANPieController* controller[CP_CHANNEL_MAX];

        int CANPieStatus;

        int CANPieChannel;

        CANBusInterface *bus;

        bool process_in_int;

        CANMessage CANmsg;

        unsigned int total_recv;
        unsigned int total_trns;
        unsigned int failed_recv;
        unsigned int failed_trns;
        unsigned int generic_trns;
	};
			
}


#endif


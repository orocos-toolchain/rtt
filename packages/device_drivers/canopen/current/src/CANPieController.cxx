/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:17 CET 2005  CANPieController.cxx 

                        CANPieController.cxx -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 

#include "rtt/can/CANPieController.hpp"

#include <pkgconf/system.h>
#ifdef OROPKG_CORELIB_ACTIVITIES
#include <rtt/NonPreemptibleActivity.hpp>
#include <rtt/Logger.hpp>
#endif

#include "rtt/can/cpuser.h"
#include <rtt/os/fosi.h>

namespace RTT
{namespace CAN
{
#ifdef OROPKG_CORELIB_ACTIVITIES
    
#endif

  CANPieController::CANPieController( double period,  bool interrupt ) :
#ifdef OROPKG_CORELIB_ACTIVITIES
    NonPreemptibleActivity( period ), 
#endif
    CANPieChannel(0),  process_in_int(interrupt), 
    total_recv(0), total_trns(0), failed_recv(0), failed_trns(0)
  {
  }
            
        CANPieController::~CANPieController()
        {
            CpUserAppDeInit(0);
            Logger::log() <<Logger::Info << "CANPie Controller Statistics :"<<Logger::nl;
            Logger::log() << " Total Received    : "<<total_recv<< ".  Failed to Receive (FIFO empty) : "<< failed_recv<<Logger::nl;
            Logger::log() << " Total Transmitted : "<<total_trns<< ".  Failed to Transmit (FIFO full) : "<< failed_trns<<
                ".  Generic Transmit Errors: "<<generic_trns<<Logger::endl;
        }

        bool CANPieController::initialise() 
        {
            return (CANPieStatus == CpErr_OK);
        }

        void CANPieController::step() 
        {
            while ( readFromBuffer(CANmsg) )
                {
                    CANmsg.origin = this;
                    bus->write(&CANmsg); // we own CANmsg;
                }
        }
        
        void CANPieController::finalise() {}

        void CANPieController::addBus( unsigned int chan, CANBusInterface* _bus)
        {
            CANPieChannel = chan;
            bus = _bus;
            controller[CANPieChannel] = this;
            CANPieStatus = CpUserAppInit(CANPieChannel, 32, 64, 10);
            Logger::log() <<Logger::Info << "CANPieController : Added bus with status :"<<CANPieStatus<<Logger::endl;
            CpUserIntFunctions( CANPieChannel, ReceiveIntHandler, 0 , 0);
            bus->setController( this );
        }

        void CANPieController::process(const CANMessage* msg)
        {
            writeToBuffer(*msg);
        }

        unsigned int CANPieController::nodeId() const
        {
            return 0;
        }

        unsigned int CANPieController::ReceiveIntHandler(unsigned int channel, CpStruct_CAN* msg)
        {
            return controller[int(channel)]->receive(msg);
            //return CP_CALLBACK_PUSH_FIFO; // let the object itself decide.
        }

        bool CANPieController::readFromBuffer( CANMessage& msg)
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

        bool CANPieController::writeToBuffer(const CANMessage& msg)
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

        int CANPieController::receive(CpStruct_CAN* msg)
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
            
        CANPieController* CANPieController::controller[CP_CHANNEL_MAX];
}}





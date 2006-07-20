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
#include "CANMessage.hpp"

#include <pkgconf/system.h>
#ifdef OROPKG_CORELIB_ACTIVITIES
#include <rtt/NonPreemptibleActivity.hpp>
#endif

#include "cpuser.h"

namespace CAN
{

#ifdef OROPKG_CORELIB_ACTIVITIES
  
#endif
  /**
   * A Controller which interacts with the CANPie platform to
   * address the physical controller.
   */
  class CANPieController
    : public CANControllerInterface
#ifdef OROPKG_CORELIB_ACTIVITIES
    ,public NonPreemptibleActivity
#endif
  {
  public:
    /**
     * Create a CANPie Controller on channel \a channel and
     * indicate if received messages are put on the bus with
     * an interrupt or if they are polled.
     */
    CANPieController( double period,  bool interrupt = false);
    virtual ~CANPieController();

    bool initialise();

    void step();
        
    void finalise();

    virtual void addBus( unsigned int chan, CANBusInterface* _bus);

    virtual void process(const CANMessage* msg);

    virtual unsigned int nodeId() const;

    static unsigned int ReceiveIntHandler(unsigned int channel, CpStruct_CAN* msg);

    bool readFromBuffer( CANMessage& msg);

    bool writeToBuffer(const CANMessage& msg);
  protected:
    /**
     * This method is called by the ReceiveIntHandler in response to an
     * interrupt.
     */
    int receive(CpStruct_CAN* msg);
            
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


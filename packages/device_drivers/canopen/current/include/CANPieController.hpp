#ifndef CANPIECONTROLLER_HPP
#define CANPIECONTROLLER_HPP

#include "CANControllerInterface.hpp"
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
             CANPieChannel(0),  process_in_int(interrupt)
        {
        }
            
        virtual ~CANPieController()
        {
            CpUserAppDeInit(0);
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

            if ( (status = CpUserMsgRead(CANPieChannel, &msg) ) )
                    msg.origin = this;

            return (status == CpErr_OK);
        }

        bool writeToBuffer(const CANMessage& msg)
        {
            int status = CpUserMsgWrite(CANPieChannel, &msg);
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

	};
			
}


#endif


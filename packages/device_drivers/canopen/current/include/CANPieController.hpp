#ifndef CANPIECONTROLLER_HPP
#define CANPIECONTROLLER_HPP

#include "CANControllerInterface.hpp"

#ifdef OROPKG_CORELIB_TASKS
#include <corelib/TaskEventDriven.hpp>
#endif

// the functions are implemented 
// in another object file of libtarget.a
#include <can/cpuser.h>

#include <os/fosi.h>

//extern struct module __this_module;

namespace CAN
{
    using namespace ORO_CoreLib;
	/**
     * A Controller which interacts with the CANPie platform to
     * address the physical controller.
	 */
	class CANPieController
        : public CANControllerInterface
#ifdef OROPKG_CORELIB_TASKS
          ,public TaskEventDriven
#endif
    {
    public:
        /**
         * Create a CANPie Controller on channel <channel> and
         * indicate if received messages are put on the bus with
         * an interrupt or if they are polled.
         */
        CANPieController(int channel, CANBusInterface* _bus, bool interrupt = false) :
#ifdef OROPKG_CORELIB_TASKS
             TaskEventDriven(), 
#endif
             CANPieChannel(channel), bus(_bus), process_in_int(interrupt)
        {
            controller[channel] = this;
            CANPieStatus = CpUserAppInit(channel, 6, 6, 10);
            CpUserIntFunctions((unsigned char)channel, ReceiveIntHandler, 0 , 0);
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
                bus->write(&CANmsg); // we own CANmsg;
        }
        
        void finalise() {}
#endif

        virtual void process(const CANMessage* msg)
        {
            writeToBuffer(*msg);
        }

        virtual unsigned int nodeId() const
        {
            return 0;
        }

        static unsigned char ReceiveIntHandler(unsigned char channel, CpStruct_CAN* msg)
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


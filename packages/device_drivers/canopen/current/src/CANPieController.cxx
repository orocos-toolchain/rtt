
#include "can/CANPieController.hpp"

#include <pkgconf/system.h>
#ifdef OROPKG_CORELIB_TASKS
#include <corelib/TaskNonPreemptible.hpp>
#include <corelib/Logger.hpp>
#endif

#include "can/cpuser.h"
#include <os/fosi.h>

namespace CAN
{
#ifdef OROPKG_CORELIB_TASKS
    using namespace ORO_CoreLib;
#endif

  CANPieController::CANPieController( double period,  bool interrupt ) :
#ifdef OROPKG_CORELIB_TASKS
    TaskNonPreemptible( period ), 
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
}






#include "can/CANMessage.hpp"
#include "can/CANPieController.hpp"

namespace CAN
{
    // Static controller var for CANPieController.hpp
    CANPieController* CANPieController::controller[CP_CHANNEL_MAX];
    CANDummyDevice CANMessage::candevice_dummy;
}

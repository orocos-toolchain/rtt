#ifndef CALIBRATION_INTERFACE_HPP
#define CALIBRATION_INTERFACE_HPP

namespace ORO_DeviceDriver
{
    /**
     * @brief A class which will calibrate a sensor. It is
     * useful for making 'calibration' objects.
     */
    class CalibrationInterface
    {
    public:
        virtual ~CalibrationInterface() {}
        virtual void calibrate() = 0;
    };
}

#endif

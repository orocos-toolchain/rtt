#ifndef ORO_RTT_dev_FWD_HPP
#define ORO_RTT_dev_FWD_HPP

namespace RTT {
    namespace dev {
        class AnalogInInterface;
        class AnalogInput;
        class AnalogOutInterface;
        class AnalogOutput;
        class AxisInterface;
        class CalibrationInterface;
        class DigitalInInterface;
        class DigitalInput;
        class DigitalOutInterface;
        class DigitalOutput;
        class DriveInterface;
        class EncoderInterface;
        class HomingInterface;
        class PulseTrainGeneratorInterface;
        template < class _ValueType >
        class NameServer;
        template <class SensorData>
        class SensorInterface;
        template <class T>
        class NameServerRegistrator;
    }
    namespace detail {
        using namespace dev;
    }
}
#endif

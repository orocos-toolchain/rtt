#ifndef END_LIMIT_DETECTOR_HPP
#define END_LIMIT_DETECTOR_HPP

#include <device_interface/DigitalInInterface.hpp>
#include <device_interface/SensorInterface.hpp>

namespace ORO_DeviceDriver
{

    /**
     * @brief This class represents a virtual Digital Input device
     * with two bits denoting software end limit reaches. (e.g. end of travel)
     *
     * The first bit (0) is on when the
     * physical limit is less than the minimal limit, the
     * second bit (1) is on when the physical limit is greater
     * than the maximal limit.
     *
     * The sensor's minMeasurement() and maxMeasurement() values are
     * always taken into account.
     */
    class EndLimitDetector
        : public ORO_DeviceInterface::DigitalInInterface
    {
        ORO_DeviceInterface::SensorInterface<double>* sens;
        double minpos;
        double maxpos;
    public :
        /**
         * @brief Create a End Limit Detector with a sensor.
         *
         * @param _sensor The sensor returning a calibrated value.
         * @param _minpos The minimal limit, if omitted, the _sensor->minMeasurement() is taken.
         * @param _maxpos The maximal limit, if omitted, the _sensor->maxMeasurement() is taken.
         */
        EndLimitDetector( ORO_DeviceInterface::SensorInterface<double>* _sensor,
                             double _minpos = 1.0/0.0, double _maxpos = 1.0/0.0 )
            : sens(_sensor), minpos(_minpos), maxpos(_maxpos)
        {}

        virtual bool isOn( unsigned int bit = 0) const
        {
            double pos =  sens->readSensor();
            if ( bit == 0)
                return pos < minpos || pos < sens->minMeasurement();
            if ( bit == 1)
                return pos > maxpos || pos > sens->maxMeasurement();
            return false;
        }

        virtual bool isOff( unsigned int bit = 0) const
        {
            return !isOn(bit);
        }

        virtual bool readBit( unsigned int bit = 0) const
        {
            return isOn(bit);
        }

        virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const
        {
            if (start_bit > 1 || stop_bit > 1 )
                return 0;
            // first form the total sequence, then shift + mask
            return (( isOn(0) + isOn(1)*2 ) >> start_bit ) & (1 << (stop_bit+1)-1);
        }

        virtual unsigned int nbOfInputs() const
        { return 2; }
    };

}

#endif

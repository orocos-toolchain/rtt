#ifndef FAKEDIGITALDEVICE_HPP
#define FAKEDIGITALDEVICE_HPP

#include <device_interface/DigitalInInterface.hpp>
#include <device_interface/DigitalOutInterface.hpp>


namespace ORO_DeviceDriver
{
    using namespace ORO_DeviceInterface;

    class FakeDigitalDevice
        : public DigitalInInterface,
          public DigitalOutInterface
    {
    public:
        const static int chans = 32;
        bool channels[chans];

        FakeDigitalDevice()
            : DigitalInInterface("FakeDigitalDevice"),
              DigitalOutInterface("FakeDigitalDevice")
        {}
        
        virtual void switchOn( unsigned int n )
        {
            channels[n] = true;
        }

        virtual void switchOff( unsigned int n )
        {
            channels[n] = false;
        }

        virtual void setBit( unsigned int bit, bool value )
        {
            channels[bit] = value;
        }
                
        virtual void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value)
        {
            for (unsigned int i = start_bit; i <= stop_bit; ++i)
                channels[i] = value & ( 1<<( i - start_bit ) );
        }

        virtual bool checkBit(unsigned int n) const
        {
            return channels[n];
        }


        virtual unsigned int checkSequence( unsigned int start_bit, unsigned int stop_bit ) const
        {
            unsigned int result = 0;
            for (unsigned int i = start_bit; i <= stop_bit; ++i)
                result += (channels[i] & 1)<<i;
            return result;
        }
            
        virtual unsigned int nbOfOutputs() const
        {
            return chans;
        }

        virtual unsigned int nbOfInputs() const
        {
            return chans;
        }

        virtual bool isOn( unsigned int bit = 0) const 
        {
            return channels[bit];
        }

        virtual bool isOff( unsigned int bit = 0) const
        {
            return !channels[bit];
        }
            
        virtual bool readBit( unsigned int bit = 0) const
        {
            return channels[bit];
        }

        virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const
        {
            return checkSequence(start_bit, stop_bit);
        }
            
    };


}


#endif

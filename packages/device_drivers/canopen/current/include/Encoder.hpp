#ifndef CANOPENENCODER_HPP
#define CANOPENENCODER_HPP

#include "BeckhoffSSI.hpp"
#include <device_interface/EncoderInterface.hpp>


namespace CAN
{
    using Beckhoff::BeckhoffSSI;

    class Encoder 
        : public EncoderInterface
    {
        BeckhoffSSI* bhssi;
        unsigned int chan;
    public:
        Encoder( BeckhoffSSI* ssi, unsigned int channel ) : bhssi(ssi), chan(channel) {}

        int positionGet()
        {
            int p;
            bhssi->positionGet(chan, p);
            return p;
        }

        int turnGet()
        {
            int t;
            bhssi->turnGet(chan, t);
            return t;
        }
            
        void positionSet( int p)
        {
            bhssi->positionSet(chan, p);
        }

        void turnSet( int t )
        {
            bhssi->turnSet(chan, t);
        }

        int resolution()
        {
            return bhssi->resolution();
        }
    };
}

#endif

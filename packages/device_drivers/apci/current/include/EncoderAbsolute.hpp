/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:23:39 CEST 2002  EncoderAbsolute.hpp 

                       EncoderAbsolute.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

   ***************************************************************************
   *                                                                         *
   *   This program is free software; you can redistribute it and/or modify  *
   *   it under the terms of the GNU General Public License as published by  *
   *   the Free Software Foundation; either version 2 of the License, or     *
   *   (at your option) any later version.                                   *
   *                                                                         *
   ***************************************************************************/



#ifndef EncoderAbsolute_HPP
#define EncoderAbsolute_HPP

#include <os/cpp.hpp>

#include <utils.h>
#include <fdi/SensorInterface.hpp>
#include <fdi/EncoderAbsoluteInterface.hpp>
#include "EncoderSSI.hpp"
#include <corelib/EventPositionInterface.hpp>
#include <corelib/EventInterfaces.hpp>

// EncoderAbsolute contains 1 ssiModule, 1 ssiModule contains 3 encoders.

namespace CBDeviceDriver
{

    using namespace ORO_CoreLib;

    typedef double ReadType;

    /**
     * You can adapt/copy this helper class to contain the values
     * of your specific robot. It contains all data needed to convert
     * encoder readings to axis position values in radians.
     */
    struct EncoderDataKuka361
    {
        /**
         * Transforms an axis/encoder number to the correct SSI encoder 
         * number. Axis counts from 1 to 6, return values are 0,1,2.
         */
        static unsigned int Enc2SSI( unsigned int axis_nr )
        {
            static unsigned int map[ 6 ] = {0, 1, 2, 0, 1, 2};

            if ( axis_nr < 1 || axis_nr > 6 )
            {
                rtos_printf( "Axis conversion error\n" );
                return 0;
            }

            return map[ axis_nr - 1 ];
        }

        /**
         * Array of offsets of each axis
         */
        static const int ASOFFSET[ 6 ];

        /**
         * All conversion values are called CONVX
         */
        static const double CONV1;
        static const double CONV2;
        static const double CONV3;
        static const double CONV4;
        static const double CONV5;
        static const double CONV6;

        /**
         * The resolution of the encoders.
         */
        static const size_t ENC_RES = 4096;
        /**
         * The total number of encoders.
         */
        static const size_t NbOfEncoders = 6;
        /**
         * The total number of modules (mostly 3 encoders per module).
         */
        static const size_t NbOfModules = 2;
        /**
         * Array of all Encoder bits to radians conversions.
         */
        static const double Enc2Rad[ 6 ];
        /**
         * Mapping of raw encoder number to axis number.
         */
        static const int mapping[ 6 ]; // van raw enc --> value van as nr.
    };


    /**
     * This class uses the EncoderSSI class for reading one of three
     * absolute encoder positions of one function module. You can also
     * use it as a sensor and read the interpreted position.
     */
    template <class Conversion = EncoderDataKuka361>
    class EncoderAbsolute :
        public EncoderAbsoluteInterface,
        public SensorInterface<double>,
        public Conversion
    {
        public:
            typedef ReadType Read;

            /**
             * Construct an EncoderAbsolute located on module <_ssiModule>
             * and having encoder nr <_enc_nr> on that module.
             */
            EncoderAbsolute( EncoderSSI* _ssiModule, unsigned int _enc_nr)
                    : encoderNr( _enc_nr ), ssiModule( _ssiModule )
            { }
            
            /**
             * Construct a nameserved EncoderAbsolute located on module 
             * <_ssiModule> and having encoder nr <_enc_nr> on that module.
             */
            EncoderAbsolute( EncoderSSI* _ssiModule, unsigned int _enc_nr, const std::string& name)
                    : EncoderAbsoluteInterface( name ), encoderNr( _enc_nr ), ssiModule( _ssiModule )
            { }
            
            virtual ~EncoderAbsolute()
            {}

            virtual int readSensor( double& p )
            {
                unsigned int tus;
                static unsigned int counter;

                int encoderNr6D = encoderNr + 3 * ( ssiModule->getModuleNr() );

                ssiModule->read( encoderNr, tus );
                // debugging
                // rtos_printf("%s",bin(tus));
                p = ( double ) tus;
                p -= ASOFFSET[ encoderNr6D ];
                p *= Enc2Rad[ encoderNr6D ];
                if (counter == 200)
                {
                    rtos_printf("rad: %s\n", p);
                    counter = 0;
                }
                else 
                    counter++;
                return 0;
            }

            virtual bool readPosition( int& pos )
            {
                return (ssiModule->read( encoderNr, (unsigned int&)pos ) == 0);
            }

            virtual double minMeasurement()
            {
                int encoderNr6D = encoderNr + 3 * ( ssiModule->getModuleNr() );
                return (0 - ASOFFSET[ encoderNr6D ]) * Enc2Rad[ encoderNr6D ];
            }

            virtual double maxMeasurement()
            {
                int encoderNr6D = encoderNr + 3 * ( ssiModule->getModuleNr() );
                return (ENC_RES - ASOFFSET[ encoderNr6D ]) * Enc2Rad[ encoderNr6D ];
            }

            virtual double zeroMeasurement()
            {
                return 0;
            }
            
        protected:
            unsigned int encoderNr;
            EncoderSSI* ssiModule;
    };

}

#endif


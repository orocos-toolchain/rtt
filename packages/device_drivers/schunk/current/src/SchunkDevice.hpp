/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SchunkDevice.hpp 

                        SchunkDevice.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
 
 

#ifndef SCHUNKDEVICE_HPP
#define SCHUNKDEVICE_HPP

//#include <rtstl/rtstl.hpp>
#include <fdi/SensorInterface.hpp>
#include "SchunkData.hpp"
#include "SchunkCommunicationInterface.hpp"
#include "SchunkState.hpp"
#include "SchunkStateBRL3f.hpp"

namespace CBDeviceDriver
{

    using namespace ORO_CoreLib;

    class SchunkDevice : public SensorInterface<SchunkData>
    {

        private:
            SchunkState *state;

        protected:
            SchunkCommunicationInterface *_comm;

            //int toBinary();

        public:
            SchunkDevice( SchunkCommunicationInterface *comm );

            /**
             * Flush the inputbuffer of the SchunkCommunicationInterface.
             */
            void flushInput();

            /**
             * Get one reading from the sensor and save it in *data.
             *
             * @param data, a pointer to the SchunkData in which to save
             * the record
             */
            int getLine( SchunkData* data );

            /**
             * Set the controller to the default configuration (binary
             * mode, decimal force-/torque-values, transmit all 6 values,
             * line feed enabled, flush inputbuffer). Set the state accordingly.
             */
            void initializeSchunk();

            int read( SchunkData &data );

            friend class SchunkStateBRL3f;

    };
};


#endif // SCHUNKDEVICE


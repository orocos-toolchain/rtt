/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:23:39 CEST 2002  EncoderSSI.hpp 

                       EncoderSSI.hpp -  description
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



#ifndef EncoderSSI_HPP
#define EncoderSSI_HPP

#include <pkgconf/system.h>

#ifdef OROPKG_CORELIB_TASKS
#include <os/MutexLock.hpp>
#include <corelib/TaskNonPreemptible.hpp>
#endif

#include <pkgconf/device_drivers_apci.h>

#include <string>

namespace CBDeviceDriver
{
    /**
     * A class for reading a ssi module of the apci1710 card
     *
     * TODO : inlining some functions
     */
    class EncoderSSI
#ifdef OROPKG_CORELIB_TASKS
        : public ORO_CoreLib::TaskNonPreemptible
#endif
    {
        public:
            /**
             * Create a new SSI reader for module <mNr>
             */
            EncoderSSI( unsigned int mNr, const std::string& name );

            virtual ~EncoderSSI();

            int read( unsigned int encNr, unsigned int& position );

            inline unsigned int getModuleNr()
            {
                return moduleNr;
            }

        protected:
            virtual void step()
            {
                refresh();
            }

            void readCard( unsigned int* );

            void refresh();

            void switchBuffers();

            unsigned int moduleNr;

#ifdef OROINT_DEVICE_DRIVERS_APCI1710
            OROIMP_DEVICE_DRIVERS_APCI1710_T* dev;
#endif

            /**
             * This points to the buffer containing the last
             * read positions
             */
            unsigned int* positions;
            /**
             * This points to the buffer which will be overwritten
             * with new values
             */
            unsigned int* store;

            unsigned int buf1[ 3 ];
            unsigned int buf2[ 3 ];

#ifdef OROPKG_CORELIB_TASKS
            ORO_OS::Mutex readLock;
#endif
    };

};

#endif


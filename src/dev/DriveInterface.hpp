/***************************************************************************
  tag: FMTC  do nov 2 13:05:56 CET 2006  DriveInterface.hpp 

                        DriveInterface.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef DEVICE_DRIVERS_DRIVE_INTERFACE_HPP
#define DEVICE_DRIVERS_DRIVE_INTERFACE_HPP

#include "../Event.hpp"

namespace RTT
{
    class DigitalOutput;

    /**
     * @brief A Drive Object controls a single axis. It is responsible
     * for keeping track of its status (enabled, disabled).
     * @ingroup DeviceInterface
     */
    class DriveInterface
    {
    public:
        virtual ~DriveInterface() {}

        /**
         * Enable the drive.
         */
        virtual bool enableDrive() = 0;

        /**
         * Disable the drive.
         */
        virtual bool disableDrive() = 0;

        /**
         * Query if the drive is enabled or not.
         */
        virtual bool isEnabled() const = 0;

        /**
         * @brief Get the enable signal of the drive as a DigitalOutput.
         */
        virtual DigitalOutput* getEnable() = 0;
  
        /**
         * Add an offset to the current drive value.
         * Use this value to compensate for drift when
         * the drive is enabled.
         */
        virtual void addOffset(double addoffset) = 0;

        /**
         * Get the current offset
         */
        virtual double getOffset() = 0;

        /**
         * Sets a new velocity.
         * @param v The velocity
         * @return false if not enabled.
         */
        virtual bool driveSet( double v ) = 0;

        /**
         * Returns the current drive velocity.
         */
        virtual double driveGet() const = 0;

        /**
         * Returns the maximum drive velocity without limitations.
         */
        virtual double maxDriveGet() const = 0;

        /**
         * Returns the minimum drive velocity without limitations.
         */
        virtual double minDriveGet() const = 0;

        /**
         * Limit the velocity of the drive.
         */
        virtual void limit(double lower, double higher, const Event<void(std::string)>& ev) = 0;
    };

}

#endif

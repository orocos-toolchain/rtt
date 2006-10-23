
#ifndef DEVICE_DRIVERS_DRIVE_INTERFACE_HPP
#define DEVICE_DRIVERS_DRIVE_INTERFACE_HPP

#include "Event.hpp"

namespace RTT
{
    class DigitalOutput;

    /**
     * @brief A Drive Object controls a single axis. It is responsible
     * for keeping track of its status (enabled, disabled).
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

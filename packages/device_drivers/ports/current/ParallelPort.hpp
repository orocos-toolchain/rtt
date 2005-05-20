
#ifndef ORO_PARALLELPORT_HPP
#define ORO_PARALLELPORT_HPP

#include "pkgconf/device_drivers_ports.h"
#include <device_drivers/TemplateDigitalOut.hpp>
#include <device_interface/DigitalOutInterface.hpp>

namespace ORO_DeviceDriver
{

    /**
     * A Parallel port driver using Linux ioperm/outb.  It is
     * thread-safe, but can handle only 1 parallel port per instance.
     * You need to
     * instantiate this object in every thread where you want to
     * access the port. If the threads write to different pins, they
     * are guaranteed not to clear out each others data.
     */
    class ParallelPort
        : public ORO_DeviceInterface::DigitalOutInterface
    {
        static TemplateDigitalOut Base;

        /**
         * Write out bit_status to parport.
         */
        void update() const;

        /**
         * Read in bit_status of parport.
         */
        void refresh();

        unsigned short int address_;
    public:
        /**
         * Create a new Parallelport with a base address.
         * It can be used by the Orocos Thread system to output task
         * scheduling block-sequences. You must try/catch this constructor.
         *
         * @param address The base address of the parallel port
         * @throw exception if operation not allowed.
         */
        ParallelPort( unsigned short int address = ORONUM_DEVICE_DRIVERS_PORTS_PARPORT );

        /**
         * Create a new Parallelport with a given name and base address.
         * It can be used by the Orocos Thread system to output task
         * scheduling block-sequences. You must try/catch this constructor.
         *
         * @param address The base address of the parallel port
         * @param name The name of this instance
         * @throw exception if operation not allowed.
         */
        ParallelPort( const std::string& name, unsigned short int address = ORONUM_DEVICE_DRIVERS_PORTS_PARPORT );

        ~ParallelPort();

        void switchOn( unsigned int bit);
        void switchOff( unsigned int bit);
        void setBit( unsigned int bit, bool value);
        void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value);
        bool checkBit( unsigned int bit) const;
        unsigned int checkSequence (unsigned int start_bit, unsigned int stop_bit) const;
        unsigned int nbOfOutputs() const;
    };

};

#endif

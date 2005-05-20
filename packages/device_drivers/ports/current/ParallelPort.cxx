#include <device_drivers/ParallelPort.hpp>
#include <stdlib.h>
#include <sys/io.h>

namespace ORO_DeviceDriver
{
    namespace detail {
        struct IOPerm_not_allowed {};
    }

    TemplateDigitalOut ParallelPort::Base;

    ParallelPort::ParallelPort( const std::string& name, unsigned short int address)
        : ORO_DeviceInterface::DigitalOutInterface( name ), address_( address )
    {
        if ( ioperm(address_, 3, 1)) {   
            // ask for permissions to access the parallel port
            // YOU MUST BE ROOT TO ASK FOR THIS SORT OF PERMISSION
            // usage of ioperm: address_ ... base address we want to use
            //              3 ... the next 3 bytes
            //              1 ... we want the permission
            perror("ParallelPort : ioperm open");
            throw detail::IOPerm_not_allowed();
        }

        outb(0, address_ + 2); // set to output mode
        //this->refresh();
        this->update();
    }

    ParallelPort::ParallelPort( unsigned short int address )
        : address_( address )
    {
        if ( ioperm(address_, 3, 1)) {   
            // ask for permissions to access the parallel port
            // YOU MUST BE ROOT TO ASK FOR THIS SORT OF PERMISSION
            // usage of ioperm: address_ ... base address we want to use
            //              3 ... the next 3 bytes
            //              1 ... we want the permission
            perror("ParallelPort : ioperm open");
            throw detail::IOPerm_not_allowed();
        }

        outb(0, address_ + 2); // set to output mode
        //this->refresh();
        this->update();
    }

    ParallelPort::~ParallelPort()
    {
        if (ioperm(address_, 3, 0)) { 
            // tell the system we do not use the port anymore
            // usage of ioperm: address_ ... base address we want to use
            //                                      3 ... the next 3 bytes
            //                                      0 ... free the permission       
            perror("ParallelPort : ioperm close");
        }
    }

    void ParallelPort::refresh()
    {
        unsigned char status = inb(address_);
        Base.setSequence(0, 7, status);
    }

    void ParallelPort::update() const
    {
        unsigned char out = Base.getBitStatus();
        outb(out, address_);
    }

     void ParallelPort::switchOn( unsigned int bit)
    {
        Base.switchOn(bit);
        this->update();
    }

    void ParallelPort::switchOff( unsigned int bit)
    {
        Base.switchOff(bit);
        this->update();
    }

    void ParallelPort::setBit( unsigned int bit, bool value)
    {
        Base.setBit(bit,value);
        this->update();
    }


    void ParallelPort::setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value)
    {
        Base.setSequence(start_bit, stop_bit, value);
        this->update();
    }

    bool ParallelPort::checkBit( unsigned int bit) const
    {
        return Base.checkBit(bit);
    }

    unsigned int ParallelPort::checkSequence (unsigned int start_bit, unsigned int stop_bit) const
    {
        return Base.checkSequence(start_bit, stop_bit);
    }

    unsigned int ParallelPort::nbOfOutputs() const {
        return 8;
    }
}

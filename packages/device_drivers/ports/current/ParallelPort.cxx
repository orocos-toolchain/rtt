/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  ParallelPort.cxx 

                        ParallelPort.cxx -  description
                           -------------------
    begin                : Sat May 21 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#include <rtt/dev/ParallelPort.hpp>
#include <stdlib.h>
#include <sys/io.h>

namespace RTT
{
    namespace detail {
        struct IOPerm_not_allowed {};
    }

    TemplateDigitalOut ParallelPort::Base;

    ParallelPort::ParallelPort( const std::string& name, unsigned short int address)
        : DigitalOutInterface( name ), address_( address )
    {
        if ( ioperm(address_, 3, 1)) {   
            // ask for permissions to access the parallel port
            // YOU MUST BE ROOT TO ASK FOR THIS SORT OF PERMISSION
            // usage of ioperm: address_ ... base address we want to use
            //              3 ... the next 3 bytes
            //              1 ... we want the permission
            perror("ParallelPort : ioperm open");
#ifndef ORO_EMBEDDED
            throw detail::IOPerm_not_allowed();
#else 
            return;
#endif
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
#ifndef ORO_EMBEDDED
            throw detail::IOPerm_not_allowed();
#else 
            return;
#endif
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

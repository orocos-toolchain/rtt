/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ProgramExceptions.hpp 

                        ProgramExceptions.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef ORO_PROGRAM_EXCEPTIONS_HPP
#define ORO_PROGRAM_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace RTT
{
    /**
     * This exception is thrown when a program or state machine
     * could not be loaded into a ProgramProcessor or StateMachineProcessor.
     */
    class program_load_exception
        : public std::exception
    {
        std::string merror;
    public:
        program_load_exception( const std::string& error )
            : merror( error )
        {
        }
        ~program_load_exception() throw() {}
        const char* what() const throw()
        {
            return merror.c_str();
        }
    };

    /**
     * This exception is thrown when a program or state machine
     * could not be unloaded into a ProgramProcessor or StateMachineProcessor.
     */
    class program_unload_exception
        : public std::exception
    {
        std::string merror;
    public:
        program_unload_exception( const std::string& error )
            : merror( error )
        {
        }
        ~program_unload_exception() throw() {}
        const char* what() const throw()
        {
            return merror.c_str();
        }
    };
}
#endif

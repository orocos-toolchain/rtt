/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProgramInterface.hpp 

                        ProgramInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef PROGRAMINTERFACE_HPP
#define	PROGRAMINTERFACE_HPP

#include <string>

namespace ORO_Execution
{
	
	/**
	 * @brief This abstract class represents a collection of 
	 * instructions that can be executed.
	 */
	class ProgramInterface
	{
    public:
        virtual ~ProgramInterface()
        {}

        /**
         * Execute the next logical step of this program interface.
         */
        virtual void execute()= 0;

        /**
         * Reset the execution point to the beginning of this program interface.
         */
        virtual void reset() = 0;
	
        /**
         * Return the current 'line number' of the program.
         */
        virtual int getLineNumber() const = 0;

        /**
         * Return the program text to which \a getLineNumber()
         * refers.
         */
        virtual std::string getText() const = 0;

        /**
         * Programs can be refered to by name.
         */
        virtual const std::string& getName() const = 0;
	};


}


#endif



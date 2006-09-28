/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  BeckhoffModuleInterface.hpp 

                        BeckhoffModuleInterface.hpp -  description
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
 
#ifndef BECKHOFFTERMINALINTERFACE_HPP
#define BECKHOFFTERMINALINTERFACE_HPP


namespace RTT
{namespace CAN
{
    
	/**
	 */
	struct BeckhoffTerminalInterface 
    {
        virtual ~BeckhoffTerminalInterface() {}

        virtual void initialize() = 0;

        virtual unsigned int terminalNr() = 0;
        
	};
			
}}


#endif


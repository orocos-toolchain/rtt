/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProgramInterface.hpp 

                        ProgramInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef PROGRAMINTERFACE_HPP
#define	PROGRAMINTERFACE_HPP


namespace ORO_Execution
{
	
	/**
	 * This abstract class represents a collection of 
	 * data that can be executed.
	 */
	class ProgramInterface
	{
		public:
		
			ProgramInterface()
			{}
	
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
	
	};


}


#endif



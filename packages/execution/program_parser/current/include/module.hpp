/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  module.hpp 

                        module.hpp -  description
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
 
 


#ifndef MODULE_HPP
#define MODULE_HPP


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * This function contains all the code to startup the program.
     */
    void initCpp(void);

    /**
     * This function contains all the code to cleanup the program.
     */
    void cleanupCpp(void);


#ifdef __cplusplus
}
#endif



#endif

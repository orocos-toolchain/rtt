/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  CallbackInterface.hpp 

                        CallbackInterface.hpp -  description
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
 
 
#ifndef CALLBACKINTERFACE_HPP
#define CALLBACKINTERFACE_HPP

namespace ORO_CoreLib
{
    /**
     * @brief Generic callback interface.
     *
     * A CallbackInterface allows you to signal an instance
     * that something has to be completed. This can be an Event,
     * or any other mechanism.
     */
    struct CallbackInterface
    {
        /**
         * Execute callback functionality.
         * @note This method is not const, so can not be called on 
         *       a const pointer to CallbackInterface.
         */
        virtual void complete() = 0;
    };


}


#endif

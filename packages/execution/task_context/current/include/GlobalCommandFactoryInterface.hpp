/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:41:00 CEST 2004  GlobalCommandFactoryInterface.hpp 

                        GlobalCommandFactoryInterface.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef GLOBALCOMMANDFACTORYINTERFACE_HPP
#define GLOBALCOMMANDFACTORYINTERFACE_HPP

#include "CommandFactoryInterface.hpp"

namespace ORO_Execution
	{

    /**
     * @brief This interface describes a global command factory.
     *
     * This factory has the collection of all CommandFactories.
     */
    class GlobalCommandFactoryInterface
    {
    public:
        virtual ~GlobalCommandFactoryInterface() {}

      /**
       *  Check whether the factory registered with a given object,
       *  contains a given command.
       *
       * @param objectname a registered object
       * @param	com a command
       */
      virtual bool hasCommand(const std::string& objectname,const std::string& com) = 0;

      /**
       * Search for an object and return its factory.
       * @param objectname name of the object.
       * @return the requested factory, or 0, indicating no factory
       * has been registered under that name..
       */
      virtual const CommandFactoryInterface* getObjectFactory(
        const std::string& objectname ) const = 0;
    };
}

#endif

/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  WriteNonBlockingInterface.hpp 

                        WriteNonBlockingInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
#ifndef WRITENONBLOCKINGINTERFACE_HPP
#define WRITENONBLOCKINGINTERFACE_HPP

#include "WriteInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * This interface describes a non blocking write interface for byte data.
     * It 's implementation must guarantee that execution of any member of this
     * class doesn't sleep.
     */

    class WriteNonBlockingInterface : public WriteInterface
    {

        public:
            virtual ~WriteNonBlockingInterface()
            {}

    }

    ;

}

#endif // WRITENONBLOCKINGINTERFACE_HPP



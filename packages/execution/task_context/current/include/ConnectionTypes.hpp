/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  ConnectionTypes.hpp 

                        ConnectionTypes.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_EXECUTION_CONNECTION_TYPE_HPP
#define ORO_EXECUTION_CONNECTION_TYPE_HPP

namespace ORO_Execution
{

    /**
     * Enumerates all the types of connections supported by
     * this factory.
     */
    struct ConnectionTypes
    {
        enum ConnectionType {
            locked, /** Handle concurrency on basis of a Mutex. */
            lockfree /** Handle concurrency on basis of a LockFree algorithm. */
        };
    };

}

#endif

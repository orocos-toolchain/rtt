/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:48 CET 2006  TaskContextProxy.hpp

                        TaskContextProxy.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef ORO_CORBA_CONTROLTASK_FACTORY_HPP
#define ORO_CORBA_CONTROLTASK_FACTORY_HPP


#include "../../TaskContext.hpp"

namespace RTT
{namespace corba
{
    /**
     * Use this class to create Corba TaskContext proxies, without including any
     * CORBA header. This allows you to build applications that don't link with
     * a CORBA library.
     */
    class TaskContextFactory
    {
    public:
        /**
         * Invoke this method once to initialise the Orb which will
         * run the task servers.
         * @param orb_timeout timeout value for each remote call, expressed in seconds.
         * The resolution is up to 100 nano seconds. Anything smaller will be interpreted
         * as a zero.
         */
        static bool InitOrb(int argc, char* argv[], Seconds orb_timeout=0 );

        /**
         * Invoke this method once to cleanup the orb.
         */
        static void DestroyOrb();

        /**
         * Factory method: create a CORBA Proxy for an existing TaskContextServer.
         * @param name The name of the TaskContextServer to connect to or the Object Reference of the object to connect to
         * @param is_ior set to \a true if \a name is an IOR. Defaults to false.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA proxy for \a name.
         */
        static TaskContext* Create(std::string name, bool is_ior = false);

        /**
         * Factory method: create a CORBA Proxy for an existing TaskContextServer.
         * @param filename A file containing an IOR which refers to the existing TaskContextServer.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA proxy for \a filename.
         */
        static TaskContext* CreateFromFile(std::string filename);

     };
}}
#endif

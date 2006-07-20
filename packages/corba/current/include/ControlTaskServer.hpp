/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:48 CET 2006  ControlTaskServer.hpp 

                        ControlTaskServer.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
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
 
 

#include <map>
#include <tao/corba.h>
#include "TaskContext.hpp"
#include "ApplicationServer.hpp"
#include "ControlTaskS.h"

namespace Corba
{
    /**
     * This class manages the creation of TaskContext Corba Servers
     * and a Corba Object Request Broker (Orb) which executes the servers.
     */
    class ControlTaskServer
        : public ApplicationServer
    {
    protected:
        typedef std::map<TaskContext*, ControlTaskServer*> ServerMap;
        static ServerMap servers;

        /**
         * Private constructor which creates a new servant.
         */
        ControlTaskServer(TaskContext* taskcontext, bool use_naming);

        Orocos::ControlTask_var mtask;
        TaskContext* mtaskcontext;
        bool muse_naming;

      /**
       * When a ControlTaskServer is destroyed, the object reference
       * is removed from the Naming Service and the servant is deleted.
       */
      ~ControlTaskServer();

    public:

        /**
         * Invoke this method once to initialise the Orb which will
         * run the task servers.
         */
        static bool InitOrb(int argc, char* argv[] );

        /**
         * Invoke this method once to shutdown the Orb which is
         * running the task servers in RunOrb(). When this function
	 * returns, no CORBA invocations are in progress, unless wait_for_completion is false.
         */
        static void ShutdownOrb(bool wait_for_completion = true);

      /**
       * Destroys all ControlTaskServer objects.
       */
      static void CleanupServers();

        /**
         * Invoke this method to run the orb and accept client requests.
         * Use ShutdownOrb() to break out of this method.
         */
        static void RunOrb();

        /**
         * Invoke this method once to cleanup the orb.
         */
        static void DestroyOrb();

        /**
         * Factory method: create a CORBA server for an existing TaskContext.
         * @param tc The TaskContext to serve.
         * @param use_naming Set to \a false in order not to use the Corba Naming Service.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA server for \a tc.
         */
        static ControlTaskServer* Create(TaskContext* tc, bool use_naming = true);

        /**
         * Get the Corba Object of this ControlTask.
         * This object universally identifies the remote ControlTaskServer
         * and can be used to tell other (remote) objects where to find it.
         */
        Orocos::ControlTask_ptr server() const;
    };
}

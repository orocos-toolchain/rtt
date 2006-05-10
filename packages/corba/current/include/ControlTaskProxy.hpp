/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:48 CET 2006  ControlTaskProxy.hpp 

                        ControlTaskProxy.hpp -  description
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
 
 

#include <tao/corba.h>
#include <exception>
#include <execution/TaskContext.hpp>
#include <string>
#include <map>
#include "ControlTaskC.h"
#include <orbsvcs/CosPropertyServiceC.h>
#include "ApplicationServer.hpp"

namespace ORO_Corba
{
    /**
     * Thrown if a server does not exist or has the wrong type.
     */
    struct IllegalServer 
        : public std::exception
    {
        std::string reason;
        IllegalServer();
        ~IllegalServer() throw();
        const char* what() const throw();
    };

    /**
     * This class manages the access of remote TaskContext Corba Servers
     * and a Corba Object Request Broker (Orb) which connects to these 
     * servers.
     */
    class ControlTaskProxy
        : public ORO_Execution::TaskContext,
          public ApplicationServer
    {
    protected:
        static std::map<Orocos::ControlTask_ptr, ControlTaskProxy*> proxies;

        /**
         * Private constructor which creates a new connection to
         * a stringified ior or taskname in NameServer.
         */
        ControlTaskProxy(std::string location, bool is_ior);

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        ControlTaskProxy( ::Orocos::ControlTask_ptr t );

        void synchronize();

        Orocos::ControlTask_var mtask;

    public:
        ~ControlTaskProxy();

        /**
         * Invoke this method once to initialise the Orb which will
         * connect to the task servers.
         */
        static bool InitOrb(int argc, char* argv[] );

        /**
         * Invoke this method once to cleanup the orb.
         */
        static void DestroyOrb();

        /**
         * Factory method: create a CORBA connection to an existing ControlTaskServer.
         * @param name The name of the ControlTaskServer to connect to or the Object Reference of the object to connect to
         * @param is_ior set to \a true if \a name is an IOR. Defaults to false.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA proxy for \a name.
         */
        static ControlTaskProxy* Create(std::string name, bool is_ior = false);

        /**
         * Factory method: create a CORBA connection to an existing ControlTaskServer.
         * @param task The Object to connect to.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA proxy for \a task.
         */
        static ControlTaskProxy* Create(::Orocos::ControlTask_ptr task);

        /**
         * Get the Corba Object of the ControlTask.
         * This object universally identifies the remote ControlTaskServer
         * and can be used to tell other (remote) objects where to find it.
         */
        Orocos::ControlTask_ptr server() const;

        virtual bool executeCommand( ORO_CoreLib::CommandInterface* c);

        virtual int queueCommand( ORO_CoreLib::CommandInterface* c);

        virtual const std::string& getName();

        virtual void setName(const std::string& n);

        virtual bool addPeer( TaskContext* peer, std::string alias = "" );

        virtual void removePeer( const std::string& name );

        virtual bool connectPeers( TaskContext* peer );

        virtual void disconnectPeers( const std::string& name );

        virtual PeerList getPeerList() const;

        virtual bool hasPeer( const std::string& peer_name ) const;

        virtual TaskContext* getPeer(const std::string& peer_name ) const;

        /**
         * Returns the properties of this Task as a PropertySet.
         */
        CosPropertyService::PropertySet_ptr propertySet();

    };
}

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
 
#ifndef ORO_CORBA_CONTROLTASK_PROXY_HPP 
#define ORO_CORBA_CONTROLTASK_PROXY_HPP 
 

#include <tao/corba.h>
#include <exception>
#include "../TaskContext.hpp"
#include <string>
#include <map>
#include "ControlTaskC.h"
#include <orbsvcs/CosPropertyServiceC.h>
#include "ApplicationServer.hpp"

namespace RTT
{namespace Corba
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
        : public TaskContext,
          public ApplicationServer
    {
    public:
        typedef std::map<Corba::ControlTask_ptr, ControlTaskProxy*> PMap;
        static PMap proxies;

    protected:
        /**
         * Private constructor which creates a new connection to
         * a stringified ior or taskname in NameServer.
         */
        ControlTaskProxy(std::string location, bool is_ior);

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        ControlTaskProxy( ::RTT::Corba::ControlTask_ptr t );

        void synchronize();

        Corba::ControlTask_var mtask;

        /**
         * For now one POA handles all proxies.
         */
        static PortableServer::POA_var proxy_poa;
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
        static ControlTaskProxy* Create(::RTT::Corba::ControlTask_ptr task);

        /**
         * Get the Corba Object of the ControlTask.
         * This object universally identifies the remote ControlTaskServer
         * and can be used to tell other (remote) objects where to find it.
         */
        Corba::ControlTask_ptr server() const;

        virtual bool executeCommand( CommandInterface* c);

        virtual int queueCommand( CommandInterface* c);

        virtual void setName(const std::string& n);

        virtual bool addPeer( TaskContext* peer, std::string alias = "" );

        virtual void removePeer( const std::string& name );

        virtual void removePeer( TaskContext* peer );

        virtual bool connectPeers( TaskContext* peer );

        virtual void disconnectPeers( const std::string& name );

        virtual PeerList getPeerList() const;

        virtual bool hasPeer( const std::string& peer_name ) const;

        virtual TaskContext* getPeer(const std::string& peer_name ) const;

        /**
         * Returns the properties of this Task as a PropertySet.
         */
        CosPropertyService::PropertySet_ptr propertySet();

        /**
         * Returns the default POA for all proxies.
         */
        static PortableServer::POA_ptr ProxyPOA();
    };
}}
#endif

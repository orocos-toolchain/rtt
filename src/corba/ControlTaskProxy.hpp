/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:48 CET 2006  ControlTaskProxy.hpp

                        ControlTaskProxy.hpp -  description
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

#ifndef ORO_CORBA_CONTROLTASK_PROXY_HPP
#define ORO_CORBA_CONTROLTASK_PROXY_HPP


#include "../rtt-config.h"
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/corba.h>
#else
#include <omniORB4/CORBA.h>
#endif

#include <exception>
#include "../TaskContext.hpp"
#include <string>
#include <map>
#include "ControlTaskC.h"
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
        IllegalServer(const std::string& reason);
        ~IllegalServer() throw();
        const char* what() const throw();
    };

    /**
     * This class manages the access of remote TaskContext Corba Servers
     * and a Corba Object Request Broker (Orb) which connects to these
     * servers.
     * @ingroup CompIDL
     */
    class RTT_CORBA_API ControlTaskProxy
        : public TaskContext,
          public ApplicationServer
    {
    public:
        typedef std::map<ControlTaskProxy*, Corba::ControlTask_ptr> PMap;
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

        void synchronizeOnce();
        void synchronize();

        mutable Corba::ControlTask_var mtask;

        /**
         * For now one POA handles all proxies.
         */
        static PortableServer::POA_var proxy_poa;

        void fetchObjects(OperationInterface* parent, ControlObject_ptr mtask);
    public:
        ~ControlTaskProxy();

        /**
         * Invoke this method once to cleanup the orb.
         */
        static void DestroyOrb();

        /**
         * Factory method: create a CORBA Proxy for an existing ControlTaskServer.
         * @param name The name of the ControlTaskServer to connect to or the Object Reference of the object to connect to
         * @param is_ior set to \a true if \a name is an IOR. Defaults to false.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA proxy for \a name.
         */
        static ControlTaskProxy* Create(std::string name, bool is_ior = false);

        /**
         * Factory method: create a CORBA Proxy for an existing ControlTaskServer.
         * This method may in fact return the real TaskContext in case the servant
         * of \a task is in the same process.
         * @param task The Object to create a proxy for.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA proxy for \a task, or the TaskContext
         * itself.
         */
        static TaskContext* Create(::RTT::Corba::ControlTask_ptr task, bool force_remote = false);

        /**
         * Get the Corba Object of the ControlTask.
         * This object universally identifies the remote ControlTaskServer
         * and can be used to tell other (remote) objects where to find it.
         */
        Corba::ControlTask_ptr server() const;

        virtual bool activate();

        virtual bool start();

        virtual bool stop();

        virtual bool isRunning() const;

        virtual bool configure();

        virtual bool cleanup();

        virtual bool resetError();

        virtual bool isActive() const;

        virtual bool isConfigured() const;

        virtual bool inFatalError() const;

        virtual bool inRunTimeWarning() const;

        virtual bool inRunTimeError() const;

        virtual int getErrorCount() const;

        virtual int getWarningCount() const;

        virtual TaskState getTaskState() const;

        virtual void setName(const std::string& n);

        virtual bool addPeer( TaskContext* peer, std::string alias = "" );

        virtual void removePeer( const std::string& name );

        virtual void removePeer( TaskContext* peer );

        virtual bool connectPeers( TaskContext* peer );

        virtual void disconnectPeers( const std::string& name );

        virtual PeerList getPeerList() const;

        virtual bool hasPeer( const std::string& peer_name ) const;

        virtual TaskContext* getPeer(const std::string& peer_name ) const;

        virtual bool connectPorts( TaskContext* peer );

        virtual bool ready();
        /**
         * Returns the default POA for all proxies.
         */
        static PortableServer::POA_ptr ProxyPOA();
    };
}}
#endif

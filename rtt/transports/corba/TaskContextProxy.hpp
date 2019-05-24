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

#ifndef ORO_CORBA_CONTROLTASK_PROXY_HPP
#define ORO_CORBA_CONTROLTASK_PROXY_HPP


#ifndef _REENTRANT
#define _REENTRANT
#endif
#include "corba.h"
#ifdef CORBA_IS_TAO
#include "corba.h"
#else
#include <omniORB4/CORBA.h>
#endif

#include <exception>
#include "../../TaskContext.hpp"
#include <string>
#include <map>
#include "TaskContextC.h"
#include "ApplicationServer.hpp"
#include <list>

namespace RTT
{namespace corba
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
    class RTT_CORBA_API TaskContextProxy
        : public TaskContext,
          public ApplicationServer
    {
    public:
        typedef std::map<TaskContextProxy*, corba::CTaskContext_ptr> PMap;
        static PMap proxies;

    protected:
        /**
         * Private constructor which creates a new connection to
         * a stringified ior or taskname in NameServer.
         */
        TaskContextProxy(std::string location, bool is_ior);

        /**
         * A Private constructor which does nothing
         * */
        TaskContextProxy();

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        TaskContextProxy( ::RTT::corba::CTaskContext_ptr t );

        /** CDataFlowInterface does not delete ports automatically, because they
         * can then be defined as members of the TaskContext classes.
         *
         * We must therefore delete in the proxy destructor the ones we are
         * adding through synchronization
         */
        std::list<base::PortInterface*> port_proxies;

        /**
         * initializes the class from a stringified ior or taskname in NameServer.
         * */
        void initFromURIOrTaskname(std::string location, bool is_ior);

        void synchronize();

        mutable corba::CTaskContext_var mtask;

        /**
         * For now one POA handles all proxies.
         */
        static PortableServer::POA_var proxy_poa;

        void synchronizeRequesters(ServiceRequester::shared_ptr parent, CServiceRequester_ptr csrq, const CServiceRequesterDescription & cdescription);
        void synchronizeServices(Service::shared_ptr parent, CService_ptr serv, const CServiceDescription & cdescription);
        void synchronizePorts(Service::shared_ptr parent, CDataFlowInterface_ptr dfact, const CServiceDescription & cdescription);

    public:
        ~TaskContextProxy();

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
        static TaskContextProxy* Create(std::string name, bool is_ior = false);

        /**
         * Factory method: create a CORBA Proxy for an existing TaskContextServer.
         * @param filename A file containing an IOR which refers to the existing TaskContextServer.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA proxy for \a filename.
         */
        static TaskContextProxy* CreateFromFile(std::string filename);

        /**
         * Factory method: create a CORBA Proxy for an existing TaskContextServer.
         * This method may in fact return the real TaskContext in case the servant
         * of \a task is in the same process.
         * @param task The Object to create a proxy for.
         * @retval 0 if the ORB is not initialised
         * @return A new or previously created CORBA proxy for \a task, or the TaskContext
         * itself.
         */
        static TaskContext* Create(::RTT::corba::CTaskContext_ptr task, bool force_remote = false);

        /**
         * Get the Corba Object of the CTaskContext.
         * You need to duplicate this object reference in case you wish to keep a reference
         * to it.
         * This object universally identifies the remote TaskContextServer
         * and can be used to tell other (remote) objects where to find it.
         */
        corba::CTaskContext_ptr server() const;

        virtual bool activate();

        virtual bool start();

        virtual bool stop();

        virtual bool isRunning() const;

        virtual bool configure();

        virtual bool cleanup();

        virtual bool recover();
        
        virtual bool isActive() const;

        virtual bool isConfigured() const;

        virtual bool inFatalError() const;

        virtual bool inRunTimeError() const;

        virtual bool inException() const;

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

        virtual bool connectServices( TaskContext* peer );

        virtual bool ready();
        /**
         * Returns the default POA for all proxies.
         */
        static PortableServer::POA_ptr ProxyPOA();
    };
}}
#endif

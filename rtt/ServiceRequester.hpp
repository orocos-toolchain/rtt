/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  ServiceRequester.hpp

                        ServiceRequester.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_SERVICEREQUESTER_HPP_
#define ORO_SERVICEREQUESTER_HPP_

#include "rtt-config.h"
#include "rtt-fwd.hpp"
#include "base/OperationCallerBaseInvoker.hpp"
#include "Service.hpp"
#include <map>
#include <vector>
#include <string>

#include <boost/enable_shared_from_this.hpp>
#if BOOST_VERSION >= 104000 && BOOST_VERSION < 105300
#include <boost/smart_ptr/enable_shared_from_this2.hpp>
#endif
#if BOOST_VERSION >= 105300
#include <boost/smart_ptr/enable_shared_from_raw.hpp>
#endif

namespace RTT
{
    /**
     * An object that expresses you wish to use a service.
     * The ServiceRequester is symmetrical to the Service.
     * Where a Service registers operations that a component can
     * execute ('provides'), the ServiceRequester registers the methods that a caller
     * wishes to call ('requires'). One method in a ServiceRequester maps
     * to one operation in a Service.
     *
     * Typical use is to inherit from ServiceRequester and add named OperationCaller objects
     * to it using addOperationCaller. @see RTT::Scripting for an example.
     * @ingroup Services
     */
    class RTT_API ServiceRequester :
#if BOOST_VERSION >= 104000
#if BOOST_VERSION < 105300
        public boost::enable_shared_from_this2<ServiceRequester>
#else
        public boost::enable_shared_from_raw
#endif
#else
        public boost::enable_shared_from_this<ServiceRequester>
#endif

    {
    public:
        typedef std::vector<std::string> RequesterNames;
        typedef std::vector<std::string> OperationCallerNames;
        typedef boost::shared_ptr<ServiceRequester> shared_ptr;
        typedef boost::shared_ptr<const ServiceRequester> shared_constptr;

#if BOOST_VERSION >= 105300
        ServiceRequester::shared_ptr shared_from_this() { return boost::shared_from_raw(this); }
        ServiceRequester::shared_constptr shared_from_this() const { return boost::shared_from_raw(this); }
#endif

        ServiceRequester(const std::string& name, TaskContext* owner = 0);
        virtual ~ServiceRequester();

        const std::string& getRequestName() const { return mrname; }

        RequesterNames getRequesterNames() const;

        /**
         * The owner is the top-level TaskContext owning this service
         * (indirectly).
         */
        TaskContext* getServiceOwner() const { return mrowner; }

        /**
         * Sets the owning TaskContext that is considered as the
         * caller of requested operations.
         */
        void setOwner(TaskContext* new_owner);

        /**
         * Returns the service we're referencing.
         * In case you used connectTo to more than one service,
         * this returns the service which was used when connectTo
         * first returned true.
         */
        Service::shared_ptr getReferencedService() { return mprovider; }

        bool addOperationCaller( base::OperationCallerBaseInvoker& mbi);

        OperationCallerNames getOperationCallerNames() const;

        base::OperationCallerBaseInvoker* getOperationCaller(const std::string& name);

        ServiceRequester::shared_ptr requires();

        ServiceRequester::shared_ptr requires(const std::string& service_name);

        /**
         * Add a new ServiceRequester to this TaskContext.
         *
         * @param obj This object becomes owned by this TaskContext.
         *
         * @return true if it could be added, false if such
         * service requester already exists.
         */
        bool addServiceRequester(shared_ptr obj);

        /**
         * Query if this service requires certain sub-services.
         * @param service_name
         * @return
         */
        bool requiresService(const std::string& service_name) {
            return mrequests.find(service_name) != mrequests.end();
        }

        /**
         * Connects this service's methods to the operations provided by op.
         * This method tries to match as many as possible method-operation pairs.
         *
         * You may call this function with different instances of sp to 'resolve'
         * missing functions, only the non-connected methods will be further filled in.
         * @param sp An interface-compatible Service.
         *
         * @return true if all methods of that are required are provided, false
         * if not all methods could yet be matched.
         */
        virtual bool connectTo(Service::shared_ptr sp);

        /**
         * Returns true when all methods were resolved.
         * @return
         */
        virtual bool ready() const;

        /**
         * Disconnects all methods from their implementation.
         */
        virtual void disconnect();

        /**
         * Remove all operation callers from this service requester.
         */
        virtual void clear();

    protected:
        typedef std::map< std::string, ServiceRequester::shared_ptr > Requests;
        /// the services we implement.
        Requests mrequests;

        /// Our methods
        typedef std::map<std::string, base::OperationCallerBaseInvoker*> OperationCallers;
        OperationCallers mmethods;

        std::string mrname;
        TaskContext* mrowner;
        Service::shared_ptr mprovider;
    };
}

#endif /* ORO_SERVICEREQUESTER_HPP_ */

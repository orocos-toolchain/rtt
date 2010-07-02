#ifndef ORO_SERVICEREQUESTER_HPP_
#define ORO_SERVICEREQUESTER_HPP_

#include "rtt-config.h"
#include "../rtt-fwd.hpp"
#include "../base/MethodBaseInvoker.hpp"
#include "ServiceProvider.hpp"
#include <map>
#include <vector>
#include <string>

namespace RTT
{

    namespace interface
    {

        /**
         * An object that expresses you wish to use a service.
         * The ServiceRequester is symmetrical to the ServiceProvider.
         * Where a ServiceProvider registers operations that a component can
         * execute ('provides'), the ServiceRequester registers the methods that a caller
         * wishes to call ('requires'). One method in a ServiceRequester maps
         * to one operation in a ServiceProvider.
         *
         * Typical use is to inherit from ServiceRequester and add named Method objects
         * to it using addMethod. @see RTT::Scripting for an example.
         */
        class RTT_API ServiceRequester
        {
        public:
            typedef std::vector<std::string> RequesterNames;
            typedef std::vector<std::string> MethodNames;
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
             * Returns the service we're referencing.
             * In case you used connectTo to more than one service,
             * this returns the service which was used when connectTo
             * first returned true.
             */
            ServiceProvider::shared_ptr getReferencedService();

            bool addMethod( base::MethodBaseInvoker& mbi);

            MethodNames getMethodNames() const;

            base::MethodBaseInvoker& getMethod(const std::string& name);

            ServiceRequester* requires() { return this; }

            ServiceRequester* requires(const std::string& service_name) {
                ServiceRequester* sp = mrequests[service_name];
                if (sp)
                    return sp;
                sp = new ServiceRequester(service_name, mrowner);
                mrequests[service_name] = sp;
                return sp;
            }

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
             * @param sp An interface-compatible ServiceProvider.
             *
             * @return true if all methods of that are required are provided, false
             * if not all methods could yet be matched.
             */
            bool connectTo(ServiceProvider::shared_ptr sp);

            /**
             * Returns true when all methods were resolved.
             * @return
             */
            bool ready() const;

            /**
             * Disconnects all methods from their implementation.
             */
            void disconnect();

        protected:
            typedef std::map< std::string, interface::ServiceRequester* > Requests;
            /// the services we implement.
            Requests mrequests;

            /// Our methods
            typedef std::map<std::string, base::MethodBaseInvoker*> Methods;
            Methods mmethods;

            std::string mrname;
            TaskContext* mrowner;
            ServiceProvider::shared_ptr mprovider;
        };

    }

}

#endif /* ORO_SERVICEREQUESTER_HPP_ */

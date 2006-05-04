#ifndef ORO_CORBA_ACTIONPROXY_HPP
#define ORO_CORBA_ACTIONPROXY_HPP

#include <corelib/CommandInterface.hpp>
#include "ExecutionC.h"

namespace ORO_Corba
{

    /**
     * This class manages the access of remote Action Corba Servers.
     */
    class ActionProxy
        : public ORO_CoreLib::CommandInterface
    {
    protected:
        static std::map<Orocos::Action_ptr, ActionProxy*> proxies;

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        ActionProxy( ::Orocos::Action_ptr t );

        Orocos::Action_var mdata;

    public:

        /**
         * Factory method: create a CORBA connection to an existing Action Object.
         * @param act The Object to connect to.
         * @return A new or previously created CORBA proxy for \a act.
         */
        static ActionProxy* Create(::Orocos::Action_ptr act);

        /**
         * Get the Corba Object reference of the Action.
         * This object universally identifies the remote Action Object
         * and can be used to tell other (remote) objects where to find it.
         */
        Orocos::Action_ptr server() const;

        virtual void readArguments() {}

        virtual bool execute() {
            return mdata->execute();
        }

        virtual void reset() {
            mdata->reset();
        }

        virtual ORO_CoreLib::CommandInterface* clone() const {
            return new ActionProxy( mdata.in() );
        }

        virtual ORO_CoreLib::CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            return this->clone();
        }

    };

}

#endif

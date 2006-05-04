#ifndef ORO_CORBA_COMMANDPROXY_HPP
#define ORO_CORBA_COMMANDPROXY_HPP

#include <execution/DispatchInterface.hpp>
#include "ExecutionC.h"

namespace ORO_Corba
{

    /**
     * This class manages the access of remote Command Corba Servers.
     */
    class CommandProxy
        : public ORO_Execution::DispatchInterface
    {
    protected:
        static std::map<Orocos::Command_ptr, CommandProxy*> proxies;

        /**
         * Private constructor which creates a new connection to
         * a corba object
         */
        CommandProxy( ::Orocos::Command_ptr t );

        Orocos::Command_var mdata;

    public:

        /**
         * Factory method: create a CORBA connection to an existing Command Object.
         * @param act The Object to connect to.
         * @return A new or previously created CORBA proxy for \a act.
         */
        static CommandProxy* Create(::Orocos::Command_ptr act);

        /**
         * Get the Corba Object reference of the Command.
         * This object universally identifies the remote Command Object
         * and can be used to tell other (remote) objects where to find it.
         */
        Orocos::Command_ptr server() const;

        virtual bool execute() {
            return mdata->execute();
        }

        virtual void reset() {
            mdata->reset();
        }

        virtual bool sent() const {
            return mdata->sent();
        }

        virtual bool accepted() const {
            return mdata->accepted();
        }

        virtual bool executed() const {
            return mdata->executed();
        }

        virtual bool valid() const {
            return mdata->valid();
        }

        virtual ORO_CoreLib::ConditionInterface* createValidCondition() const;

        virtual ORO_CoreLib::CommandInterface* clone() const {
            return new CommandProxy( mdata.in() );
        }

        virtual ORO_CoreLib::CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            return this->clone();
        }

    };

}

#endif

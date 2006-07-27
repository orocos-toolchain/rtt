#ifndef ORO_DISPATCH_ACTION_HPP
#define ORO_DISPATCH_ACTION_HPP

#include "DispatchInterface.hpp"

namespace RTT
{
    /**
     * An action which invokes a dispatch command
     * in execute().
     */
    class DispatchAction
        : public ActionInterface
    {
        DispatchInterface*  mdi;
    public:
        DispatchAction(DispatchInterface* di)
            : mdi(di)
        {}

        ~DispatchAction() {
            delete mdi;
        }

        void reset() { mdi->reset(); }

        bool execute() {
            // if the command was not dispatched yet,
            // do this and return the result.
            if ( !mdi->sent() )
                return mdi->dispatch(); 
            // if it was dispatched and already executed,
            // return the result of the command function
            if ( mdi->executed() )
                return mdi->valid();
            // in the intermediate state, just return the
            // value of the dispatch().
            return mdi->accepted();
        }

        bool valid() const { return mdi->valid(); }

        void readArguments() { mdi->readArguments(); }

        virtual DispatchAction* clone() const {
            return new DispatchAction( mdi->clone() );
        }

        virtual DispatchAction* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new DispatchAction( mdi->copy(alreadyCloned));
        }
        
    };

}

#endif

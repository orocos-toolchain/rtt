#include "Processor.hpp"
#include "DataSource.hpp"
#include <corelib/CommandInterface.hpp>


namespace ORO_Execution
{
    /**
     * Dispatch a CommandInterface to another processor.
     * Combine with TryCommand, which provides the DataSource for
     * the constructor.
     * The execute() method will return false from the moment
     * on that the dispatched command failed. Hence, this execute()
     * must not wrapped in an AsyncCommandDecorator, but directly
     * executed by the calling processor.
     */
    struct CommandDispatch :
        public CommandInterface
    {
        DataSource<bool>::shared_ptr _dsb;
        bool send;
        Processor* proc;
        CommandInterface* com;
        CommandDispatch(Processor* p, CommandInterface* c,  DataSource<bool>* dsb )
            : _dsb(dsb), send(true), proc(p), com(c) {}
        bool execute() {
            if ( send ) {
                if (proc->process( com ) == true) // try to send each time.
                    {
                        send = false;
                        //cout << "Message Dispatched !" <<endl;
                    }
            }
            // return the accept/reject status.
            return _dsb->get();
        }
        void reset() {
            send = true;
            com->reset();
            _dsb->reset();
        }
        CommandInterface* clone() const {
            return new CommandDispatch( proc, com, _dsb.get() );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandDispatch( proc, com, _dsb->copy( alreadyCloned ) );
        }
    };
}

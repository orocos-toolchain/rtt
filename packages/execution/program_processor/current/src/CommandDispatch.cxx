
#include "execution/CommandDispatch.hpp"
#include "execution/Processor.hpp"

namespace ORO_Execution
{
        CommandDispatch::CommandDispatch(Processor* p, CommandInterface* c,  VariableDataSource<bool>* result )
            : _result(result), send(true), proc(p), com(c) {}

        CommandDispatch::~CommandDispatch() {
            delete com;
        }

        bool CommandDispatch::execute() {
            if ( send ) {
                if ( proc->process( com ) != 0 ) {
                    // send success !
                    send = false;
                    //cout << "Message Dispatched !" <<endl;
                }
                else {
                    // send failed ! Target Processor probably not running, give up.
                    send = false;
                    // set _result to false, because com will not do this itself.
                    _result->set(false);
                    return false;
                }
            }
            // return the (remote)  accept/reject status.
            // initially, it will be true (not yet executed), but may revert to false
            // if the command function returns false.
            return _result->get();
        }

        void CommandDispatch::reset() {
            send = true;
            com->reset();
            // do not reset _result, we do not own it.
        }

        CommandInterface* CommandDispatch::clone() const {
            return new CommandDispatch( proc, com->clone(), _result.get() );
        }

        CommandInterface* CommandDispatch::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandDispatch( proc, com->copy( alreadyCloned ), _result->copy( alreadyCloned ) );
        }

}

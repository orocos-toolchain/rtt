
#include "execution/CommandDispatch.hpp"
#include "execution/Processor.hpp"

namespace ORO_Execution
{
        CommandDispatch::CommandDispatch(Processor* p, CommandInterface* c,  DataSource<bool>* result )
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
                    return false;
                }
            }
            // return the accept/reject status.
            return _result->get();
        }

        void CommandDispatch::reset() {
            send = true;
            com->reset();
            _result->reset();
        }

        CommandInterface* CommandDispatch::clone() const {
            return new CommandDispatch( proc, com->clone(), _result.get() );
        }

        CommandInterface* CommandDispatch::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandDispatch( proc, com->copy( alreadyCloned ), _result->copy( alreadyCloned ) );
        }

}

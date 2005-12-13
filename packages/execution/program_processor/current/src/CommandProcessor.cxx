#include "execution/CommandProcessor.hpp"
#include <corelib/CommandInterface.hpp>
#include <corelib/AtomicQueue.hpp>
#include <corelib/Logger.hpp>

#include <os/Semaphore.hpp>

namespace ORO_Execution
{

    using namespace std;
    using namespace ORO_CoreLib;
    using namespace ORO_OS;

    CommandProcessor::CommandProcessor(int queue_size, Semaphore* s )
        :a_queue( new ORO_CoreLib::AtomicQueue<CommandInterface*>(queue_size) ),
         coms_processed(0),
         queuesem( s ),
         accept(false)
    {
    }

    CommandProcessor::~CommandProcessor()
    {
        delete a_queue;
    }

    bool CommandProcessor::initialize()
    {
        a_queue->clear();
        accept = true;
        return true;
    }

    void CommandProcessor::finalize()
    {
        accept = false;
    }

	void CommandProcessor::step()
    {
        // execute one command from the AtomicQueue.
        CommandInterface* com(0);
        int res = a_queue->dequeueCounted( com );
        if ( com ) {
            if ( queuesem )
                queuesem->trywait();
            // note : the command's result is discarded.
            // Wrap your command (ie CommandDispatch) to keep track
            // of the result of enqueued commands.
            com->execute();
            // let the user know this command was processed.
            coms_processed = res;
        }
    }

    int CommandProcessor::process( CommandInterface* c )
    {
        if (accept && c) {
            int result = a_queue->enqueueCounted( c );
            if ( queuesem )
                queuesem->signal();
            return result;
        }
        return 0;
    }

    bool CommandProcessor::isProcessed( int nr ) const
    {
        return ( nr <= coms_processed );
    }
}


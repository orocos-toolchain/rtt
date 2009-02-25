
#include "MessageProcessor.hpp"
#include <ActionInterface.hpp>
#include <AtomicQueue.hpp>

namespace RTT
{

    using namespace std;

    using namespace OS;

    MessageProcessor::MessageProcessor(int queue_size)
        :a_queue( new AtomicQueue<CommandInterface*>(queue_size) ),
         accept(false)
    {
    }

    MessageProcessor::~MessageProcessor()
    {
        delete a_queue;
    }

    bool MessageProcessor::initialize()
    {
        a_queue->clear();
        accept = true;
        return true;
    }

    void MessageProcessor::finalize()
    {
        accept = false;
    }

	void MessageProcessor::step()
    {
        // execute one command from the AtomicQueue.
        ActionInterface* com(0);
        while ( !a_queue->isEmpty() ) {
            a_queue->dequeue( com );
            com->execute();
            rt_free( com );
        }
    }

    bool MessageProcessor::hasWork()
    {
        return ! a_queue->isEmpty();
    }

    bool MessageProcessor::process( ActionInterface* c )
    {
        if (accept && c) {
            bool result = a_queue->enqueue( c );
            this->getActivity()->trigger();
            return result;
        }
        return false;
    }

}




#ifndef ORO_MESSAGE_PROCESSOR_HPP
#define ORO_MESSAGE_PROCESSOR_HPP

#include "../base/RunnableInterface.hpp"
#include "../base/DisposableInterface.hpp"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "rtt-config.h"
#include "rtt-internal-fwd.hpp"

namespace RTT
{
namespace internal {
    /**
     * @brief This class implements an Orocos message processor.
     * It executes incomming messages when running.
     *
     * @ingroup Processor
     */
    class MessageProcessor
        : public base::RunnableInterface
    {
    public:
        /**
         * Constructs a new MessageProcessor
         * @param queue_size The size of the message queue.
         */
        MessageProcessor(int queue_size = ORONUM_EXECUTION_PROC_QUEUE_SIZE);

        virtual ~MessageProcessor();

        virtual bool initialize();
        virtual void step();
        virtual void finalize();
        virtual bool hasWork();

        /**
         * Queue and execute (process) a given message. The message is
         * executed in step() or loop() directly after all other
         * queued ActionInterface objects. The constructor parameter
         * \a queue_size limits how many messages can be queued in
         * between step()s or loop().
         *
         * @return true if the message got accepted, false otherwise.
         * @return false when the MessageProcessor is not running or does not accept messages.
         * @see acceptMessages
         */
        virtual bool process(base::DisposableInterface* c);

        /**
         * Should the MessageProcessor accept or reject messages in \a process().
         * @param true_false true to accept, false to reject.
         */
        void acceptMessages( bool true_false) { accept=true_false; }

    protected:

        AtomicQueue<base::DisposableInterface*,base::NonBlockingPolicy,base::NonBlockingPolicy>* a_queue;

        bool accept;
    };
}
}

#endif

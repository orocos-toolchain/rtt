
#ifndef ORO_COMMAND_PROCESSOR_HPP
#define ORO_COMMAND_PROCESSOR_HPP

#include "corelib/RunnableInterface.hpp"
#include "corelib/CommandInterface.hpp"
#include "corelib/BufferPolicy.hpp"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <pkgconf/execution_program_processor.h>

namespace ORO_OS
{
    class Semaphore;
    class Mutex;
    class MutexRecursive;
}

namespace ORO_CoreLib
{
    template< class T, class RP, class WP>
    class AtomicQueue;
}

namespace ORO_Execution
{
    /**
     * @brief This class implements an Orocos command processor.
     * It executes external commands when running.
     *
     */
    class CommandProcessor
        : public ORO_CoreLib::RunnableInterface
    {
    public:
        /**
         * Constructs a new CommandProcessor
         * @param queue_size The size of the command queue.
         */
        CommandProcessor(int queue_size = ORONUM_EXECUTION_PROC_QUEUE_SIZE, ORO_OS::Semaphore* work_sem = 0);

        virtual ~CommandProcessor();

        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        /**
         * Queue and execute (process) a given command. The command is executed in step() or loop() 
         * directly after all other queued CommandInterface objects. The constructor parameter \a queue_size 
         * limits how many commands can be queued in between step()s or loop().
         * @return The command number. You can check if the given command was processed
         * by calling CommandProcessor::isProcessed( command id ). The command number is reset to one
         * each time the CommandProcessor is (re-)started. 
         * @return 0 when the CommandProcessor is not running or does not accept commands.
         * @see isProcessed, acceptCommands
         */
        int process(ORO_CoreLib::CommandInterface* c);

        /**
         * Check if a given command id has been processed.
         */
        bool isProcessed( int id ) const;

        /**
         * Should the CommandProcessor accept or reject commands in \a process().
         * @param true_false true to accept, false to reject.
         */
        void acceptCommands( bool true_false) { accept=true_false; }

    private:

        ORO_CoreLib::AtomicQueue<ORO_CoreLib::CommandInterface*,ORO_CoreLib::NonBlockingPolicy,ORO_CoreLib::NonBlockingPolicy>* a_queue;

        /**
         * Counting how much commands we processed.
         */
        int coms_processed;

        ORO_OS::Semaphore* queuesem;
        bool accept;
    };

}

#endif

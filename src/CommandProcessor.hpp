/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  CommandProcessor.hpp

                        CommandProcessor.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#ifndef ORO_COMMAND_PROCESSOR_HPP
#define ORO_COMMAND_PROCESSOR_HPP

#include "RunnableInterface.hpp"
#include "CommandInterface.hpp"
#include "BufferPolicy.hpp"

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "rtt-config.h"

namespace RTT
{
    template< class T, class RP, class WP>
    class AtomicQueue;
}

namespace RTT
{
    /**
     * @brief This class implements an Orocos command processor.
     * It executes external commands when running.
     *
     * @par Changing the Command Processing Policy
     *
     * The default policy of the CommandProcessor is to dequeue one
     * command in each step() and execute it. If you want to change
     * this policy, subclass the CommandProcessor and override the
     * virtual functions, such as step() or process().  The current
     * command queue is non blocking for push and pop operations.
     * process() fails when the queue is full; step() does nothing
     * when the queue is empty. A subclass may install a new
     * command queue with for example blocking semantics.
     * @ingroup Processor
     */
    class CommandProcessor
        : public RunnableInterface
    {
    public:
        /**
         * Constructs a new CommandProcessor
         * @param queue_size The size of the command queue.
         */
        CommandProcessor(int queue_size = ORONUM_EXECUTION_PROC_QUEUE_SIZE);

        virtual ~CommandProcessor();

        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        /**
         * Queue and execute (process) a given command. The command is
         * executed in step() or loop() directly after all other
         * queued CommandInterface objects. The constructor parameter
         * \a queue_size limits how many commands can be queued in
         * between step()s or loop().
         *
         * @warning process() assumes that step() is executed once
         * for each command, hence, it invokes ActivityInterface::trigger() each time.
         * In case you override step() in a subclass, override
         * process as well in order to reduce calling trigger().
         *
         * @return The command number. You can check if the given command was processed
         * by calling CommandProcessor::isProcessed( command id ). The command number is reset to one
         * each time the CommandProcessor is (re-)started.
         * @return 0 when the CommandProcessor is not running or does not accept commands.
         * @see isProcessed, acceptCommands
         */
        virtual int process(CommandInterface* c);

        /**
         * Check if a given command id has been processed.
         */
        virtual bool isProcessed( int id ) const;

        /**
         * Should the CommandProcessor accept or reject commands in \a process().
         * @param true_false true to accept, false to reject.
         */
        void acceptCommands( bool true_false) { accept=true_false; }

    protected:

        AtomicQueue<CommandInterface*,NonBlockingPolicy,NonBlockingPolicy>* a_queue;

        /**
         * Counting how much commands we processed.
         */
        int coms_processed;

        bool accept;
    };

}

#endif

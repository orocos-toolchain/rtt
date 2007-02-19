/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  Processor.hpp

                        Processor.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef PROGRAM_PROCESSOR_HPP
#define PROGRAM_PROCESSOR_HPP

#include "RunnableInterface.hpp"
#include "BufferPolicy.hpp"
#include "ListLockFree.hpp"
#include "ProgramExceptions.hpp"
#include "ProgramInterface.hpp"

#include <map>
#include <string>
#include <vector>

#include "rtt-config.h"

namespace RTT
{
    template< class T, class RP, class WP>
    class AtomicQueue;

}

namespace RTT
{
    /**
     * This class implements a real-time program processor.
     * It executes Realtime Programs when running.
     *
     * @par Changing the Program Processing Policy
     *
     * The default policy of the ProgramProcessor is to call
     * ProgramInterface::execute() of all loaded programs in each step(). Within
     * the same step(), after all programs are executed, any loaded
     * function is executed. The functions are automatically unloaded
     * when they are in error or finished. 
     *
     * If you want to change this
     * policy, subclass the ProgramProcessor and override the virtual
     * functions, such as step().
     * @ingroup Processor
     */
    class ProgramProcessor
        : public RunnableInterface
    {
    public:
        /**
         * Constructs a new ProgramProcessor
         * @param f_queue_size The maximum number of 'functions' this processor can execute
         */
        ProgramProcessor(int f_queue_size = ORONUM_EXECUTION_PROC_QUEUE_SIZE);

        virtual ~ProgramProcessor();

        typedef ProgramInterface::Status Status;

        /**
         * Load a new Program. The Processor takes full ownership and will
         * delete it upon destruction.
         * @throw program_load_exception if a program with the same name already exists.
         */
        bool loadProgram( ProgramInterfacePtr pi );

        /**
         * Completely discard a loaded Program.
         * @throw program_unload_exception if the program is
         * not stopped or does not exist.
         * @deprecated by unloadProgram
         */
        bool deleteProgram(const std::string& name);
        
        /**
         * Unload a program from this processor.
         * @throw program_unload_exception if the program is
         * not stopped or does not exist.
         */
        bool unloadProgram(const std::string& name);

        /**
         * Return the status of a Program.
         */
        Status::ProgramStatus getProgramStatus(const std::string& name) const;

        /**
         * Return the status of a Program as a human readable string.
         */
        std::string getProgramStatusStr(const std::string& name) const;

        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        /**
         * Run a given function in step() or loop(). The function may only
         * be destroyed after isFunctionFinished() returns true,the 
         * Processor is stopped or removeFunction() was invoked. The number of functions the Processor can
         * run in parallel is set in its constructor argument \a queue_size. In
         * addition to that number, a queue of the same size is allocated for
         * pending functions to execute.
         * @return false if the Processor is not running or the 'pending' queue is full.
         * @see isFunctionFinished, acceptCommands, removeFunction
         */
        virtual bool runFunction(ProgramInterface* f);

        /**
         * Remove a running function added with runFunction.
         * This method is only required if the function is to be destroyed
         * and is still present in the ProgramProcessor.
         */
        virtual bool removeFunction(ProgramInterface* f);

        /**
         * Get a list of all loaded Programs.
         */
        std::vector<std::string> getProgramList() const;

        /**
         * Get a pointer to a loaded Program.
         */
        const ProgramInterfacePtr getProgram(const std::string& name) const;

        ProgramInterfacePtr getProgram(const std::string& name);

    protected:
        typedef ListLockFree<ProgramInterfacePtr> ProgMap;
        ProgMap* programs;

        std::vector<ProgramInterface*> funcs;

        AtomicQueue<ProgramInterface*,NonBlockingPolicy,NonBlockingPolicy>* f_queue;

    };

}

#endif

/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  Processor.hpp

                        Processor.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef PROGRAM_PROCESSOR_HPP
#define PROGRAM_PROCESSOR_HPP

#include "corelib/RunnableInterface.hpp"
#include "corelib/BufferPolicy.hpp"
#include "ProgramExceptions.hpp"
#include "ProgramInterface.hpp"

#include <map>
#include <string>
#include <vector>

#include <pkgconf/execution_program_processor.h>

namespace ORO_OS
{
    class Mutex;
    class MutexRecursive;
    class Semaphore;
}

namespace ORO_CoreLib
{
    template< class T, class RP, class WP>
    class AtomicQueue;
}

namespace ORO_Execution
{
    /**
     * This class implements a controllable program processor.
     * It executes Realtime Programs when running.
     */
    class ProgramProcessor
        : public ORO_CoreLib::RunnableInterface
    {
    public:
        /**
         * Constructs a new ProgramProcessor
         * @param f_queue_size The maximum number of 'functions' this processor can execute
         * @param work_sem An optional semaphore which the semaphore will signal if it receives
         * work and consume if it processes work. Only to be used in non periodic mode.
         *
         */
        ProgramProcessor(int f_queue_size = ORONUM_EXECUTION_PROC_QUEUE_SIZE, ORO_OS::Semaphore* work_sem = 0);

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
        bool runFunction(ProgramInterface* f);

        /**
         * Remove a running function added with runFunction.
         * This method is only required if the function is to be destroyed
         * and is still present in the ProgramProcessor.
         */
        bool removeFunction(ProgramInterface* f);

        /**
         * Get a list of all loaded Programs.
         */
        std::vector<std::string> getProgramList() const;

        /**
         * Get a pointer to a loaded Program.
         */
        const ProgramInterfacePtr getProgram(const std::string& name) const;

        ProgramInterfacePtr getProgram(const std::string& name);

    private:
        typedef std::map<std::string,ProgramInterfacePtr> ProgMap;
        ProgMap* programs;
        typedef ProgMap::iterator program_iter;
        typedef ProgMap::const_iterator cprogram_iter;

        std::vector<ProgramInterface*> funcs;

        ORO_CoreLib::AtomicQueue<ProgramInterface*,ORO_CoreLib::NonBlockingPolicy,ORO_CoreLib::NonBlockingPolicy>* f_queue;

        /**
         * Guard load list
         */
        ORO_OS::MutexRecursive* loadmonitor;
        ORO_OS::Semaphore* f_queue_sem;

    };

}

#endif

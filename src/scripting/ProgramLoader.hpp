/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  ProgramLoader.hpp

                        ProgramLoader.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
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


#ifndef ORO_PROGRAM_LOADER_HPP
#define ORO_PROGRAM_LOADER_HPP

#include <string>
#include "../TaskContext.hpp"
#include "../rtt-config.h"

namespace RTT
{ namespace scripting {

    /**
     * This class loads and unloads Orocos Program Script and Orocos State Description files
     * to a TaskContext's Processor.
     * It wraps around Parser and can process both files and C++ streams.
     * Parse exceptions are directed to the Logger class or
     * rethrown if requested by the user.
     */
    class RTT_API ProgramLoader
    {
        bool mrethrow;
    public:
        /**
         * Create a ProgramLoader which by default does not
         * throw any exceptions.
         * @param rethrow If set to true, rethrow the parse and load exceptions
         * in addition to logging the error.
         * @see program_load_exception, program_unload_exception, parse_exception
         */
        ProgramLoader( bool rethrow = false );

        /**
         * List of executed functions.
         */
        typedef std::vector< base::ProgramInterfacePtr > Functions;

        /**
         * Executes all not exported functions in \a target's
         * Processor or loads exported functions in \a target's Command Interface
         * @return All functions being executed.
         */
        Functions loadFunction( const std::string& s, TaskContext* target, const std::string& filename);

        /**
         * Executes all not exported functions in \a target's
         * Processor or loads exported functions in \a target's Command Interface.
         * @return All functions being executed.
         */
        Functions loadFunction( const std::string& filename, TaskContext* target);

        /**
         * Load Programs and/or exported Functions from an Orocos Program Script into a TaskContext.
         * Reads out the stream \a s , parses it, and loads it in \a target's Processor.
         * @return true if all programs and functions could be loaded.
         */
        bool loadProgram( const std::string& s, TaskContext* target, const std::string& filename);

        /**
         * Load Programs and/or exported Functions from an Orocos Program Script into a TaskContext.
         * Reads out the local file \a filename , parses it, and loads it in \a target's Processor.
         * @return true if all programs and functions could be loaded.
         */
        bool loadProgram( const std::string& filename, TaskContext* target);

        /**
         * Unload a Program from a TaskContext.
         * @param name the name of the program.
         */
        bool unloadProgram( const std::string& name, TaskContext* target);

        /**
         * Load StateMachines from an Orocos State Description into a TaskContext.
         * Reads out the stream \a s , parses it, and loads it in \a target's Processor.
         * @return true if all state machines could be loaded.
         */
        bool loadStateMachine(const std::string& s, TaskContext* target, const std::string& filename );

        /**
         * Load StateMachines from an Orocos State Description into a TaskContext.
         * Reads out the local file \a filename , parses it, and loads it in \a target's Processor.
         * @return true if all state machines could be loaded.
         */
        bool loadStateMachine(const std::string& filename, TaskContext* target);

        /**
         * Unload a root (parent) StateMachine from a TaskContext.
         * @param name the name of the StateMachine.
         */
        bool unloadStateMachine( const std::string& name, TaskContext* target);
    };
}}

#endif

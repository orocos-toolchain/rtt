/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProgramInterface.hpp

                        ProgramInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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

#ifndef ORO_PROGRAMINTERFACE_HPP
#define	ORO_PROGRAMINTERFACE_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "../base/DataSourceBase.hpp"
#include "../base/AttributeBase.hpp"
#include "../base/ExecutableInterface.hpp"
#include "../rtt-config.h"
#include "../rtt-fwd.hpp"
#include "rtt-scripting-fwd.hpp"

namespace RTT
{ namespace scripting {
    typedef boost::shared_ptr<ProgramInterface> ProgramInterfacePtr;
    typedef boost::weak_ptr<ProgramInterface> ProgramInterfaceWPtr;

	/**
	 * @brief A Program represents a collection of
	 * instructions that can be stepwise executed.
	 */
	class RTT_API ProgramInterface
        : public base::ExecutableInterface
	{
    public:
        /**
         * Placeholder of the ProgramStatus.
         */
        struct RTT_API Status {
            /**
             * Enumerates the statuses of a ProgramInterface.
             */
            enum ProgramStatus {
                unknown = 0, //! This program is currently not loaded in an ExecutionEngine.
                stopped, //! The program is not running.
                paused,  //! The program was running but is now paused.
                running, //! The program is running.
                error    //! The program was running but has encountered an error.
            };
        };
    protected:
        Status::ProgramStatus pStatus;
    public:
        ProgramInterface();

        virtual ~ProgramInterface();

        /**
         * Start the execution of this program.
         */
        virtual bool start() = 0;

        /**
         * Pause or start-and-pause the execution of this program.
         */
        virtual bool pause() = 0;

        /**
         * Execute a single action when paused.
         */
        virtual bool step() = 0;

        /**
         * Stop the execution of this program.
         */
        virtual bool stop() = 0;

        /**
         * Execute as much actions until the program needs to
         * wait on a condition to become true. When paused,
         * only execute one action.
         * @return false if a program error occured.
         */
        virtual bool execute()= 0;

        /**
         * Reset the execution point to the beginning of this program interface.
         */
        virtual void reset() = 0;

        /**
         * Returns true if the program is running.
         */
        inline bool isRunning() const { return pStatus == Status::running; }

        /**
         * Returns true if the program is paused.
         */
        inline bool isPaused() const { return pStatus == Status::paused; }

        /**
         * Returns true if the program is not executing (stopped) or not loaded.
         */
        inline bool isStopped() const { return pStatus == Status::stopped; }

        /**
         * Returns true if the program is in error.
         */
        inline bool inError() const { return pStatus == Status::error; }

        virtual bool stepDone() const = 0;

        /**
         * Return the current 'line number' of the program.
         */
        virtual int getLineNumber() const = 0;

        /**
         * Return the current status of the program.
         */
        Status::ProgramStatus getStatus() const { return pStatus; }

        /**
         * Return the program text to which \a getLineNumber()
         * refers.
         */
        virtual std::string getText() const = 0;

        /**
         * Programs can be refered to by name.
         */
        virtual const std::string& getName() const = 0;

        /**
         * Get the argument list of this program.
         */
        virtual std::vector<base::AttributeBase*> getArguments() const = 0;

        /**
         * Get the return value of this program.
         */
        virtual base::AttributeBase* getResult() const = 0;

        /**
         * Clone this Program.  This will produce a completely
         * new instance, that has nothing in common with this one.
         * It takes care to properly map identical DataSources to
         * identical DataSources.
         *
         * @param alreadyMappedData A map of some DataSources used in
         *   this program to new DataSources that should replace them
         *   in the new Program.  This is provided, because in some
         *   cases the outside world also keeps references to
         *   datasources used somewhere in this programgraph.  It is
         *   then important that when this Program is copied, the
         *   outside world has a way to get a reference to the
         *   corresponding datasources in the new program.  We do this
         *   by allowing it to map some datasources itself, and simply
         *   provide us a list of its mappings.
         */
        virtual ProgramInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacementdss ) const = 0;

        virtual ProgramInterface* clone() const = 0;
	};


}}


#endif



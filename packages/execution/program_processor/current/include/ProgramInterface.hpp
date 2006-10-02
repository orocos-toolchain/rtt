/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProgramInterface.hpp 

                        ProgramInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef PROGRAMINTERFACE_HPP
#define	PROGRAMINTERFACE_HPP

#include <string>
#include <vector>
#include "DataSourceBase.hpp"
#include "AttributeBase.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{
	class ProgramProcessor;
    class ProgramInterface;
    typedef boost::shared_ptr<ProgramInterface> ProgramInterfacePtr;
    typedef boost::weak_ptr<ProgramInterface> ProgramInterfaceWPtr;

	/**
	 * @brief A Program represents a collection of 
	 * instructions that can be stepwise executed.
	 */
	class ProgramInterface
	{
    public:
        /**
         * Placeholder of the ProgramStatus.
         */
        struct Status {
            /**
             * Enumerates the statuses of a ProgramInterface.
             */
            enum ProgramStatus { stopped, //! The program is loaded in a ProgramProcessor but not running.
                                 running, //! The program is running.
                                 paused,  //! The program was running but is now paused.
                                 error,   //! The program was running but has encountered an error.
                                 unloaded //! This program is currently not loaded in a ProgramProcessor.
            };
        };
    protected:
        Status::ProgramStatus pStatus;
        ProgramProcessor* pp;
        virtual void handleUnload();
    public:
        ProgramInterface(ProgramProcessor* progp = 0);

        virtual ~ProgramInterface();

        void setProgramProcessor(ProgramProcessor* progp);

        ProgramProcessor* getProgramProcessor() const { return pp; }

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
        inline bool isStopped() const { return pStatus == Status::stopped || pStatus == Status::unloaded ; }
	
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
        virtual std::vector<AttributeBase*> getArguments() const = 0;

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
        virtual ProgramInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacementdss ) const = 0;

        virtual ProgramInterface* clone() const = 0;
	};


}


#endif



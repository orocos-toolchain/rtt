/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ProgramProcessor.cxx 

                        ProgramProcessor.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#include "rtt/ProgramProcessor.hpp"
#include "rtt/ProgramInterface.hpp"
#include <rtt/AtomicQueue.hpp>
#include <rtt/Logger.hpp>
#include <rtt/Exceptions.hpp>

#include <boost/bind.hpp>
#include <functional>

namespace RTT
{
    using namespace boost;
    using namespace std;
    

    ProgramProcessor::ProgramProcessor(int f_queue_size)
        : programs( new ProgMap(4) ),
          funcs( f_queue_size ),
          f_queue( new AtomicQueue<ProgramInterface*>(f_queue_size) )
    {
    }

    ProgramProcessor::~ProgramProcessor()
    {
        while ( !programs->empty() ) {
            Logger::log() << Logger::Info << "ProgramProcessor deletes Program "<< programs->front()->getName() << "..."<<Logger::endl;
            programs->front()->stop();
            this->unloadProgram( programs->front()->getName() );
        }

        delete programs;
        delete f_queue;
    }

     ProgramInterface::Status::ProgramStatus ProgramProcessor::getProgramStatus(const std::string& name) const
     {
         ProgramInterfacePtr pip = programs->find_if( bind(equal_to<string>(), name, bind(&ProgramInterface::getName, _1)) );

         if ( pip )
             return pip->getStatus();
         return Status::unloaded;
     }

     std::string ProgramProcessor::getProgramStatusStr(const std::string& name) const
     {
        switch ( getProgramStatus( name ))
            {
            case Status::unloaded:
                return "unloaded";
                break;
            case Status::stopped:
                return "stopped";
                break;
            case Status::running:
                return "running";
                break;
            case Status::paused:
                return "paused";
                break;
            case Status::error:
                return "error";
                break;
            }
        return "na";
     }

	bool ProgramProcessor::loadProgram(ProgramInterfacePtr pi)
    {
        ProgramInterfacePtr pip = programs->find_if( boost::bind(equal_to<string>(), pi->getName(), boost::bind(&ProgramInterface::getName, _1)) );
        if ( pip )
            return false;
        programs->grow();
        programs->append( pi );
        pi->setProgramProcessor(this);
        pi->reset();
        return true;
    }

	bool ProgramProcessor::deleteProgram(const std::string& name)
    {
        return this->unloadProgram(name);
    }

	bool ProgramProcessor::unloadProgram(const std::string& name)
    {
        ProgramInterfacePtr pip = programs->find_if( bind(equal_to<string>(), name, bind(&ProgramInterface::getName, _1)) );

        if ( pip && pip->isStopped() )
            {
                pip->setProgramProcessor(0);
                programs->erase(pip);
                programs->shrink();
                return true;
            }
        if ( !pip ) {
                std::string error(
                                  "Could not unload Program \"" + name +
                                  "\" with the processor. It does not exist." );
                ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
            }
        if ( !pip->isStopped() ) {
                std::string error(
                                  "Could not unload Program \"" + name +
                                  "\" with the processor. It is still running." );
                ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
            }
        return false;
    }

    bool ProgramProcessor::initialize()
    {
        f_queue->clear();
        return true;
    }

    void ProgramProcessor::finalize()
    {
        // stop all programs.
        programs->apply( boost::bind(&ProgramInterface::stop, _1));
    }

	void ProgramProcessor::step()
    {

        //Execute all normal programs->
        programs->apply( boost::bind(&ProgramInterface::execute, _1 ) );

        // Execute any Function :
        {
            // 0. find an empty spot to store :
            ProgramInterface* foo = 0;
            std::vector<ProgramInterface*>::iterator f_it = find(funcs.begin(), funcs.end(), foo );
            // 1. Fetch new ones from queue.
            while ( !f_queue->isEmpty() && f_it != funcs.end() ) {
                f_queue->dequeue( foo );
                *f_it = foo;
                // stored successfully, now reset for next item from queue.
                foo = 0;
                f_it = find(f_it, funcs.end(), foo );
            }
            // 2. execute all present
            for(std::vector<ProgramInterface*>::iterator it = funcs.begin();
                it != funcs.end(); ++it )
                if ( *it )
                    if ( (*it)->isStopped() || (*it)->inError() ){
                        (*it)->setProgramProcessor(0);
                        (*it) = 0;
                    } else
                        (*it)->execute();
        }
    }

    bool ProgramProcessor::runFunction( ProgramInterface* f )
    {
        if (this->getActivity() && this->getActivity()->isRunning() && f) {
            f->setProgramProcessor(this);
            if ( f->start() == false)
                return false;
            int result = f_queue->enqueue( f );
            // signal work is to be done:
            this->getActivity()->trigger();
            return result != 0;
        }
        return false;
    }

    bool ProgramProcessor::removeFunction( ProgramInterface* f )
    {
        std::vector<ProgramInterface*>::iterator f_it = find(funcs.begin(), funcs.end(), f );
        if ( f_it != funcs.end() ) {
            f->setProgramProcessor(0);
            *f_it = 0;
            return true;
        }
        return false;
    }

    std::vector<std::string> ProgramProcessor::getProgramList() const
    {
        std::vector<string> sret;
        programs->apply( bind( &vector<string>::push_back, ref(sret), bind( &ProgramInterface::getName, _1) ) );
        return sret;
    }

    const ProgramInterfacePtr ProgramProcessor::getProgram(const std::string& name) const
    {
        return programs->find_if(bind(equal_to<string>(), name, bind(&ProgramInterface::getName, _1)));
    }

    ProgramInterfacePtr ProgramProcessor::getProgram(const std::string& name)
    {
        return programs->find_if(bind(equal_to<string>(), name, bind(&ProgramInterface::getName, _1)));
    }
}


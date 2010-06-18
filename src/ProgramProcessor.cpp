/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ProgramProcessor.cxx

                        ProgramProcessor.cxx -  description
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



#include "ProgramProcessor.hpp"
#include "ProgramInterface.hpp"
#include "Queue.hpp"
#include <Logger.hpp>
#include <Exceptions.hpp>
#include "os/MutexLock.hpp"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <functional>

namespace RTT
{
    using namespace boost;
    using namespace std;
    using namespace OS;


    ProgramProcessor::ProgramProcessor(int f_queue_size)
        : programs( new ProgMap(4) ),
          funcs( f_queue_size ),
          f_queue( new Queue<ProgramInterface*>(f_queue_size) )
    {
    }

    ProgramProcessor::~ProgramProcessor()
    {
        this->clear();
        delete programs;
        delete f_queue;
    }

    void ProgramProcessor::clear() {
        while ( !programs->empty() ) {
            Logger::log() << Logger::Info << "ProgramProcessor deletes Program "<< programs->front()->getName() << "..."<<Logger::endl;
            programs->front()->stop();
            this->unloadProgram( programs->front()->getName() );
        }
        ProgramInterface* foo = 0;
    	for(std::vector<ProgramInterface*>::iterator it = funcs.begin();
    			it != funcs.end(); ++it ) {
			foo = *it;
    		if ( foo ) {
    			foo->stop();
    			foo->setProgramProcessor(0);
    			(*it) = 0;
    		}
    	}
        while ( !f_queue->isEmpty() ) {
            f_queue->dequeue( foo );
            foo->stop();
   			foo->setProgramProcessor(0);
        }
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

        {
            ProgramInterface* foo = 0;
            // Execute any Function :
            // 1. Fetch new ones from queue.
            while ( !f_queue->isEmpty() ) {
                // 0. find an empty spot to store :
                std::vector<ProgramInterface*>::iterator f_it = find(funcs.begin(), funcs.end(), foo );
                if ( f_it == funcs.end() )
                	break; // no spot found, leave in queue.
                f_queue->dequeue( foo );
                *f_it = foo;
                // stored successfully, now reset for next item from queue.
                foo = 0;
            }
            assert(foo == 0);
            // 2. execute all present (if any):
            if ( find_if( funcs.begin(), funcs.end(), lambda::_1 != foo) != funcs.end() ) {
    			MutexLock ml( syncer ); // we block until removeFunction finished.
            	for(std::vector<ProgramInterface*>::iterator it = funcs.begin();
            			it != funcs.end(); ++it ) {
        			foo = *it;
            		if ( foo ) {
            			if ( foo->isStopped() || foo->inError() ){
            				foo->setProgramProcessor(0);
            				(*it) = 0;
            			} else
            				foo->execute();
            		}
            	}
            }
        }
    }

    bool ProgramProcessor::runFunction( ProgramInterface* f )
    {
        if (this->getActivity() && this->getActivity()->isActive() && f) {
            f->setProgramProcessor(this);
            if ( f->start() == false) {
                f->setProgramProcessor(0);
                return false;
            }
            int result = f_queue->enqueue( f );
            // signal work is to be done:
            this->getActivity()->trigger();
            return result != 0;
        }
        return false;
    }

    bool ProgramProcessor::removeFunction( ProgramInterface* f )
    {
        // This is a best-effort implementation. We can't store foo in funcs
        // since this would race with step() :-(
        ProgramInterface* foo = 0;
        int sz =  f_queue->size();
        while ( sz ) {
            // Dequeue and check
            f_queue->dequeue( foo );
            if ( f == foo)
                return true;
            f_queue->enqueue( foo );
            // check next one in queue...
            foo = 0;
            --sz;
        }

        std::vector<ProgramInterface*>::iterator f_it = find(funcs.begin(), funcs.end(), f );
        if ( f_it != funcs.end() ) {
        	{
        		MutexLock ml(syncer);
        		f->stop();
                f->setProgramProcessor( 0 );
        		*f_it = 0;
        	}
            return true;
        }
        return false;
    }

    std::vector<std::string> ProgramProcessor::getProgramList() const
    {
        std::vector<string> sret;
        typedef void (vector<string>::*push_back_t)(const vector<string>::value_type& );
        push_back_t push_back = &vector<string>::push_back;
        programs->apply( bind( push_back, boost::ref(sret), bind( &ProgramInterface::getName, _1) ) );
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


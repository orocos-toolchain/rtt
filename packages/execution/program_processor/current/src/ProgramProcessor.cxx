
#include "execution/ProgramProcessor.hpp"
#include "execution/ProgramInterface.hpp"
#include <corelib/AtomicQueue.hpp>
#include <corelib/Logger.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/function.hpp>
#include <os/MutexLock.hpp>
#include <os/Semaphore.hpp>
#include <iostream>

namespace ORO_Execution
{
    using namespace boost;
    using namespace std;
    using ORO_OS::MutexLock;
    using namespace ORO_CoreLib;

    ProgramProcessor::ProgramProcessor(int f_queue_size, ORO_OS::Semaphore* work_sem)
        : programs( new ProgMap() ),
          funcs( f_queue_size ),
          f_queue( new ORO_CoreLib::AtomicQueue<ProgramInterface*>(f_queue_size) ),
          loadmonitor( new ORO_OS::MutexRecursive() ),
          f_queue_sem( work_sem )
    {
    }

    ProgramProcessor::~ProgramProcessor()
    {
        while ( !programs->empty() ) {
            Logger::log() << Logger::Info << "ProgramProcessor deletes Program "<< programs->begin()->first << "..."<<Logger::endl;
            programs->begin()->second->stop();
            this->unloadProgram( programs->begin()->first );
        }

        delete programs;
        delete f_queue;
        delete loadmonitor;
    }

     ProgramInterface::Status::ProgramStatus ProgramProcessor::getProgramStatus(const std::string& name) const
     {
        program_iter it =
            programs->find( name );
        if ( it == programs->end() )
            return Status::unloaded;
        return it->second->getStatus();
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
        program_iter it =
            programs->find( pi->getName() );
        if ( it != programs->end() )
            return false;
        MutexLock lock( *loadmonitor );
        programs->insert( make_pair( pi->getName(), pi ) );
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
        program_iter it =
            programs->find( name );

        if ( it != programs->end() && it->second->isStopped() )
            {
                MutexLock lock( *loadmonitor );
                it->second->setProgramProcessor(0);
                programs->erase(it);
                return true;
            }
        if ( it == programs->end() ) {
                std::string error(
                                  "Could not unload Program \"" + name +
                                  "\" with the processor. It does not exist." );
                throw program_unload_exception( error );
            }
        if ( !it->second->isStopped() ) {
                std::string error(
                                  "Could not unload Program \"" + name +
                                  "\" with the processor. It is still running." );
                throw program_unload_exception( error );
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
        {
            MutexLock lock( *loadmonitor );
            for( program_iter it = programs->begin(); it != programs->end(); ++it)
                it->second->stop();
        }
    }

	void ProgramProcessor::step()
    {
        {
            MutexLock lock( *loadmonitor );

            //Execute all normal programs->
            for(program_iter it=programs->begin(); it != programs->end(); ++it)
                it->second->execute();
        }

        // Execute any Function :
        {
            // 0. find an empty spot to store :
            ProgramInterface* foo = 0;
            std::vector<ProgramInterface*>::iterator f_it = find(funcs.begin(), funcs.end(), foo );
            // 1. Fetch new ones from queue.
            while ( !f_queue->isEmpty() && f_it != funcs.end() ) {
                f_queue->dequeue( foo );
                // decrement semaphore if looping, but be sure not to block in any case
                if ( f_queue_sem )
                    f_queue_sem->trywait();
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
        if (this->getTask() && this->getTask()->isRunning() && f) {
            f->setProgramProcessor(this);
            if ( f->start() == false)
                return false;
            int result = f_queue->enqueue( f );
            if ( f_queue_sem )
                f_queue_sem->signal();
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
        std::vector<std::string> ret;
        for ( cprogram_iter i = programs->begin(); i != programs->end(); ++i )
            ret.push_back( i->first );
        return ret;
    }

    const ProgramInterfacePtr ProgramProcessor::getProgram(const std::string& name) const
    {
        program_iter it =
            programs->find( name );

        if ( it != programs->end() )
            return it->second;
        return ProgramInterfacePtr();
    }

    ProgramInterfacePtr ProgramProcessor::getProgram(const std::string& name)
    {
        program_iter it =
            programs->find( name );

        if ( it != programs->end() )
            return it->second;
        return ProgramInterfacePtr();
    }
}


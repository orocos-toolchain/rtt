#ifndef ORO_TASK_CONTEXT_HPP
#define ORO_TASK_CONTEXT_HPP

#include <os/MutexLock.hpp>

#include "TemplateFactories.hpp"
#include "Factories.hpp"
#include "Processor.hpp"
#include "ValueRepository.hpp"

#include <string>
#include "mystd.hpp"

namespace ORO_Execution
{
    /**
     * A TaskContext groups the operations, events, datasources,
     * peer-tasks and processor a task has. It groups logically related classes
     * and allows to iterate over its peers.
     *
     * When a peer is added, the (script) programs of this task can access
     * the peer using peername.methodname() or peername.objname.methodname().
     * Peer commands should always be executed by the peer's Processor.
     *
     * It is assumed that the RunnableInterface of the TaskInterface
     * instance of this TaskContext invokes
     * the Processor::doStep() method in its RunnableInterface::step() method.
     * In this way, the user of this class can determine himself at which
     * point and at which moment remote commands and local programs can be executed.
     */
    class TaskContext
    {
        Processor*     _task_proc;
        std::string    _task_name;
    
        typedef std::map< std::string, TaskContext* > PeerMap;
        PeerMap         _task_map;

        ORO_OS::Mutex execguard;
    public:
        /**
         * Create a TaskContext visible with \a name, which commands are processed
         * by \a proc and which is run by \a run.
         */
        TaskContext(const std::string& name, Processor* proc )
            : _task_proc( proc ), _task_name(name)
        {
            _task_map[ "this" ] = this;
        }

        ~TaskContext()
        {
            // it is not possible to remove ourselves from our peers, since
            // they we do not know who they are. A callback / event mechanism
            // could be used though, but parsed programs would still contain
            // pointers to non existing peers.
        }

        /**
         * Execute or queue a command.
         * If processor->getTask() && processor->getTask()->isRunning(), the 
         * command \a c will be queued and executed by the processor, otherwise,
         * the command is executed directly.
         */
        bool executeCommand( CommandInterface* c)
        {
            if ( _task_proc->getTask() && _task_proc->getTask()->isRunning() )
                return _task_proc->process( c );
            else {
                ORO_OS::MutexLock lockit( execguard );
                return c->execute();
            }
            return true;
        }

        const std::string& getName()
        {
            return _task_name;
        }

        /**
         * Add a one-way connection from this task to a peer task.
         */
        bool addPeer( TaskContext* peer )
        {
            if ( _task_map.count( peer->getName() ) != 0 )
                return false;
            _task_map[ peer->getName() ] = peer;
            return true;
        }

        /**
         * Remove a one-way connection from this task to a peer task.
         */
        void removePeer( const std::string& name )
        {
            if ( _task_map.end() != _task_map.find( name ) )
                _task_map.erase( _task_map.find( name ) );
        }

        /**
         * Add a two-way connection from  this task to a peer task.
         * This method is strict : both peers must not be connected to succeed.
         */
        bool connectPeers( TaskContext* peer )
        {
            if ( _task_map.count( peer->getName() ) != 0
                 || peer->hasPeer( _task_name ) )
                return false;
            _task_map[ peer->getName() ] = peer;
            peer->addPeer ( this );
            return true;
        }

        /**
         * Remove a two-way connection from this task to a peer task.
         * This method is strict : both peers must be connected to succeed.
         */
        void disconnectPeers( const std::string& name )
        {
            if ( _task_map.end() != _task_map.find( name ) ) {
                TaskContext* peer = _task_map.find(name)->second;
                if ( peer->hasPeer( _task_name ) ) {
                    _task_map.erase( _task_map.find( name ) );
                    peer->removePeer( _task_name );
                }
            }
        }

        std::vector<std::string> getPeerList() const
        {
            std::vector<std::string> res;
            std::transform(_task_map.begin(), _task_map.end(),
                           std::back_inserter( res ),
                           mystd::select1st<PeerMap::value_type>() );
            return res;
        }

        bool hasPeer( const std::string& peer_name ) const
        {
            return _task_map.count( peer_name ) == 1;
        }

        TaskContext* getPeer(const std::string& peer_name ) const
        {
            if (this->hasPeer( peer_name ) )
                return _task_map.find(peer_name)->second;
            return 0;
        }

        /**
         * Returns the processor of our task.
         */
        Processor* getProcessor() const
        {
            return _task_proc;
        }

        /**
         * The commandFactory of this TaskContext.
         */
        GlobalCommandFactory    commandFactory;
        GlobalDataSourceFactory dataFactory;
        GlobalMethodFactory     methodFactory;

        /**
         * The task-local values ( variables) of this TaskContext.
         */
        ValueRepository         valueRepository;
    };
}

#endif

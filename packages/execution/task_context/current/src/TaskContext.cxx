
#include "execution/TaskContext.hpp"
#include <corelib/CommandInterface.hpp>
#include <os/MutexLock.hpp>

#include "execution/Factories.hpp"
#include "execution/Processor.hpp"

#include <string>
#include "execution/mystd.hpp"

namespace ORO_Execution
{
        TaskContext::TaskContext(const std::string& name, Processor* proc )
            : _task_proc( proc == 0 ? new Processor : proc  ),
              _task_proc_owner( proc == 0 ? true : false ),
              _task_name(name)
        {
            _task_map[ "this" ] = this;
        }

        TaskContext::~TaskContext()
        {
            // it is not possible to remove ourselves from our peers, since
            // they we do not know who they are. A callback / event mechanism
            // could be used though, but parsed programs would still contain
            // pointers to non existing peers.
            if ( _task_proc_owner )
                delete _task_proc;
        }

        bool TaskContext::executeCommand( CommandInterface* c)
        {
            if ( _task_proc->getTask() && _task_proc->getTask()->isRunning() )
                return _task_proc->process( c ) != 0;
            else {
                ORO_OS::MutexLock lockit( execguard );
                return c->execute();
            }
            return true;
        }

        int TaskContext::queueCommand( CommandInterface* c)
        {
            return _task_proc->process( c );
        }

        bool TaskContext::addPeer( TaskContext* peer )
        {
            if ( _task_map.count( peer->getName() ) != 0 )
                return false;
            _task_map[ peer->getName() ] = peer;
            return true;
        }

        void TaskContext::removePeer( const std::string& name )
        {
            if ( _task_map.end() != _task_map.find( name ) )
                _task_map.erase( _task_map.find( name ) );
        }

        bool TaskContext::connectPeers( TaskContext* peer )
        {
            if ( _task_map.count( peer->getName() ) != 0
                 || peer->hasPeer( _task_name ) )
                return false;
            _task_map[ peer->getName() ] = peer;
            peer->addPeer ( this );
            return true;
        }

        void TaskContext::disconnectPeers( const std::string& name )
        {
            if ( _task_map.end() != _task_map.find( name ) ) {
                TaskContext* peer = _task_map.find(name)->second;
                if ( peer->hasPeer( _task_name ) ) {
                    _task_map.erase( _task_map.find( name ) );
                    peer->removePeer( _task_name );
                }
            }
        }

        std::vector<std::string> TaskContext::getPeerList() const
        {
            std::vector<std::string> res;
            std::transform(_task_map.begin(), _task_map.end(),
                           std::back_inserter( res ),
                           mystd::select1st<PeerMap::value_type>() );
            return res;
        }

        bool TaskContext::hasPeer( const std::string& peer_name ) const
        {
            return _task_map.count( peer_name ) == 1;
        }

        TaskContext* TaskContext::getPeer(const std::string& peer_name ) const
        {
            if (this->hasPeer( peer_name ) )
                return _task_map.find(peer_name)->second;
            return 0;
        }

}


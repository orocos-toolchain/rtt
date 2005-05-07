/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TaskContext.cxx 

                        TaskContext.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 

#include "execution/TaskContext.hpp"
#include <corelib/CommandInterface.hpp>

#include "execution/Factories.hpp"
#include "execution/Processor.hpp"

#include <string>
#include "execution/mystd.hpp"
#include "execution/DataSource.hpp"
#include "execution/TaskAttribute.hpp"

namespace ORO_Execution
{

        TaskContext::TaskContext(const std::string& name, Processor* proc )
            :  _task_proc_owner( proc == 0 ? true : false ),
               _task_proc( proc == 0 ? new Processor : proc  ),
              _task_name(name)
        {
            // I'll only add this line if there is a good reason for,
            // for now, it's confusing to see 'this' being listed as peer.
            //_task_map[ "this" ] = this;
        }


        TaskContext::~TaskContext()
        {
            // it is not possible to remove ourselves from our peers, since
            // they we do not know who they are. A callback / event mechanism
            // could be used though, but parsed programs would still contain
            // pointers to non existing peers.
            if ( _task_proc_owner )
                delete _task_proc;
            attributeRepository.clear();
        }

        bool TaskContext::executeCommand( CommandInterface* c)
        {
            return _task_proc->process( c ) != 0;
        }

        int TaskContext::queueCommand( CommandInterface* c)
        {
            return _task_proc->process( c );
        }

        bool TaskContext::addPeer( TaskContext* peer, std::string alias )
        {
            if ( alias.empty() )
                alias = peer->getName();
            if ( _task_map.count( alias ) != 0 )
                return false;
            _task_map[ alias ] = peer;
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


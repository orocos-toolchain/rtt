/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ExpressionServer.cxx 

                        ExpressionServer.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#include "corba/ExpressionServer.hpp"
#include "corelib/Logger.hpp"
#include "corba/ExecutionI.h"

namespace ORO_Corba
{
    using namespace ORO_CoreLib;


    ExpressionServer::EServantMap ExpressionServer::EServants;
    ExpressionServer::EServerMap ExpressionServer::EServers;
    ExpressionServer::AServerMap ExpressionServer::AServers;
    ExpressionServer::MServerMap ExpressionServer::MServers;

    Orocos::Expression_ptr ExpressionServer::CreateExpression( ORO_CoreLib::DataSourceBase::const_ptr expr ) {
        // try to retrieve:
        Orocos::Expression_ptr result = EServers[ expr ];
        if (result )
            return Orocos::Expression::_duplicate(result);
        // create new:
        Logger::log() <<Logger::Debug<< "Created 'Any' Expression server for type "<< expr->getType()<<Logger::endl;
        Orocos_AnyExpression_i* newexpr = new Orocos_AnyExpression_i( expr );
        EServants[expr] = newexpr;
        EServers[expr] = newexpr->_this();
        return Orocos::Expression::_duplicate( EServers[expr] );
    }


    Orocos::AssignableExpression_ptr ExpressionServer::CreateAssignableExpression( ORO_CoreLib::DataSourceBase::shared_ptr expr ){
        // try to retrieve:
        Orocos::AssignableExpression_ptr result = AServers[ expr ];
        if (result )
            return Orocos::AssignableExpression::_duplicate(result);
        // create new:
        Logger::log() <<Logger::Debug<< "Created 'Any' Assignable Expression server for type "<< expr->getType()<<Logger::endl;
        Orocos_AnyAssignableExpression_i* newexpr = new Orocos_AnyAssignableExpression_i( expr );
        AServers[expr] = newexpr->_this();
        EServants[expr] = newexpr;
        EServers[expr] = Orocos::AssignableExpression::_narrow(AServers[expr]);
        return Orocos::AssignableExpression::_duplicate( AServers[expr] );
    }

    Orocos::Method_ptr ExpressionServer::CreateMethod( ORO_CoreLib::DataSourceBase::shared_ptr expr ) {
        // try to retrieve:
        Orocos::Method_ptr result = MServers[ expr ];
        if (result )
            return Orocos::Method::_duplicate(result);
        // create new:
        Logger::log() <<Logger::Debug<< "Created 'Any' Method server for type "<< expr->getType()<<Logger::endl;
        Orocos_AnyMethod_i* newexpr = new Orocos_AnyMethod_i( expr );
        MServers[expr] = newexpr->_this();
        EServants[expr] = newexpr;
        EServers[expr] = Orocos::Expression::_narrow(MServers[expr]);
        return Orocos::Method::_duplicate( MServers[expr] );
    }

    void ExpressionServer::copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned )
    {
        /**
         * Inform servants. All servants are present in the EServantMap.
         */
        // check map for clones and replace with 'new'.
        EServantMap sclone_map;
        for ( EServantMap::iterator it = EServants.begin(); it != EServants.end(); ) {
            if ( alreadyCloned.count( it->first.get() ) ) {
                const ORO_CoreLib::DataSourceBase* old_ds = it->first.get();
                ORO_CoreLib::DataSourceBase* new_ds = alreadyCloned[ old_ds ];
                ++it;
                if ( old_ds == new_ds )
                    continue; // intercept non-clone cases.
                // store server in temp map.
                sclone_map[ new_ds ] = EServants[ old_ds ];
                // update servant pointer to new ds:
                sclone_map[ new_ds ]->copy( new_ds );
                // this erases 'old' iterator position.
                EServants.erase( old_ds );
            } else
                ++it;
        }
        // now populate the map with the new clones:
        EServants.insert( sclone_map.begin(), sclone_map.end() );
            
        /**
         * Update maps
         */
        // check map for clones and replace with 'new'.
        EServerMap eclone_map;
        for ( EServerMap::iterator it = EServers.begin(); it != EServers.end(); ) {
            if ( alreadyCloned.count( it->first.get() ) ) {
                const ORO_CoreLib::DataSourceBase* old_ds = it->first.get();
                ORO_CoreLib::DataSourceBase* new_ds = alreadyCloned[ old_ds ];
                ++it;
                if ( old_ds == new_ds )
                    continue; // intercept non-clone cases.
                // store server in temp map.
                eclone_map[ new_ds ] = EServers[ old_ds ];
                // this erases 'old' iterator position.
                EServers.erase( old_ds );
            } else
                ++it;
        }
        // now populate the map with the new clones:
        EServers.insert( eclone_map.begin(), eclone_map.end() );

        // check map for clones and replace with 'new'.
        AServerMap aclone_map;
        for ( AServerMap::iterator it = AServers.begin(); it != AServers.end(); ) {
            if ( alreadyCloned.count( it->first.get() ) ) {
                ORO_CoreLib::DataSourceBase* old_ds = it->first.get();
                ORO_CoreLib::DataSourceBase* new_ds = alreadyCloned[ old_ds ];
                ++it;
                if ( old_ds == new_ds )
                    continue; // intercept non-clone cases.
                // store server in temp map.
                aclone_map[ new_ds ] = AServers[ old_ds ];
                // this erases 'old' iterator position.
                AServers.erase( old_ds );
            } else
                ++it;
        }
        // now populate the map with the new clones:
        AServers.insert( aclone_map.begin(), aclone_map.end() );

        // check map for clones and replace with 'new'.
        MServerMap mclone_map;
        for ( MServerMap::iterator it = MServers.begin(); it != MServers.end(); ) {
            if ( alreadyCloned.count( it->first.get() ) ) {
                ORO_CoreLib::DataSourceBase* old_ds = it->first.get();
                ORO_CoreLib::DataSourceBase* new_ds = alreadyCloned[ old_ds ];
                ++it;
                if ( old_ds == new_ds )
                    continue; // intercept non-clone cases.
                // store server in temp map.
                mclone_map[ new_ds ] = MServers[ old_ds ];
                // this erases 'old' iterator position.
                MServers.erase( old_ds );
            } else
                ++it;
        }
        // now populate the map with the new clones:
        MServers.insert( mclone_map.begin(), mclone_map.end() );

    }

}

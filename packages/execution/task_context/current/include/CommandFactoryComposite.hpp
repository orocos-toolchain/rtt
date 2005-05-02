/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  CommandFactoryComposite.hpp 

                        CommandFactoryComposite.hpp -  description
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
 
 

#ifndef COMMANDFACTORYCOMPOSITE_HPP
#define COMMANDFACTORYCOMPOSITE_HPP

#include "CommandFactoryInterface.hpp"

namespace ORO_Execution
{
    /**
     * Used by the GlobalFactory to merge two
     * CommandFactories for one object.
     */
    class CommandFactoryComposite
        : public CommandFactoryInterface
    {
        const CommandFactoryInterface* one, *other;
    public:
        CommandFactoryComposite( const CommandFactoryInterface* f,
                                 const CommandFactoryInterface* s)
            : one(f), other(s)
        {}

        ~CommandFactoryComposite() {
            delete one;
            delete other;
        }

        bool hasCommand(const std::string& com) const 
        {
            return one->hasCommand(com) || other->hasCommand(com);
        }

        std::vector<std::string> getCommandList() const
        {
            std::vector<std::string> res = one->getCommandList();
            std::vector<std::string> res2 = other->getCommandList();
            res.insert( res.end(), res2.begin(), res2.end() );
            return res;
        }

        std::string getResultType( const std::string& com ) const
        {
            if ( one->hasCommand( com ) )
                return one->getResultType(com);
            return other->getResultType(com);
        }

        std::string getDescription( const std::string& com ) const
        {
            if ( one->hasCommand( com ) )
                return one->getDescription(com);
            return other->getDescription(com);
        }

        ORO_CoreLib::PropertyBag
        getArgumentSpec( const std::string& command ) const
        {
            if ( one->hasCommand( command ) )
                return one->getArgumentSpec(command);
            return other->getArgumentSpec(command);
        }

        std::vector< ArgumentDescription > getArgumentList( const std::string& command ) const
        {
            if ( one->hasCommand( command ) )
                return one->getArgumentList(command);
            return other->getArgumentList(command);
        }

        ComCon create( const std::string& command,
                       const ORO_CoreLib::PropertyBag& args, bool asyn=true ) const
        {
            if ( one->hasCommand( command ) )
                return one->create(command, args, asyn);
            return other->create(command, args, asyn);
        }

        ComCon create(
                      const std::string& command,
                      const std::vector<ORO_CoreLib::DataSourceBase*>& args, bool asyn=true ) const
        {
            if ( one->hasCommand( command ) )
                return one->create(command, args, asyn);
            return other->create(command, args, asyn);
        }
    };
};

#endif

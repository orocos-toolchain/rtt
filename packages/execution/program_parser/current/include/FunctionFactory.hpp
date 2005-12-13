/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  FunctionFactory.hpp 

                        FunctionFactory.hpp -  description
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
 
 
#ifndef FUNCTIONFACTORY_HPP
#define FUNCTIONFACTORY_HPP

#include "CommandFactoryInterface.hpp"
#include "corelib/Property.hpp"
#include <map>
#include <string>
#include "ProgramInterface.hpp"

namespace ORO_Execution
{
    class ExecutionEngine;

    using ORO_CoreLib::Property;
    using ORO_CoreLib::PropertyBag;

    /**
     * A CommandFactory which delivers commands which execute
     * a FunctionGraph in a Processor.
     */
    class FunctionFactory
        : public CommandFactoryInterface
    {
        typedef std::map<std::string, ProgramInterfacePtr > map_t;
        map_t funcmap;
        ExecutionEngine* proc;
    public:
        FunctionFactory(ExecutionEngine* procs);
        ~FunctionFactory();

        void addFunction(const std::string& name, ProgramInterfacePtr f ) ;

        bool hasCommand(const std::string& com) const ;

        std::vector<std::string> getCommandList() const;

        std::string getResultType( const std::string& com ) const;
        std::string getDescription( const std::string& com ) const;

        ORO_CoreLib::PropertyBag
        getArgumentSpec( const std::string& command ) const;

        std::vector< ArgumentDescription > getArgumentList( const std::string& command ) const;

        ComCon create( const std::string& command,
                       const ORO_CoreLib::PropertyBag& args,
                       bool asyn = true ) const;

        ComCon create(const std::string& command,
                      const std::vector<ORO_CoreLib::DataSourceBase*>& args,
                      bool asyn = true ) const;

        ComCon create(const std::string& command,
                      const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args,
                      bool asyn = true ) const;
    };
}

#endif

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

#include "OperationFactory.hpp"
#include "Property.hpp"
#include <map>
#include <string>
#include "ProgramInterface.hpp"
#include "DispatchInterface.hpp"

namespace RTT
{
    class ExecutionEngine;

    
    

    /**
     * A CommandFactory which delivers commands which execute
     * a FunctionGraph in a Processor.
     */
    class FunctionFactory
        : public detail::OperationFactoryPart<DispatchInterface*>
    {
        ProgramInterfacePtr func;
        ExecutionEngine* proc;
    public:
        FunctionFactory(ProgramInterfacePtr func, ExecutionEngine* procs);

        std::string resultType() const;

        PropertyBag
        getArgumentSpec() const;

        std::vector< ArgumentDescription > getArgumentList() const;

        int arity() const;

        DispatchInterface* produce(const std::vector<DataSourceBase::shared_ptr>& args
                                   ) const;
    };
}

#endif

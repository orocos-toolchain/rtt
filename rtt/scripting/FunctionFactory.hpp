/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  FunctionFactory.hpp

                        FunctionFactory.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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


#ifndef FUNCTIONFACTORY_HPP
#define FUNCTIONFACTORY_HPP

#include "../ArgumentDescription.hpp"
#include "../OperationInterface.hpp"
#include "../Property.hpp"
#include "ProgramInterface.hpp"

#include <map>
#include <string>

namespace RTT
{ namespace scripting {

    /**
     * A Factory which delivers operations which execute
     * a FunctionGraph in an engine..
     */
    class FunctionFactory
        : public OperationInterfacePart
    {
        ProgramInterfacePtr func;
        ExecutionEngine* proc;

        base::DataSourceBase::shared_ptr produceHelper(
                      const std::vector<base::DataSourceBase::shared_ptr>& args
                      , ExecutionEngine* caller, bool issend
                      ) const;
    public:
        FunctionFactory(ProgramInterfacePtr func, ExecutionEngine* procs);

        std::string resultType() const;

        unsigned int arity() const;

        unsigned int collectArity() const;

        const types::TypeInfo* getArgumentType(unsigned int arg) const;

        const types::TypeInfo* getCollectType(unsigned int arg) const;

        std::string getName() const;

        std::string description() const;

        std::vector< ArgumentDescription > getArgumentList() const;

        base::DataSourceBase::shared_ptr produce(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller
                                   ) const;
        base::DataSourceBase::shared_ptr produceHandle() const;
        base::DataSourceBase::shared_ptr produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller
                                   ) const;
        base::DataSourceBase::shared_ptr produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking
                                   ) const;

#ifdef ORO_SIGNALLING_OPERATIONS
        Handle produceSignal(base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* subscriber) const;
#endif
    };
}}

#endif

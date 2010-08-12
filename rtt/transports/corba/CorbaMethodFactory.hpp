/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  CorbaMethodFactory.hpp

                        CorbaMethodFactory.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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



#ifndef ORO_CORBAMETHODFACTORY_HPP
#define ORO_CORBAMETHODFACTORY_HPP

#include "corba.h"
#ifdef CORBA_IS_TAO
#include <tao/PortableServer/PortableServer.h>
#endif
#include "../../interface/OperationRepository.hpp"
#include "../../interface/FactoryExceptions.hpp"
#include "ServiceProviderC.h"
#include "CorbaConversion.hpp"
#include "CorbaTypeTransporter.hpp"

namespace RTT
{namespace corba
{

    /**
     * A local factory for creating remote Corba methods.
     * It connects to an corba::Service and translates
     * C++ calls to corba idl.
     */
    class RTT_CORBA_API CorbaMethodFactory
        : public RTT::interface::OperationRepositoryPart
    {
        corba::CService_var mfact;
        PortableServer::POA_var mpoa;
        std::string method;
    public:
        typedef std::vector<base::DataSourceBase::shared_ptr> CArguments;
        typedef std::vector<std::string> Members;
        typedef std::vector< interface::ArgumentDescription > CDescriptions;

        CorbaMethodFactory( const std::string& method_name, corba::CService_ptr fact, PortableServer::POA_ptr the_poa );

        virtual ~CorbaMethodFactory();

        virtual unsigned int arity() const;

        virtual const types::TypeInfo* getArgumentType(unsigned int i) const;

        virtual const types::TypeInfo* getCollectType(unsigned int i) const;

        virtual unsigned int collectArity() const;

        virtual std::string resultType() const;

        virtual std::string description() const;

        virtual std::vector< interface::ArgumentDescription > getArgumentList() const;

        virtual base::DataSourceBase::shared_ptr produce(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const ;
        virtual base::DataSourceBase::shared_ptr produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const;

        virtual base::DataSourceBase::shared_ptr produceHandle() const;

        virtual base::DataSourceBase::shared_ptr produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking) const;

        Handle produceSignal(base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args) const;
    };

}}

#endif

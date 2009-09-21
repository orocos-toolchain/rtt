/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  CORBAExpression.hpp

                        CORBAExpression.hpp -  description
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


#ifndef ORO_CORBAEXPRESSION_HPP
#define ORO_CORBAEXPRESSION_HPP

#include "../../internal/DataSource.hpp"
#include "../../Logger.hpp"
#include "../../types/BuildType.hpp"
#include "../../base/ActionInterface.hpp"
#include "../../scripting/CommandBinary.hpp"
#include "CorbaConversion.hpp"
#include "CorbaTypeTransporter.hpp"
#include <cassert>

namespace RTT
{namespace corba
{
    struct  UpdatedCommand : public ::RTT::base::ActionInterface
    {
        base::DataSourceBase::shared_ptr mds;
        UpdatedCommand( base::DataSourceBase* ds )
            :mds(ds)
        {}

        bool execute() {
            mds->updated();
            return true;
        }

        void readArguments() {}

        ::RTT::base::ActionInterface* clone() const {
            return new UpdatedCommand(mds.get());
        }

        ::RTT::base::ActionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
            return new UpdatedCommand(mds->copy(alreadyCloned));
        }
    };


    /**
     * Mirrors a remote expression object.
     */
    template<class T>
    class CORBAExpression
        : public internal::DataSource<T>
    {
        corba::CExpression_var mexpr;
        mutable typename internal::DataSource<T>::value_t last_value;
        CorbaTypeTransporter* ctp;

    public:
        CORBAExpression( corba::CExpression_ptr expr )
            : mexpr( corba::CExpression::_duplicate( expr ) ), last_value()
        {
            assert( !CORBA::is_nil(mexpr) );
            types::TypeTransporter* tp = this->getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
            ctp = dynamic_cast<corba::CorbaTypeTransporter*>(tp);
            assert(ctp);
        }

        void* server(int p, void* arg)
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return corba::CExpression::_duplicate( mexpr );
            return 0;
        }

        void* server(int p, void* arg) const
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return corba::CExpression::_duplicate( mexpr );
            return 0;
        }

        typename internal::DataSource<T>::result_t value() const {
            return last_value;
        }

        virtual typename internal::DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            internal::ReferenceDataSource<T> rds(last_value);
            rds.ref();
            if ( ctp->updateFromAny(&res.in(),&rds ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAExpression to remote value!"<<Logger::endl;
            return last_value;
        }

        virtual internal::DataSource<T>* clone() const {
            return new CORBAExpression<T>( corba::CExpression::_duplicate( mexpr.in() ) );
        }

        virtual internal::DataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAExpression<T>*>(this);
            return const_cast<CORBAExpression<T>*>(this);
        }

        virtual std::string getType() const {
            // both should be equivalent, but we display the local type.
            return internal::DataSource<T>::GetType();
            //return std::string( mexpr->getType() );
        }

        virtual int serverProtocol() const
        {
            return ORO_CORBA_PROTOCOL_ID;
        }
    };

    /**
     * Specialisation for void DataSources.
     */
    template<>
    class CORBAExpression<void>
        : public internal::DataSource<void>
    {
        corba::CExpression_var mexpr;
    public:
        CORBAExpression( corba::CExpression_ptr expr )
            : mexpr( corba::CExpression::_duplicate( expr ) )
        {
            assert( expr );
        }

        void* server(int p, void* arg)
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return corba::CExpression::_duplicate( mexpr );
            return 0;
        }

        void* server(int p, void* arg) const
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return corba::CExpression::_duplicate( mexpr );
            return 0;
        }

        void value() const {
        }

        virtual void get() const {
            // need var in order not to leak any...
            CORBA::Any_var res = mexpr->get();
        }

        virtual internal::DataSource<void>* clone() const {
            return new CORBAExpression<void>( corba::CExpression::_duplicate( mexpr.in() ) );
        }

        virtual internal::DataSource<void>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAExpression<void>*>(this);
            return const_cast<CORBAExpression<void>*>(this);
        }

        virtual int serverProtocol() const
        {
            return ORO_CORBA_PROTOCOL_ID;
        }
    };

    /**
     * Mirrors a remote assignable expression.
     */
    template<class T>
    class CORBAAssignableExpression
        : public internal::AssignableDataSource<T>
    {
        typedef typename internal::AssignableDataSource<T>::value_t value_t;
        corba::CAssignableExpression_var mexpr;
        typename internal::AssignableDataSource<value_t>::shared_ptr storage;
        CorbaTypeTransporter* ctp;
        //mutable typename internal::DataSource<T>::value_t last_value;
    public:
        CORBAAssignableExpression( corba::CAssignableExpression_ptr expr )
            : mexpr( corba::CAssignableExpression::_duplicate(expr) )//, last_value()
        {
            storage = types::BuildType<value_t>::Value();
            assert( expr );
            types::TypeTransporter* tp = this->getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
            ctp = dynamic_cast<corba::CorbaTypeTransporter*>(tp);
            assert(ctp);
        }

        void* server(int p, void* arg)
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return corba::CExpression::_duplicate( mexpr );
            return 0;
        }

        void* server(int p, void* arg) const
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return corba::CExpression::_duplicate( mexpr );
            return 0;
        }

        typename internal::DataSource<T>::result_t value() const {
            return storage->rvalue();
        }

        typename internal::AssignableDataSource<T>::const_reference_t rvalue() const {
            return storage->rvalue();
        }


        virtual typename internal::DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            internal::ReferenceDataSource<T> rds( storage->set() );
            rds.ref();
            if ( ctp->updateFromAny(&res.in(), &rds ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAAssignableExpression to remote value!"<<Logger::endl;
            return storage->rvalue();
        }

        virtual void set( typename internal::AssignableDataSource<T>::param_t t ) {
            internal::ValueDataSource<T> vds(t);
            vds.ref();
            CORBA::Any_var toset = ctp->createAny(&vds);
            mexpr->set( toset.in() );
            storage->set( t );
        }

        virtual typename internal::AssignableDataSource<T>::reference_t set() {
            return storage->set();
        }

        virtual void updated()
        {
            internal::ValueDataSource<T> vds( storage->value() );
            vds.ref();
            CORBA::Any_var toset = ctp->createAny(&vds);
            mexpr->set( toset.in() );
        }

        using internal::AssignableDataSource<T>::update;

        virtual bool update(const CORBA::Any& any) {
            // send update and get result back.
            if ( mexpr->set( any ) ) {
                this->get();
                return true;
            }
            return false;
        }

        ::RTT::base::ActionInterface* updateCommand( base::DataSourceBase* other)
        {
            ::RTT::base::ActionInterface* ci = storage->updateCommand(other);
            if (ci)
                return new scripting::CommandBinary( ci, new UpdatedCommand( this ) );
            return 0;
        }

        virtual ::RTT::base::ActionInterface* updatePartCommand(base::DataSourceBase* index, base::DataSourceBase* rhs )
        {
            ::RTT::base::ActionInterface* ci = storage->updatePartCommand(index, rhs);
            if (ci)
                return new scripting::CommandBinary( ci, new UpdatedCommand( this ) );
            return 0;
        }

        virtual internal::AssignableDataSource<T>* clone() const {
            return new CORBAAssignableExpression<T>( corba::CAssignableExpression::_duplicate( mexpr.in() ) );
        }

        virtual internal::AssignableDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAAssignableExpression<T>*>(this);
            return const_cast<CORBAAssignableExpression<T>*>(this);
        }

        virtual int serverProtocol() const
        {
            return ORO_CORBA_PROTOCOL_ID;
        }
    };

}}

#endif

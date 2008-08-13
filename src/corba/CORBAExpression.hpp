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

#include "../DataSource.hpp"
#include "../Logger.hpp"
#include "../BuildType.hpp"
#include "../CommandInterface.hpp"
#include "../CommandBinary.hpp"
#include "CorbaConversion.hpp"
#include <cassert>

namespace RTT
{namespace Corba
{
    struct  UpdatedCommand : public ::RTT::CommandInterface
    {
        DataSourceBase::shared_ptr mds;
        UpdatedCommand( DataSourceBase* ds )
            :mds(ds)
        {}

        bool execute() {
            mds->updated();
            return true;
        }

        void readArguments() {}

        ::RTT::CommandInterface* clone() const {
            return new UpdatedCommand(mds.get());
        }

        ::RTT::CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new UpdatedCommand(mds->copy(alreadyCloned));
        }
    };


    /**
     * Mirrors a remote expression object.
     */
    template<class T>
    class CORBAExpression
        : public DataSource<T>
    {
        Corba::Expression_var mexpr;
        mutable typename DataSource<T>::value_t last_value;
    public:
        CORBAExpression( Corba::Expression_ptr expr )
            : mexpr( Corba::Expression::_duplicate( expr ) ), last_value()
        {
            assert( mexpr.in() );
        }

        void* server(int p, void* arg)
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return Corba::Expression::_duplicate( mexpr );
            return 0;
        }

        void* server(int p, void* arg) const
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return Corba::Expression::_duplicate( mexpr );
            return 0;
        }

        typename DataSource<T>::result_t value() const {
            return last_value;
        }

        virtual typename DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            ReferenceDataSource<T> rds(last_value);
            rds.ref();
            if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &res.in() ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAExpression to remote value!"<<Logger::endl;
            return last_value;
        }

        virtual DataSource<T>* clone() const {
            return new CORBAExpression<T>( Corba::Expression::_duplicate( mexpr.in() ) );
        }

        virtual DataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAExpression<T>*>(this);
            return const_cast<CORBAExpression<T>*>(this);
        }

        virtual std::string getType() const {
            // both should be equivalent, but we display the local type.
            return DataSource<T>::GetType();
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
        : public DataSource<void>
    {
        Corba::Expression_var mexpr;
    public:
        CORBAExpression( Corba::Expression_ptr expr )
            : mexpr( Corba::Expression::_duplicate( expr ) )
        {
            assert( expr );
        }

        void* server(int p, void* arg)
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return Corba::Expression::_duplicate( mexpr );
            return 0;
        }

        void* server(int p, void* arg) const
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return Corba::Expression::_duplicate( mexpr );
            return 0;
        }

        void value() const {
        }

        virtual void get() const {
            // need var in order not to leak any...
            CORBA::Any_var res = mexpr->get();
        }

        virtual DataSource<void>* clone() const {
            return new CORBAExpression<void>( Corba::Expression::_duplicate( mexpr.in() ) );
        }

        virtual DataSource<void>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
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
        : public AssignableDataSource<T>
    {
        typedef typename AssignableDataSource<T>::value_t value_t;
        Corba::AssignableExpression_var mexpr;
        typename AssignableDataSource<value_t>::shared_ptr storage;
        //mutable typename DataSource<T>::value_t last_value;
    public:
        CORBAAssignableExpression( Corba::AssignableExpression_ptr expr )
            : mexpr( Corba::AssignableExpression::_duplicate(expr) )//, last_value()
        {
            storage = detail::BuildType<value_t>::Value();
            assert( expr );
        }

        void* server(int p, void* arg)
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return Corba::AssignableExpression::_duplicate( mexpr );
            return 0;
        }

        void* server(int p, void* arg) const
        {
            if ( p == ORO_CORBA_PROTOCOL_ID)
                return Corba::AssignableExpression::_duplicate( mexpr );
            return 0;
        }

        typename DataSource<T>::result_t value() const {
            return storage->rvalue();
        }

        typename AssignableDataSource<T>::const_reference_t rvalue() const {
            return storage->rvalue();
        }


        virtual typename DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            ReferenceDataSource<T> rds( storage->set() );
            rds.ref();
            if ( rds.updateBlob(ORO_CORBA_PROTOCOL_ID, &res.in() ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAAssignableExpression to remote value!"<<Logger::endl;
            return storage->rvalue();
        }

        virtual void set( typename AssignableDataSource<T>::param_t t ) {
            ValueDataSource<T> vds(t);
            vds.ref();
            CORBA::Any_var toset = (CORBA::Any_ptr)vds.createBlob(ORO_CORBA_PROTOCOL_ID);
            mexpr->set( toset.in() );
            storage->set( t );
        }

        virtual typename AssignableDataSource<T>::reference_t set() {
            return storage->set();
        }

        virtual void updated()
        {
            ValueDataSource<T> vds( storage->value() );
            vds.ref();
            CORBA::Any_var toset = (CORBA::Any_ptr)vds.createBlob(ORO_CORBA_PROTOCOL_ID);
            mexpr->set( toset.in() );
        }

        using AssignableDataSource<T>::update;

        virtual bool update(const CORBA::Any& any) {
            // send update and get result back.
            if ( mexpr->set( any ) ) {
                this->get();
                return true;
            }
            return false;
        }

        ::RTT::CommandInterface* updateCommand( DataSourceBase* other)
        {
            ::RTT::CommandInterface* ci = storage->updateCommand(other);
            if (ci)
                return new CommandBinary( ci, new UpdatedCommand( this ) );
            return 0;
        }

        virtual ::RTT::CommandInterface* updatePartCommand(DataSourceBase* index, DataSourceBase* rhs )
        {
            ::RTT::CommandInterface* ci = storage->updatePartCommand(index, rhs);
            if (ci)
                return new CommandBinary( ci, new UpdatedCommand( this ) );
            return 0;
        }

        virtual AssignableDataSource<T>* clone() const {
            return new CORBAAssignableExpression<T>( Corba::AssignableExpression::_duplicate( mexpr.in() ) );
        }

        virtual AssignableDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
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

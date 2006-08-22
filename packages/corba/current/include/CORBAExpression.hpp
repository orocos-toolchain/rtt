/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  CORBAExpression.hpp 

                        CORBAExpression.hpp -  description
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
 
 
#ifndef ORO_CORBAEXPRESSION_HPP
#define ORO_CORBAEXPRESSION_HPP

#include "../DataSource.hpp"
#include "../Logger.hpp"
#include "../BuildType.hpp"
#include "../CommandInterface.hpp"
#include "../CommandBinary.hpp"
#include "CorbaConversion.hpp"


namespace RTT
{namespace Corba
{
    struct  UpdatedCommand : public CommandInterface
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

        CommandInterface* clone() const {
            return new UpdatedCommand(mds.get());
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new UpdatedCommand(mds->copy(alreadyCloned));
        }
    };


    
    template<class T>
    class CORBAExpression
        : public DataSource<T>
    {
        Orocos::Expression_var mexpr;
        mutable typename DataSource<T>::value_t last_value;
    public:
        CORBAExpression( Orocos::Expression_ptr expr )
            : mexpr( Orocos::Expression::_duplicate( expr ) ), last_value()
        {}

        Orocos::Expression_ptr server()
        {
            return Orocos::Expression::_duplicate( mexpr );
        }

        Orocos::Expression_ptr server() const
        {
            return Orocos::Expression::_duplicate( mexpr );
        }

        typename DataSource<T>::result_t value() const {
            return last_value;
        }

        virtual typename DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            if (AnyConversion<T>::update( res.in(), last_value ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAExpression to remote value!"<<Logger::endl;
            return last_value;
        }

        virtual DataSource<T>* clone() const {
            return new CORBAExpression<T>( Orocos::Expression::_duplicate( mexpr.in() ) );
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

        virtual bool hasServer() const
        {
            return true;
        }
    };

    /**
     * Specialisation for void DataSources.
     */
    template<>
    class CORBAExpression<void>
        : public DataSource<void>
    {
        Orocos::Expression_var mexpr;
    public:
        CORBAExpression( Orocos::Expression_ptr expr )
            : mexpr( Orocos::Expression::_duplicate( expr ) )
        {}

        Orocos::Expression_ptr server()
        {
            return Orocos::Expression::_duplicate( mexpr );
        }

        Orocos::Expression_ptr server() const
        {
            return Orocos::Expression::_duplicate( mexpr );
        }

        void value() const {
        }

        virtual void get() const {
            // need var in order not to leak any...
            CORBA::Any_var res = mexpr->get();
        }

        virtual DataSource<void>* clone() const {
            return new CORBAExpression<void>( Orocos::Expression::_duplicate( mexpr.in() ) );
        } 

        virtual DataSource<void>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAExpression<void>*>(this);
            return const_cast<CORBAExpression<void>*>(this);
        }

        virtual bool hasServer() const
        {
            return true;
        }
    };

    template<class T>
    class CORBAAssignableExpression
        : public AssignableDataSource<T>
    {
        typedef typename AssignableDataSource<T>::value_t value_t;
        Orocos::AssignableExpression_var mexpr;
        typename AssignableDataSource<value_t>::shared_ptr storage;
        //mutable typename DataSource<T>::value_t last_value;
    public:
        CORBAAssignableExpression( Orocos::AssignableExpression_ptr expr )
            : mexpr( Orocos::AssignableExpression::_duplicate(expr) )//, last_value()
        {
            storage = detail::BuildType<value_t>::Value();
        }

        Orocos::Expression_ptr server()
        {
            return Orocos::AssignableExpression::_duplicate( mexpr );
        }
        
        Orocos::Expression_ptr server() const
        {
            return Orocos::AssignableExpression::_duplicate( mexpr );
        }

        typename DataSource<T>::result_t value() const {
            return storage->rvalue();
        }

        typename AssignableDataSource<T>::const_reference_t rvalue() const {
            return storage->rvalue();
        }


        virtual typename DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            if (AnyConversion<T>::update( res.in(), storage->set() ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAAssignableExpression to remote value!"<<Logger::endl;
            return storage->rvalue();
        }

        virtual void set( typename AssignableDataSource<T>::param_t t ) {
            CORBA::Any_var toset = AnyConversion<T>::createAny(t);
            mexpr->set( toset.in() );
            storage->set( t );
        }

        virtual typename AssignableDataSource<T>::reference_t set() {
            return storage->set();
        }

        virtual void updated()
        {
            CORBA::Any_var toset = AnyConversion<T>::createAny( storage->value() );
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

        CommandInterface* updateCommand( DataSourceBase* other) 
        {
            CommandInterface* ci = storage->updateCommand(other);
            if (ci)
                return new CommandBinary( ci, new UpdatedCommand( this ) );
            return 0;
        }

        virtual CommandInterface* updatePartCommand(DataSourceBase* index, DataSourceBase* rhs )
        {
            CommandInterface* ci = storage->updatePartCommand(index, rhs);
            if (ci)
                return new CommandBinary( ci, new UpdatedCommand( this ) );
            return 0;
        }

        virtual AssignableDataSource<T>* clone() const {
            return new CORBAAssignableExpression<T>( Orocos::AssignableExpression::_duplicate( mexpr.in() ) );
        } 

        virtual AssignableDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAAssignableExpression<T>*>(this);
            return const_cast<CORBAAssignableExpression<T>*>(this);
        }

        virtual bool hasServer() const
        {
            return true;
        }
    };

}}

#endif

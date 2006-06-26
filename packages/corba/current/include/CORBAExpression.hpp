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

#include <corelib/DataSource.hpp>
#include <corelib/Logger.hpp>
#include <corelib/BuildType.hpp>
#include <corelib/CommandInterface.hpp>
#include <execution/CommandBinary.hpp>
#include "CorbaConversion.hpp"


namespace ORO_Corba
{
    struct  UpdatedCommand : public ORO_CoreLib::CommandInterface
    {
        ORO_CoreLib::DataSourceBase::shared_ptr mds;
        UpdatedCommand( ORO_CoreLib::DataSourceBase* ds )
            :mds(ds)
        {}

        bool execute() {
            mds->updated();
            return true;
        }

        void readArguments() {}

        ORO_CoreLib::CommandInterface* clone() const {
            return new UpdatedCommand(mds.get());
        }

        ORO_CoreLib::CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            return new UpdatedCommand(mds->copy(alreadyCloned));
        }
    };


    using ORO_CoreLib::Logger;
    template<class T>
    class CORBAExpression
        : public ORO_CoreLib::DataSource<T>
    {
        Orocos::Expression_var mexpr;
        mutable typename ORO_CoreLib::DataSource<T>::value_t last_value;
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

        typename ORO_CoreLib::DataSource<T>::result_t value() const {
            return last_value;
        }

        virtual typename ORO_CoreLib::DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            if (ORO_CoreLib::AnyConversion<T>::update( res.in(), last_value ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAExpression to remote value!"<<Logger::endl;
            return last_value;
        }

        virtual ORO_CoreLib::DataSource<T>* clone() const {
            return new CORBAExpression<T>( Orocos::Expression::_duplicate( mexpr.in() ) );
        } 

        virtual ORO_CoreLib::DataSource<T>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAExpression<T>*>(this);
            return const_cast<CORBAExpression<T>*>(this);
        }

        virtual std::string getType() const {
            // both should be equivalent, but we display the local type.
            return ORO_CoreLib::DataSource<T>::GetType();
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
        : public ORO_CoreLib::DataSource<void>
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

        virtual ORO_CoreLib::DataSource<void>* clone() const {
            return new CORBAExpression<void>( Orocos::Expression::_duplicate( mexpr.in() ) );
        } 

        virtual ORO_CoreLib::DataSource<void>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
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
        : public ORO_CoreLib::AssignableDataSource<T>
    {
        typedef typename ORO_CoreLib::AssignableDataSource<T>::value_t value_t;
        Orocos::AssignableExpression_var mexpr;
        typename ORO_CoreLib::AssignableDataSource<value_t>::shared_ptr storage;
        //mutable typename ORO_CoreLib::DataSource<T>::value_t last_value;
    public:
        CORBAAssignableExpression( Orocos::AssignableExpression_ptr expr )
            : mexpr( Orocos::AssignableExpression::_duplicate(expr) )//, last_value()
        {
            storage = ORO_CoreLib::detail::BuildType<value_t>::Value();
        }

        using ORO_CoreLib::AssignableDataSource<T>::server;
        
        Orocos::Expression_ptr server()
        {
            return Orocos::AssignableExpression::_duplicate( mexpr );
        }

        typename ORO_CoreLib::DataSource<T>::result_t value() const {
            return storage->rvalue();
        }

        typename ORO_CoreLib::AssignableDataSource<T>::const_reference_t rvalue() const {
            return storage->rvalue();
        }


        virtual typename ORO_CoreLib::DataSource<T>::result_t get() const {
            CORBA::Any_var res = mexpr->get();
            if (ORO_CoreLib::AnyConversion<T>::update( res.in(), storage->set() ) == false)
                Logger::log() <<Logger::Error << "Could not update CORBAAssignableExpression to remote value!"<<Logger::endl;
            return storage->rvalue();
        }

        virtual void set( typename ORO_CoreLib::AssignableDataSource<T>::param_t t ) {
            CORBA::Any_var toset = ORO_CoreLib::AnyConversion<T>::createAny(t);
            mexpr->set( toset.in() );
            storage->set( t );
        }

        virtual typename ORO_CoreLib::AssignableDataSource<T>::reference_t set() {
            return storage->set();
        }

        virtual void updated()
        {
            CORBA::Any_var toset = ORO_CoreLib::AnyConversion<T>::createAny( storage->value() );
            mexpr->set( toset.in() );
        }

        using ORO_CoreLib::AssignableDataSource<T>::update;
        virtual bool update(const CORBA::Any& any) {
            // send update and get result back.
            if ( mexpr->set( any ) ) {
                this->get();
                return true;
            }
            return false;
        }

        ORO_CoreLib::CommandInterface* updateCommand( ORO_CoreLib::DataSourceBase* other) 
        {
            ORO_CoreLib::CommandInterface* ci = storage->updateCommand(other);
            if (ci)
                return new ORO_Execution::CommandBinary( ci, new UpdatedCommand( this ) );
            return 0;
        }

        virtual ORO_CoreLib::CommandInterface* updatePartCommand(ORO_CoreLib::DataSourceBase* index, ORO_CoreLib::DataSourceBase* rhs )
        {
            ORO_CoreLib::CommandInterface* ci = storage->updatePartCommand(index, rhs);
            if (ci)
                return new ORO_Execution::CommandBinary( ci, new UpdatedCommand( this ) );
            return 0;
        }

        virtual ORO_CoreLib::AssignableDataSource<T>* clone() const {
            return new CORBAAssignableExpression<T>( Orocos::AssignableExpression::_duplicate( mexpr.in() ) );
        } 

        virtual ORO_CoreLib::AssignableDataSource<T>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
            alreadyCloned[this] = const_cast<CORBAAssignableExpression<T>*>(this);
            return const_cast<CORBAAssignableExpression<T>*>(this);
        }

        virtual bool hasServer() const
        {
            return true;
        }
    };

}

#endif

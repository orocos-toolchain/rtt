/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  DataSource.cxx 

                        DataSource.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#include "corelib/DataSource.hpp"

#include <pkgconf/os.h>
#ifdef OROINT_OS_CORBA
#include "corelib/AnyDataSource.hpp"
// WARNING: my Debian version has 'TypeCode.h',
// the OCI version uses 'Typecode.h'... is this a typo ???
#include <tao/Typecode.h>
#endif

namespace ORO_CoreLib
{

    DataSourceBase::DataSourceBase() { atomic_set(&refcount,0); }
    void DataSourceBase::ref() const { atomic_inc(&refcount); }
    void DataSourceBase::deref() const { if ( atomic_dec_and_test(&refcount) ) delete this; }

    DataSourceBase::~DataSourceBase()
    {
    }
    
    void DataSourceBase::reset()
    {
    }

    bool DataSourceBase::update(const DataSourceBase* ) {
        return false;
    }

#ifdef OROINT_OS_CORBA
    bool DataSourceBase::hasServer() const
    {
        return false;
    }
#endif

    CommandInterface* DataSourceBase::updateCommand(const DataSourceBase* ) {
        return 0;
    }

    bool DataSourceBase::updatePart( DataSourceBase*, const DataSourceBase* ) {
        return false;
    }

    CommandInterface* DataSourceBase::updatePartCommand( DataSourceBase*, const DataSourceBase* ) {
        return 0;
    }

    bool DataSourceBase::update(const CORBA::Any& ) {
        return false;
    }

    template<>
    bool DataSource<bool>::evaluate() const
    {
        return this->get();
    }

    template<>
    CORBA::Any* DataSource<void>::createAny() const
    {
#ifdef OROINT_OS_CORBA
        return new CORBA::Any();
#endif
        return 0;
    }

    template<>
    CORBA::Any* DataSource<void>::getAny() const
    {
        this->evaluate();
#ifdef OROINT_OS_CORBA
        return new CORBA::Any();
#endif
        return 0;
    }

    template<>
    DataSource<void>* DataSource<void>::narrow(DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        DataSource<void>* ret = dynamic_cast< DataSource<void>* >( dsb );
        if (ret) return ret;
#ifdef OROINT_OS_CORBA
        // then try to see if it is a CORBA object.
        //ORO_Corba::ExpressionProxyInterface* prox = dynamic_cast< ORO_Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            ::Orocos::Expression_var expr = dsb->server() ;
            return new ORO_Corba::CORBAExpression<void>( expr.in() );
        }
#endif
        // all failed:
        return 0;
    }

    template<>
    const DataSource<void>* DataSource<void>::narrow(const DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        const DataSource<void>* ret = dynamic_cast< const DataSource<void>* >( dsb );
        if (ret) return ret;
#ifdef OROINT_OS_CORBA
        // then try to see if it is a CORBA object.
        //ORO_Corba::ExpressionProxyInterface* prox = dynamic_cast< ORO_Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            ::Orocos::Expression_var expr = dsb->server() ;
            return new ORO_Corba::CORBAExpression<void>( expr.in() );
        }
#endif
        // all failed:
        return 0;
    }


#ifdef OROINT_OS_CORBA
    template<>
    DataSource<CORBA::Any_ptr>* DataSource<CORBA::Any_ptr>::narrow(DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        DataSource<CORBA::Any_ptr>* ret = dynamic_cast< DataSource<CORBA::Any_ptr>* >( dsb );
        if (ret) return ret;
        // if it is a server, we can always just ask it's any value...
        if ( dsb->hasServer() ) {
            Logger::log() << Logger::Debug << "Narrowing server "<<dsb->getType() <<" to local CORBA::Any." <<Logger::endl;
            ::Orocos::Expression_var expr = dsb->server() ;
            return new ORO_Corba::CORBAExpression<CORBA::Any_ptr>( expr.in() );
        }
        // last resort, try to do it as 'Any':
        CORBA::Any_var any = dsb->createAny();
        if ( (any->type())->equal(CORBA::_tc_null) == false ) {
            // if other's any is meaningful (not null), create Any.
            Logger::log() << Logger::Debug << "'Emergency' narrowing DataSource "<<dsb->getType() <<" to local CORBA::Any." <<Logger::endl;
            return new detail::AnyDataSource( dsb );
        }

        // all failed:
        return 0;
    }

    template<>
    const DataSource<CORBA::Any_ptr>* DataSource<CORBA::Any_ptr>::narrow(const DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        const DataSource<CORBA::Any_ptr>* ret = dynamic_cast<const DataSource<CORBA::Any_ptr>* >( dsb );
        if (ret) return ret;
        // if it is a server, we can always just ask it's any value...
        if ( dsb->hasServer() ) {
            Logger::log() << Logger::Debug << "Narrowing server "<<dsb->getType() <<" to local CORBA::Any." <<Logger::endl;
            ::Orocos::Expression_var expr = dsb->server() ;
            return new ORO_Corba::CORBAExpression<CORBA::Any_ptr>( expr.in() );
        }
        // last resort, try to do it as 'Any':
        //CORBA::Any_var any = dsb->getAny();
        if ( true ) {
            // if other's any is meaningful (not null), create Any.
            Logger::log() << Logger::Debug << "Narrowing DataSource "<<dsb->getType() <<" to AnyDataSource." <<Logger::endl;
            return new detail::AnyDataSource( dsb );
        }

        // all failed:
        return 0;
    }

    template<>
    CommandInterface* AssignableDataSource<CORBA::Any_ptr>::updateCommand( const DataSourceBase* other) 
    {
        //CORBA::Any_ptr any = other->getAny();
        if ( true ) {
            // if other's any is meaningful (not null), create Any.
            Logger::log() << Logger::Debug << "Creating updateCommand for "<<other->getType() <<" as CORBA::Any." <<Logger::endl;
            return new detail::AssignAnyCommand<CORBA::Any_ptr>( this, other );
        }

        Logger::log() << Logger::Error << "Could not create updateCommand for "<<other->getType() <<" as CORBA::Any." <<Logger::endl;
        return 0;
    }
#endif

    namespace detail {
        const std::string DataSourceTypeInfo<ValueType>::type("unknown type");
        const std::string DataSourceTypeInfo<ValueType>::qual("");
        const std::string DataSourceTypeInfo<void>::type("void");
        const std::string DataSourceTypeInfo<bool>::type("bool");
        const std::string DataSourceTypeInfo<int>::type("int");
        const std::string DataSourceTypeInfo<unsigned int>::type("unsigned int");
        const std::string DataSourceTypeInfo<double>::type("double");
        const std::string DataSourceTypeInfo<float>::type("float");
        const std::string DataSourceTypeInfo<char>::type("char");
        const std::string DataSourceTypeInfo<PropertyBag>::type("PropertyBag");
        const std::string DataSourceTypeInfo<ORO_CoreLib::MultiVector<6,double> >::type("Double6D");
        const std::string DataSourceTypeInfo<std::string>::type("string");
        const std::string DataSourceTypeInfo<std::vector<double> >::type("array");

        const std::string DataSourceTypeInfo<CORBA::Any>::type("CORBA::Any");

        // Forward decls allow us to define these, even if the geometry package is not used.
        const std::string DataSourceTypeInfo<ORO_Geometry::Frame>::type("Frame");
        const std::string DataSourceTypeInfo<ORO_Geometry::Vector>::type("Vector");
        const std::string DataSourceTypeInfo<ORO_Geometry::Rotation>::type("Rotation");
        const std::string DataSourceTypeInfo<ORO_Geometry::Twist>::type("Twist");
        const std::string DataSourceTypeInfo<ORO_Geometry::Wrench>::type("Wrench");
    }
}

void intrusive_ptr_add_ref(const ORO_CoreLib::DataSourceBase* p )
{
  p->ref();
}

void intrusive_ptr_release(const ORO_CoreLib::DataSourceBase* p )
{
  p->deref();
}

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
#include "rtt/DataSource.hpp"
#include "rtt/TypeInfoName.hpp"

#include <pkgconf/os.h>
#ifdef OROINT_OS_CORBA
#include "rtt/AnyDataSource.hpp"
#endif
#include "rtt/TemplateTypeInfo.hpp"

namespace RTT
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

    std::ostream& DataSourceBase::write(std::ostream& os)
    {
        DataSourceBase::shared_ptr mobj(this);
        return mobj->getTypeInfo()->write( os, mobj );
    }


    std::ostream& operator<<(std::ostream& os, DataSourceBase::shared_ptr mobj)
    {
        return mobj->getTypeInfo()->write( os, mobj );
    }

    std::string DataSourceBase::toString()
    {
        DataSourceBase::shared_ptr mobj(this);
        return mobj->getTypeInfo()->toString( mobj );
    }

    bool DataSourceBase::decomposeType( PropertyBag& targetbag )
    {
        DataSourceBase::shared_ptr mobj(this);
        return mobj->getTypeInfo()->decomposeType( mobj, targetbag );
    }
            
    bool DataSourceBase::composeType( DataSourceBase::shared_ptr source)
    {
        DataSourceBase::shared_ptr mobj(this);
        return mobj->getTypeInfo()->composeType( source, mobj );
    }

    bool DataSourceBase::update( DataSourceBase* ) {
        return false;
    }

#ifdef OROINT_OS_CORBA
    bool DataSourceBase::hasServer() const
    {
        return false;
    }
#endif

    void DataSourceBase::updated()
    {}


    CommandInterface* DataSourceBase::updateCommand( DataSourceBase* ) {
        return 0;
    }

    bool DataSourceBase::updatePart( DataSourceBase*, DataSourceBase* ) {
        return false;
    }

    CommandInterface* DataSourceBase::updatePartCommand( DataSourceBase*, DataSourceBase* ) {
        return 0;
    }

    bool DataSourceBase::update(const CORBA::Any& ) {
        return false;
    }

    template<>
    bool DataSource<bool>::evaluate() const
    {
        this->get();
        return true;
    }

    template<>
    CORBA::Any* DataSource<void>::createAny()
    {
#ifdef OROINT_OS_CORBA
        return new CORBA::Any();
#endif
        return 0;
    }

    template<>
    CORBA::Any* DataSource<void>::getAny()
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
        //Corba::ExpressionProxyInterface* prox = dynamic_cast< Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            Corba::Expression_var expr = dsb->server(0) ;
            return new Corba::CORBAExpression<void>( expr.in() );
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
            Corba::Expression_var expr = dsb->server(0) ;
            return new Corba::CORBAExpression<CORBA::Any_ptr>( expr.in() );
        }
        // last resort, try to do it as 'Any':
#if 0
        // This piece of code requires 'tao/Typecode.h' or 'tao/TypeCode.h'
        CORBA::Any_var any = dsb->createAny();
        if ( (any->type())->equal(CORBA::_tc_null) == false ) {
            // if other's any is meaningful (not null), create Any.
            Logger::log() << Logger::Debug << "'Emergency' narrowing DataSource "<<dsb->getType() <<" to local CORBA::Any." <<Logger::endl;
            return new detail::AnyDataSource( dsb );
        }
        Logger::log() << Logger::Debug << "Narrowing DataSource "<<dsb->getType() <<" to AnyDataSource." <<Logger::endl;
        return new detail::AnyDataSource( dsb );
#endif
        return 0;
    }

    template<>
    CommandInterface* AssignableDataSource<CORBA::Any_ptr>::updateCommand( DataSourceBase* other) 
    {
        //CORBA::Any_ptr any = other->getAny();
        if ( true ) {
            // if other's any is meaningful (not null), create Any.
            Logger::log() << Logger::Debug << "Creating updateCommand for "<<other->getType() <<" as CORBA::Any." <<Logger::endl;
            return new detail::AssignAnyCommand<CORBA::Any_ptr>( this, other );
        }

        Logger::log() << Logger::Error << "Could not create updateCommand for "<<other->getType() <<" as CORBA::Any." <<Logger::endl;
        throw bad_assignment();

        return 0;
    }
#endif

    namespace detail {

        TypeInfo* DataSourceTypeInfo<detail::UnknownType>::TypeInfoObject = 0;

        const std::string& DataSourceTypeInfo<UnknownType>::getType() { return getTypeInfo()->getTypeName(); }
        const std::string& DataSourceTypeInfo<UnknownType>::getQualifier() { return noqual; }
        const TypeInfo* DataSourceTypeInfo<UnknownType>::getTypeInfo() { 
            if (!TypeInfoObject)
                TypeInfoObject = new TypeInfoName<UnknownType>("unknown type");
            return TypeInfoObject; 
        }

        const std::string DataSourceTypeInfo<UnknownType>::noqual("");
        const std::string DataSourceTypeInfo<UnknownType>::cqual(" const");
        const std::string DataSourceTypeInfo<UnknownType>::refqual(" &");
        const std::string DataSourceTypeInfo<UnknownType>::crefqual(" const&");
        const std::string DataSourceTypeInfo<UnknownType>::ptrqual(" *");
        const std::string DataSourceTypeInfo<UnknownType>::cptrqual(" const*");

        // (void) type info
        TypeInfo* DataSourceTypeInfo<void>::TypeInfoObject = 0;
        const std::string DataSourceTypeInfo<void>::tname("void");

        const std::string& DataSourceTypeInfo<void>::getType() { return tname; }
        const std::string& DataSourceTypeInfo<void>::getQualifier() { return DataSourceTypeInfo<UnknownType>::noqual; }
        const TypeInfo* DataSourceTypeInfo<void>::getTypeInfo() { 
            return DataSourceTypeInfo<UnknownType>::getTypeInfo();
        }

    }
}

void intrusive_ptr_add_ref(const RTT::DataSourceBase* p )
{
  p->ref();
}

void intrusive_ptr_release(const RTT::DataSourceBase* p )
{
  p->deref();
};


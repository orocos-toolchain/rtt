/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  DataSource.cxx 

                        DataSource.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#include "DataSource.hpp"
#include "TypeInfoName.hpp"

#include "rtt-config.h"
#include "Types.hpp"
#include "TypeTransporter.hpp"

namespace RTT
{

    DataSourceBase::DataSourceBase() { oro_atomic_set(&refcount,0); }
    void DataSourceBase::ref() const { oro_atomic_inc(&refcount); }
    void DataSourceBase::deref() const { if ( oro_atomic_dec_and_test(&refcount) ) delete this; }

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

    template<>
    bool DataSource<bool>::evaluate() const
    {
        this->get();
        return true;
    }

    void* DataSourceBase::createBlob(int protocol)
    {
#ifndef ORO_EMBEDDED
        detail::TypeTransporter* tt = getTypeInfo()->getProtocol(protocol);
        if ( tt )
            return tt->createBlob( DataSourceBase::shared_ptr(this) );
#endif
        return 0;
    }
    
    void* DataSourceBase::getBlob(int protocol)
    {
        this->evaluate();
#ifndef ORO_EMBEDDED
        detail::TypeTransporter* tt = getTypeInfo()->getProtocol(protocol);
        if ( tt )
            return tt->createBlob( DataSourceBase::shared_ptr(this) );
#endif
        return 0;
    }
    
    bool DataSourceBase::updateBlob(int protocol, const void* data)
    {
        return false; // overridden in AssignableDataSource<T>
    }

    int DataSourceBase::serverProtocol() const
    {
        return 0; // default to local DataSource.
    }

    void* DataSourceBase::server( int protocol, void* arg )
    {
#ifndef ORO_EMBEDDED
        detail::TypeTransporter* tt = getTypeInfo()->getProtocol(protocol);
        if ( tt )
            return tt->server( DataSourceBase::shared_ptr(this), false, arg );
#endif
        return 0;
    }

    void* DataSourceBase::method( int protocol, MethodC* orig, void* arg )
    {
#ifndef ORO_EMBEDDED
        detail::TypeTransporter* tt = getTypeInfo()->getProtocol(protocol);
        if ( tt )
            return tt->method( DataSourceBase::shared_ptr(this), orig, arg );
#endif
        return 0;
    }

    namespace detail {

        TypeInfo* DataSourceTypeInfo<detail::UnknownType>::TypeInfoObject = 0;

        const std::string& DataSourceTypeInfo<UnknownType>::getType() { return getTypeInfo()->getTypeName(); }
        const std::string& DataSourceTypeInfo<UnknownType>::getQualifier() { return noqual; }
        TypeInfo* DataSourceTypeInfo<UnknownType>::getTypeInfo() { 
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


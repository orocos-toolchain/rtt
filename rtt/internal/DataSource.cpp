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
#include "../types/TypeInfoName.hpp"

#include "rtt-config.h"
#include "../types/Types.hpp"
#include "../types/TypeTransporter.hpp"

namespace RTT {
    namespace base {
        std::ostream& operator<<(std::ostream& os, DataSourceBase::shared_ptr mobj)
        {
            return mobj->getTypeInfo()->write( os, mobj );
        }
    }

    using namespace detail;
    using namespace std;

    bad_assignment::~bad_assignment() throw() {}
    const char* bad_assignment::what() const throw() { return "Bad DataSource assignment: incompatible types."; }

    DataSourceBase::shared_ptr DataSourceBase::stack_shared_ptr(DataSourceBase* dsb) {
        dsb->ref();
        return DataSourceBase::shared_ptr(dsb);
    }

    DataSourceBase::const_ptr DataSourceBase::stack_const_ptr(DataSourceBase const* dsb) {
        dsb->ref();
        return DataSourceBase::const_ptr(dsb);
    }

    DataSourceBase::DataSourceBase() : refcount(0) {  }
    void DataSourceBase::ref() const { refcount.inc(); }
    void DataSourceBase::deref() const { if ( refcount.dec_and_test() ) delete this; }

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


    std::string DataSourceBase::toString()
    {
        DataSourceBase::shared_ptr mobj(this);
        return mobj->getTypeInfo()->toString( mobj );
    }

    bool DataSourceBase::isAssignable() const {
        return false;
    }

    bool DataSourceBase::update( DataSourceBase* ) {
        return false;
    }

    void DataSourceBase::updated()
    {}


    ActionInterface* DataSourceBase::updateAction( DataSourceBase* ) {
        return 0;
    }

    DataSourceBase::shared_ptr DataSourceBase::getPart( const std::string& part_name ) {
        if ( part_name.empty() )
            return DataSourceBase::shared_ptr(this);
        return getTypeInfo()->getPart( this, part_name );
    }

    DataSourceBase::shared_ptr DataSourceBase::getPart( DataSourceBase::shared_ptr part_id, DataSourceBase::shared_ptr offset) {
        return getTypeInfo()->getPart( this, part_id );
    }

    vector<string> DataSourceBase::getPartNames() const {
        return getTypeInfo()->getPartNames();
    }

    DataSourceBase::shared_ptr DataSourceBase::getParent()  {
        return this;
    }

    template<>
    bool DataSource<bool>::evaluate() const
    {
        this->get();
        return true;
    }

    namespace internal {

        TypeInfo* DataSourceTypeInfo<detail::UnknownType>::TypeInfoObject = 0;

        const std::string& DataSourceTypeInfo<UnknownType>::getType() { return getTypeInfo()->getTypeName(); }
        const std::string& DataSourceTypeInfo<UnknownType>::getQualifier() { return noqual; }
        TypeInfo* DataSourceTypeInfo<UnknownType>::getTypeInfo() {
            if (!TypeInfoObject)
                TypeInfoObject = new TypeInfoName<UnknownType>("unknown_t");
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
            if (!TypeInfoObject)
                return DataSourceTypeInfo<UnknownType>::getTypeInfo();
            return TypeInfoObject;
        }
    }
}

void intrusive_ptr_add_ref(const RTT::base::DataSourceBase* p )
{
  p->ref();
}

void intrusive_ptr_release(const RTT::base::DataSourceBase* p )
{
  p->deref();
};


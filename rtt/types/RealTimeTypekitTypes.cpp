/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  RealTimeToolkit.cxx

                        RealTimeToolkit.cxx -  description
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



#include "rtt-config.h"
#include "RealTimeToolkit.hpp"
#include "Types.hpp"
#include "TemplateTypeInfo.hpp"
#include "StdTypeInfo.hpp"
#include "TypeInfoName.hpp"
#include "../rtt-fwd.hpp"
#include "../FlowStatus.hpp"
#include "../ConnPolicy.hpp"

#include "TypeStream.hpp"
#include "../PropertyBag.hpp"
#include "VectorComposition.hpp"
#include "SequenceTypeInfo.hpp"
#include <ostream>


namespace RTT
{
    namespace types {
        void loadOrocosTypes(TypeInfoRepository::shared_ptr ti);
    }

    using namespace std;
    using namespace detail;

    /**
     * Write boolean as 'true' or 'false'.
     */
    struct BoolTypeInfo
        : public TemplateTypeInfo<bool>
    {
        BoolTypeInfo()
            : TemplateTypeInfo<bool>("bool")
        {}

        virtual std::ostream& write( std::ostream& os, DataSourceBase::shared_ptr in ) const {
#ifdef OS_HAVE_STREAMS
            DataSource<bool>* d = AdaptDataSource<bool>()( in );
            if (d)
                return os << boolalpha << d->value();
#endif
            return os;
        }

        virtual std::istream& read( std::istream& os, DataSourceBase::shared_ptr out ) const {
#ifdef OS_HAVE_STREAMS
            AssignableDataSource<bool>::shared_ptr d = AdaptAssignableDataSource<bool>()( out );
            if ( d ) {
                boolalpha(os);
                os >> d->set();
                d->updated(); // because use of set().
            }
#endif
            return os;
        }
    };

    /**
     * Standard strings don't need decomposition.
     */
    struct StdStringTypeInfo
        : public SequenceTypeInfo<std::string,true>
    {
        StdStringTypeInfo()
            : SequenceTypeInfo<std::string,true>("string")
        {}

        base::AttributeBase* buildVariable(std::string name,int size) const
        {
            string t_init(size, ' '); // we can't use the default char(), which is null !

            // returned type is identical to parent, but we set spaces.
            AttributeBase* ret = SequenceTypeInfo<std::string,true>::buildVariable(name,size);
            Attribute<std::string> tt = ret;
            tt.set( t_init );
            return ret;
        }

        virtual bool decomposeType( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            return false;
        }

        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
            // First, try a plain update.
            if ( result->update( source.get() ) )
                return true;
            return false;
        }

    };

    bool RealTimeToolkitPlugin::loadTypes()
    {
        TypeInfoRepository::shared_ptr ti = TypeInfoRepository::Instance();

        // The standard C types + std::string are defined here,

        ti->addType( new StdTypeInfo<int>("int") );
        ti->addType( new StdTypeInfo<unsigned int>("uint") );
        ti->addType( new StdTypeInfo<double>("double") );
        ti->addType( new StdTypeInfo<float>("float") );
        ti->addType( new StdTypeInfo<char>("char") );
        ti->addType( new BoolTypeInfo() );
        ti->addType( new TypeInfoName<void>("void"));
        // string is a special case for assignment, we need to assign from the c_str() instead of from the string(),
        // the latter causes capacity changes, probably due to the copy-on-write implementation of string(). Assignment
        // from a c-style string obviously disables a copy-on-write connection.
        ti->addType( new StdStringTypeInfo() );

        // load the Orocos specific types:
        loadOrocosTypes( ti );
        return true;
    }
}

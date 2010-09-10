/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  BoolTypeInfo.hpp

                        BoolTypeInfo.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#include "../types/Types.hpp"
#include "../types/TemplateTypeInfo.hpp"
#include <ostream>
#include <istream>

namespace RTT
{
    namespace types
    {

        /**
         * Write boolean as 'true' or 'false'.
         */
        struct BoolTypeInfo: public TemplateTypeInfo<bool>
        {
            BoolTypeInfo() :
                TemplateTypeInfo<bool> ("bool")
            {
            }

            virtual std::ostream& write(std::ostream& os, base::DataSourceBase::shared_ptr in) const
            {
#ifdef OS_HAVE_STREAMS
                internal::DataSource<bool>::shared_ptr d = boost::dynamic_pointer_cast< internal::DataSource<bool> >(in);
                if (d)
                    return os << boolalpha << d->value();
#endif
                return os;
            }

            virtual std::istream& read(std::istream& os, base::DataSourceBase::shared_ptr out) const
            {
#ifdef OS_HAVE_STREAMS
                internal::AssignableDataSource<bool>::shared_ptr d = boost::dynamic_pointer_cast< internal::AssignableDataSource<bool> >(out);
                if (d)
                {
                    boolalpha(os);
                    os >> d->set();
                    d->updated(); // because use of set().
                }
#endif
                return os;
            }
        };

    }
}

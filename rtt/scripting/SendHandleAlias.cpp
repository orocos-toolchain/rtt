/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  SendHandleAlias.cpp

                        SendHandleAlias.cpp -  description
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


#include "SendHandleAlias.hpp"
#include <iostream>
using namespace std;

namespace RTT
{
    using namespace detail;

    SendHandleAlias::SendHandleAlias(const std::string& name,
                                     DataSourceBase::shared_ptr d,
                                     OperationInterfacePart* opf) :
        base::AttributeBase(name), data(d), fact(opf)
    {
    }

    base::DataSourceBase::shared_ptr SendHandleAlias::getDataSource() const
    {
        return data;
    }

    OperationInterfacePart* SendHandleAlias::getFactory() const
    {
        return fact;
    }

    SendHandleAlias* SendHandleAlias::clone() const
    {
        return new SendHandleAlias(mname, data.get(), fact);
    }
    SendHandleAlias* SendHandleAlias::copy(std::map<
            const base::DataSourceBase*, base::DataSourceBase*>& replacements,
                                           bool inst)
    {
        // BIG NOTE: Instantiating a SendHandleAlias may/WILL happen too soon, not giving a chance
        // to the arguments to instantiate during copy... Once instantiation is over, we
        // may call copy, but we guess this won't even happen in our current application.
        assert( (inst == false) && "SendHandleAlias may not be instantiated !" );
        return new SendHandleAlias(mname, data->copy(replacements), fact);
    }

}

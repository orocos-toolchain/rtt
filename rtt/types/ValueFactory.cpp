/***************************************************************************
  tag: Peter Soetens Fri Mar 16 21:32:02 2012 +0100 ValueFactory.cpp

                        ValueFactory.cpp -  description
                           -------------------
    begin                : Fri Mar 16 2012
    copyright            : (C) 2012 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "rtt/types/ValueFactory.hpp"
#include "rtt/rtt-config.h"

namespace RTT
{
    using namespace std;
    using namespace detail;
    using namespace internal;

    AttributeBase* ValueFactory::buildVariable(std::string name, int ) const {
        return this->buildVariable(name);
    }

    AttributeBase* ValueFactory::buildConstant(std::string name,DataSourceBase::shared_ptr dsb, int ) const {
        return this->buildConstant(name, dsb );
    }
}

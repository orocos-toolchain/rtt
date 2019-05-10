/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  RealTimeTypekit.cxx

                        RealTimeTypekit.cxx -  description
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


#include "rtt-typekit-config.h"
// This include must be as soon as possible in order to avoid a gcc
// compiler warning about visibility.
#include "Types.inc"

#include "RealTimeTypekit.hpp"
#include "StdTypeInfo.hpp"
#include "BoolTypeInfo.hpp"
#include "../types/TypeInfoName.hpp"


namespace RTT
{
    namespace types {
        void loadOrocosTypes(TypeInfoRepository::shared_ptr ti);
        void loadStdTypes(TypeInfoRepository::shared_ptr ti);
    }

    using namespace std;
    using namespace detail;

    bool RealTimeTypekitPlugin::loadTypes()
    {
        TypeInfoRepository::shared_ptr ti = TypeInfoRepository::Instance();

        // The standard C types + std::string are defined here,
        ti->addType( new StdTypeInfo<int>("int") );
        ti->addType( new StdTypeInfo<unsigned int>("uint") );
        ti->addType( new StdTypeInfo<long long>("llong") );
        ti->addType( new StdTypeInfo<unsigned long long>("ullong") );
        ti->addType( new StdTypeInfo<double>("double") );
        ti->addType( new StdTypeInfo<float>("float") );
        ti->addType( new StdTypeInfo<char>("char") );
        ti->addType( new BoolTypeInfo() );
        ti->addType( new TypeInfoName<void>("void"));

        // load the Orocos specific types:
        loadOrocosTypes( ti );
        loadStdTypes( ti );
        return true;
    }
}


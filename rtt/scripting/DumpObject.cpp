/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:19 CET 2008  DumpObject.cpp

                        DumpObject.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
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


#include <iostream>
#include <vector>
#include <string>
#include "../base/DataSourceBase.hpp"
#include "../interface/ServiceProvider.hpp"
#include "../interface/ServiceProvider.hpp"
#include "DumpObject.hpp"
#include "rtt-fwd.hpp"


using namespace std;
using namespace RTT::detail;

namespace RTT { namespace scripting {

void DumpObject(ServiceProvider* peer)
{
    if (!peer) { cerr << "Not dumping null task!"<<endl; return;}

    cerr << "Dumping Service "<< peer->getName() << endl;
    cerr << "Attributes: ";
    std::vector<std::string> objlist = peer->names();
    for( std::vector<std::string>::iterator it = objlist.begin(); it != objlist.end(); ++it) {
        DataSourceBase::shared_ptr pds = peer->getValue(*it)->getDataSource();
        cerr << *it <<"(="<< pds<<") ";
    }
    cerr << endl;
}}
}

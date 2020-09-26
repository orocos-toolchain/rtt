/***************************************************************************
  tag: Peter Soetens Fri Mar 16 21:32:02 2012 +0100 MemberFactory.cpp

                        MemberFactory.cpp -  description
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


#include "rtt/types/MemberFactory.hpp"
#include "rtt/Logger.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::detail;

bool MemberFactory::resize(base::DataSourceBase::shared_ptr arg, int size) const {
    return false;
}

    vector<string> MemberFactory::getMemberNames() const
    {
        return vector<string>();
    }

bool MemberFactory::getMember(internal::Reference* ref, DataSourceBase::shared_ptr item, const std::string& part_name) const
{
    return false;
}        

    DataSourceBase::shared_ptr MemberFactory::getMember(DataSourceBase::shared_ptr item, const std::string& part_name) const
    {
        /** ** Strong typed data **
         *
         * for( set i = 0; i <=10; set i = i + 1) {
         *      value[i] = i; // sequence index, runtime structure
         *      value.i  = i; // part name, browse static structure
         * }
         * set frame.pos = vector(a,b,c);     // getMember("pos")
         * set frame.pos[3] = vector(a,b,c);  // getMember("pos")->getMember(3)
         * set frame[3].pos = vector(a,b,c);  // getMember(3)->getMember("pos")
         * set frame[i].pos = vector(a,b,c);  // getMember( $i )->getMember("pos")
         * set frame["tool"].pos = vector(a,b,c); // getMember("tool") xx
         * set frame[arg].pos = vector(a,b,c);// getMember( arg )->getMember("pos")
         */
        if ( part_name.empty() )
            return item;
        else
            return DataSourceBase::shared_ptr();
    }

    DataSourceBase::shared_ptr MemberFactory::getMember(DataSourceBase::shared_ptr item, DataSourceBase::shared_ptr id) const
    {
        /** ** Strong typed data **
         *
         * for( set i = 0; i <=10; set i = i + 1) {
         *      value[i] = i; // sequence index, runtime structure
         *      value.i  = i; // part name, browse static structure
         * }
         * set frame.pos = vector(a,b,c);     // getMember("pos")
         * set frame.pos[3] = vector(a,b,c);  // getMember("pos")->getMember(3)
         * set frame[3].pos = vector(a,b,c);  // getMember(3)->getMember("pos")
         * set frame[i].pos = vector(a,b,c);  // getMember( $i )->getMember("pos")
         * set frame["tool"].pos = vector(a,b,c); // getMember("tool") xx
         * set frame[arg].pos = vector(a,b,c);// getMember( arg )->getMember("pos")
         */
        return DataSourceBase::shared_ptr();
    }



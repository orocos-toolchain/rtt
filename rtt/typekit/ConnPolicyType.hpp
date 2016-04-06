/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  ConnPolicyType.hpp

                        ConnPolicyType.hpp -  description
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


#ifndef ORO_CONNPOLICYTYPE_HPP_
#define ORO_CONNPOLICYTYPE_HPP_

#include <boost/serialization/serialization.hpp>
#include "../ConnPolicy.hpp"

namespace boost {
    namespace serialization {
        /**
         * Serializes RTT::ConnPolicy objects.
         * @param a Any boost::serialization compatible archive
         * @param c A ConnPolicy that will be read or written.
         */
        template<class Archive>
        void serialize(Archive& a, RTT::ConnPolicy& c, unsigned int) {
            a & boost::serialization::make_nvp("type", c.type);
            a & boost::serialization::make_nvp("size", c.size );
            a & boost::serialization::make_nvp("lock_policy", c.lock_policy );
            a & boost::serialization::make_nvp("init", c.init );
            a & boost::serialization::make_nvp("pull", c.pull );
            a & boost::serialization::make_nvp("buffer_policy", c.buffer_policy );
            a & boost::serialization::make_nvp("mandatory", c.mandatory );
            a & boost::serialization::make_nvp("transport", c.transport );
            a & boost::serialization::make_nvp("data_size", c.data_size );
            a & boost::serialization::make_nvp("name_id", c.name_id );
        }
    }
}


#endif /* ORO_CONNPOLICYTYPE_HPP_ */

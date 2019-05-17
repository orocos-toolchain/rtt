/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnPolicy.cpp

                        ConnPolicy.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


/*
 * ConnPolicy.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: kaltan
 */

#include "ConnPolicy.hpp"
#include "Property.hpp"
#include "PropertyBag.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;

namespace RTT
{
    struct ConnPolicy::ConnPolicyDefault {};
    ConnPolicy::ConnPolicy(const ConnPolicyDefault &)
        : type(DATA)
        , size(0)
        , lock_policy(LOCK_FREE)
        , init(false)
        , pull(false)
        , buffer_policy(PerConnection)
        , max_threads(0)
        , mandatory(true)
        , transport(0)
        , data_size(0)
    {}

    ConnPolicy &ConnPolicy::Default()
    {
        static ConnPolicy *s_default_policy = new ConnPolicy(ConnPolicyDefault());
        return *s_default_policy;
    }

    ConnPolicy ConnPolicy::buffer(int size, int lock_policy /*= LOCK_FREE*/, bool init_connection /*= false*/, bool pull /*= false*/)
    {
        ConnPolicy result;
        result.type = BUFFER;
        result.size = size;
        result.lock_policy = lock_policy;
        result.init = init_connection;
        result.pull = pull;
        return result;
    }

    ConnPolicy ConnPolicy::circularBuffer(int size, int lock_policy /*= LOCK_FREE*/, bool init_connection /*= false*/, bool pull /*= false*/)
    {
        ConnPolicy result;
        result.type = CIRCULAR_BUFFER;
        result.size = size;
        result.lock_policy = lock_policy;
        result.init = init_connection;
        result.pull = pull;
        return result;
    }

    ConnPolicy ConnPolicy::data(int lock_policy /*= LOCK_FREE*/, bool init_connection /*= true*/, bool pull /*= false*/)
    {
        ConnPolicy result;
        result.type = DATA;
        result.lock_policy = lock_policy;
        result.init = init_connection;
        result.pull = pull;
        return result;
    }

    ConnPolicy::ConnPolicy()
        : type(Default().type)
        , size(Default().size)
        , lock_policy(Default().lock_policy)
        , init(Default().init)
        , pull(Default().pull)
        , buffer_policy(Default().buffer_policy)
        , max_threads(Default().max_threads)
        , mandatory(Default().mandatory)
        , transport(Default().transport)
        , data_size(Default().data_size)
    {}

    ConnPolicy::ConnPolicy(int type)
        : type(type)
        , size(Default().size)
        , lock_policy(Default().lock_policy)
        , init(Default().init)
        , pull(Default().pull)
        , buffer_policy(Default().buffer_policy)
        , max_threads(Default().max_threads)
        , mandatory(Default().mandatory)
        , transport(Default().transport)
        , data_size(Default().data_size)
    {}

    ConnPolicy::ConnPolicy(int type, int lock_policy)
        : type(type)
        , size(Default().size)
        , lock_policy(lock_policy)
        , init(Default().init)
        , pull(Default().pull)
        , buffer_policy(Default().buffer_policy)
        , max_threads(Default().max_threads)
        , mandatory(Default().mandatory)
        , transport(Default().transport)
        , data_size(Default().data_size)
    {}

    std::ostream &operator<<(std::ostream &os, const ConnPolicy &cp)
    {
        std::string type;
        switch(cp.type) {
            case ConnPolicy::UNBUFFERED:      type = "UNBUFFERED"; break;
            case ConnPolicy::DATA:            type = "DATA"; break;
            case ConnPolicy::BUFFER:          type = "BUFFER"; break;
            case ConnPolicy::CIRCULAR_BUFFER: type = "CIRCULAR_BUFFER"; break;
            default:                          type = "(unknown type)"; break;
        }
        if (cp.size > 0) {
            type += "[" + boost::lexical_cast<std::string>(cp.size) + "]";
        }

        std::string lock_policy;
        switch(cp.lock_policy) {
            case ConnPolicy::UNSYNC:    lock_policy = "UNSYNC"; break;
            case ConnPolicy::LOCKED:    lock_policy = "LOCKED"; break;
            case ConnPolicy::LOCK_FREE: lock_policy = "LOCK_FREE"; break;
            default:                    lock_policy = "(unknown lock policy)"; break;
        }

        std::string pull;
        // note: cast to int to suppress clang "warning: switch condition has boolean value"
        switch(int(cp.pull)) {
            case int(ConnPolicy::PUSH): pull = "PUSH"; break;
            case int(ConnPolicy::PULL): pull = "PULL"; break;
        }

        os << pull << " ";
        os << BufferPolicy(cp.buffer_policy) << " ";
        os << lock_policy << " ";
        os << type;
        if (!cp.name_id.empty()) os << " (name_id=" << cp.name_id << ")";
        if (cp.max_threads > 0) os << " (max_threads=" << cp.max_threads << ")";

        return os;
    }
}

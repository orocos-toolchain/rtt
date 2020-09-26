/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  Handle.cxx

                        Handle.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#include "rtt/Handle.hpp"
#include "rtt/rtt-fwd.hpp"


namespace RTT {
    using namespace detail;

    Handle::Handle()
        : m_conn(0)
    {
    }

    Handle::Handle(connection_t conn)
        : m_conn(conn)
    {
    }

    Handle::~Handle()
    {
    }

    bool Handle::connect()
    {
        if ( connected() )
            return true;
        return m_conn && m_conn->connect();
    }

    bool Handle::disconnect()
    {
        if ( !connected() )
            return true;
        return m_conn->disconnect();
    }

    bool Handle::connected() const {
        return m_conn && m_conn->connected();
    }

    Handle::operator bool() const {
        return m_conn != 0;
    }

    bool Handle::ready() const {
        return m_conn != 0;
    }


    ScopedHandle::ScopedHandle(const Handle& hs)
        : Handle( hs )
    {
    }


    ScopedHandle::~ScopedHandle()
    {
        this->disconnect();
    }
    CleanupHandle::CleanupHandle(const Handle& hs)
        : Handle( hs )
    {
    }

    CleanupHandle::~CleanupHandle()
    {
        if (m_conn)
            m_conn->destroy();
    }
}

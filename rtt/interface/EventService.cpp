/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  EventService.cxx

                        EventService.cxx -  description
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


#include "EventService.hpp"
#include "../internal/mystd.hpp"



namespace RTT {
    using namespace detail;
    using namespace boost;

    void EventService::clear() {
        for (Hooks::iterator it = mhooks.begin(); it !=mhooks.end(); ++it )
            delete it->second;
    }


    EventService::~EventService() {
        clear();
    }

    ConnectionC EventService::setupConnection(const std::string& ename) const {
        if ( mhooks.count(ename) != 1 ) {
            log(Error) << "Can not initialize ConnectionC for '"<<ename<<"': no such Event."<<endlog();
            return ConnectionC(); // empty handle.
        }
        return ConnectionC(this, ename );
    }

    Handle EventService::setupSyn(const std::string& ename,
                                  boost::function<void(void)> func,
                                  std::vector<DataSourceBase::shared_ptr> args ) const {
        if ( mhooks.count(ename) != 1 ) {
            log(Error) << "Can not create connection to '"<<ename<<"': no such Event."<<endlog();
            return Handle(); // empty handle.
        }
        assert(false);
        detail::EventHookBase* ehi = 0; // mhooks.find(ename)->second->produce( args );

        // ehi is stored _inside_ the connection object !
        return ehi->setupSyn( func );
    }

}

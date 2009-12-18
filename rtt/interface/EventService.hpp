/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  EventService.hpp

                        EventService.hpp -  description
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


#ifndef EVENT_SERVICE_HPP
#define EVENT_SERVICE_HPP

#include "../Logger.hpp"
#include "../internal/EventHook.hpp"
#include "../internal/Signal.hpp"
#include "../internal/ConnectionC.hpp"
#include <boost/static_assert.hpp>
#include <boost/type_traits/function_traits.hpp>

namespace RTT
{ namespace interface {

    /**
     * The EventService represents the event interface. It stores pointers
     * to event objects and allows clients to retrieve event objects which
     * can then be emitted or subscribed to.
     *
     */
    class RTT_API EventService
    {
        // creates event hooks
        typedef std::map<std::string, internal::FunctorFactoryPart<internal::EventHookBase*>* > Hooks;
        Hooks mhooks;
    public:
        ~EventService();

        /**
         * Returns the names of all events added to this interface.
         * @see getNames() to get a list of all events available to scripting.
         */
        std::vector<std::string> getEvents() const;

        void clear();

        /**
         * Setup a internal::ConnectionC object to connect a number of synchronous or asynchronous callbacks to an event.
         * Use this method as in
         @verbatim
         Handle h = setupConnection("EventName").callback( &my_function ).handle();
         h.connect();
         // or for adding a method:
         h = setupConnection("EventName").callback( boost::bind( &types::MyClass::function, obj) ).handle();
         h.connect();
         @endverbatim
         * @see internal::ConnectionC
         * @see Handle
         * @throw name_not_found_exception
         */
        internal::ConnectionC setupConnection(const std::string& ename) const;

        /**
         * For internal use only. Setup a synchronous Event handler which will set \a args and
         * then call \a func synchronously when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param func  A function object which will be called.
         * @param args  The arguments which will be set before \a func is called.
         * They must be of type \a internal::AssignableDataSource<Tn> or \a internal::DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         */
        Handle setupSyn(const std::string& ename,
                        boost::function<void(void)> func,
                        std::vector<base::DataSourceBase::shared_ptr> args ) const;

    };


}}

#endif

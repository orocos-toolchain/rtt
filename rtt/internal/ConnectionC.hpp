/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ConnectionC.hpp

                        ConnectionC.hpp -  description
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


#ifndef ORO_EXECUTION_CONNECTIONC_HPP
#define ORO_EXECUTION_CONNECTIONC_HPP

#include <string>
#include "DataSources.hpp"
#include "../Handle.hpp"
#include "EventCallBack.hpp"
#include "EventProcessor.hpp"
#include <boost/function_types/function_arity.hpp>

namespace RTT { namespace internal {

    /**
     * A user friendly callback connection to an event of a TaskContext.
     * At most one synchronous and one asynchronous \a callback() can be added to a ConnectionC object.
     * The returned \a handle() connects or disconnects the added callbacks.
     *
     * @important Use the interface::EventService to create ConnectionC objects.
     * @see interface::EventService
     */
    class RTT_API ConnectionC
    {
        /**
         * The 'd' pointer pattern.
         */
        class D;
        D* d;

        ConnectionC& mcallback(EventCallBack* ecb);

        ConnectionC& mcallback(EventCallBack* ecb, EventProcessor* ep, EventProcessor::AsynStorageType s_type);
    public:
        /**
         * The default constructor.
         * Assign it from a copy from another ConnectionC object
         * in order to make it usable.
         */
        ConnectionC();

        /**
         * The constructor.
         * @see interface::EventService
         */
        ConnectionC( const interface::EventService* gcf, const std::string& name );

        /**
         * A ConnectionC is copyable by value.
         */
        ConnectionC(const ConnectionC& other);

        /**
         * A ConnectionC is assignable;
         */
        ConnectionC& operator=(const ConnectionC& other);

        ~ConnectionC();

        /**
         * Check if this connection is initialised and ready.
         */
        bool ready() const;

        /**
         * Add a Synchronous callback to an object's member function.
         * @param t A pointer to the object upon which the function must be invoked.
         * @param foo The object's member function which is called back, for example '&X::my_function'
         */
        template<class Type, class Function>
        ConnectionC& callback(Type t, Function foo )
        {
            return this->mcallback( new detail::CallBackWrapper<Function, boost::function_types::function_arity<Function>::value>(t, foo ) );
        }

        /**
         * Add an Asynchronous callback to an object's member function.
         * @param t A pointer to the object upon which the function must be invoked.
         * @param foo The object's member function which is called back, for example '&X::my_function'
         * @param ep The EventProcessor which will execute the callback 'foo'.
         * @param s_type The method used when event overruns happen. By default, only the first event
         * is propagated to the callbacks.
         */
        template<class Type, class Function>
        ConnectionC& callback(Type t,
                              Function foo,
                              EventProcessor* ep,
                              EventProcessor::AsynStorageType s_type = EventProcessor::OnlyFirst )
        {
            return this->mcallback( new detail::CallBackWrapper<Function, boost::function_types::function_arity<Function>::value>(t, foo ), ep, s_type );
        }
        /**
         * Add a Synchronous callback to a 'C' function.
         * @param foo A 'C' function to call back when the event is emitted.
         */
        template<class Function>
        ConnectionC& callback( Function foo )
        {
            return this->mcallback( new detail::CallBackWrapperFunction<Function, boost::function_types::function_arity<Function>::value>( foo ) );
        }

        /**
         * Add an Asynchronous callback to a 'C' function.
         * @param foo A 'C' function to call back when the event is emitted.
         * @param ep The EventProcessor which will execute the callback 'foo'.
         * @param s_type The method used when event overruns happen. By default, only the first event
         * is propagated to the callbacks.
         */
        template<class Function>
        ConnectionC& callback( Function foo, EventProcessor* ep,
                               EventProcessor::AsynStorageType s_type = EventProcessor::OnlyFirst )
        {
            return this->mcallback( new detail::CallBackWrapperFunction<Function, boost::function_types::function_arity<Function>::value>( foo, ep ), s_type );
        }

        /**
         * Get a Handle to the created connections. After
         * handle() is called, new callbacks may be added, upon which
         * a new handle will be returned.
         * @return An empty handle if no callback was added.
         */
        Handle handle();
    };
}}

#endif

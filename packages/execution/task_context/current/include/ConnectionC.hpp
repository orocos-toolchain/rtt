/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ConnectionC.hpp 

                        ConnectionC.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_EXECUTION_CONNECTIONC_HPP
#define ORO_EXECUTION_CONNECTIONC_HPP

#include <string>
#include "DataSource.hpp"
#include <corelib/Handle.hpp>
#include "EventCallBack.hpp"
#include "corelib/EventProcessor.hpp"
#include "boost/function_types/function_type_arity.hpp"

namespace ORO_CoreLib {
    class EventProcessor;
}

namespace ORO_Execution
{
    class EventService;

    /**
     * A user friendly callback connection to an event of a TaskContext.
     * At most one synchronous and one asynchronous \a callback() can be added to a ConnectionC object.
     * The returned \a handle() connects or disconnects the added callbacks.
     *
     * @important Use the EventService to create ConnectionC objects.
     * @see EventService
     */
    class ConnectionC
    {
        /**
         * The 'd' pointer pattern.
         */
        class D;
        D* d;

        ConnectionC& mcallback(detail::EventCallBack* ecb);

        ConnectionC& mcallback(detail::EventCallBack* ecb, ORO_CoreLib::EventProcessor* ep, ORO_CoreLib::EventProcessor::AsynStorageType s_type);
    public:
        /**
         * The default constructor.
         * Assign it from a copy from another ConnectionC object
         * in order to make it usable.
         */
        ConnectionC();

        /**
         * The constructor.
         * @see EventService
         */
        ConnectionC( const EventService* gcf, const std::string& name );

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
         * Add a Synchronous callback to an object's member function.
         * @param t A pointer to the object upon which the function must be invoked.
         * @param foo The object's member function which is called back, for example '&X::my_function'
         */
        template<class Type, class Function>
        ConnectionC& callback(Type t, Function foo )
        {
            return this->mcallback( new detail::CallBackWrapper<Function, boost::function_types::function_type_signature<Function>::arity::value>(t, foo ) );
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
                              ORO_CoreLib::EventProcessor* ep,
                              ORO_CoreLib::EventProcessor::AsynStorageType s_type = ORO_CoreLib::EventProcessor::OnlyFirst )
        {
            return this->mcallback( new detail::CallBackWrapper<Function, boost::function_types::function_type_signature<Function>::arity::value>(t, foo ), ep, s_type );
        }
        /**
         * Add a Synchronous callback to a 'C' function.
         * @param foo A 'C' function to call back when the event is emitted.
         */
        template<class Function>
        ConnectionC& callback( Function foo )
        {
            return this->mcallback( new detail::CallBackWrapperFunction<Function, boost::function_types::function_type_signature<Function>::arity::value>( foo ) );
        }

        /**
         * Add an Asynchronous callback to a 'C' function.
         * @param foo A 'C' function to call back when the event is emitted.
         * @param ep The EventProcessor which will execute the callback 'foo'.
         * @param s_type The method used when event overruns happen. By default, only the first event
         * is propagated to the callbacks.
         */
        template<class Function>
        ConnectionC& callback( Function foo, ORO_CoreLib::EventProcessor* ep,
                               ORO_CoreLib::EventProcessor::AsynStorageType s_type = ORO_CoreLib::EventProcessor::OnlyFirst )
        {
            return this->mcallback( new detail::CallBackWrapperFunction<Function, boost::function_types::function_type_signature<Function>::arity::value>( foo, ep ), s_type );
        }

        /**
         * Get a Handle to the created connections. After
         * handle() is called, new callbacks may be added, upon which
         * a new handle will be returned.
         * @return An empty handle if no callback was added.
         */
        ORO_CoreLib::Handle handle();
    };
}

#endif

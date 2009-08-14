/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:38 CET 2006  Handle.hpp

                        Handle.hpp -  description
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


#ifndef ORO_CORELIB_SIGNAL_HANDLE_HPP
#define ORO_CORELIB_SIGNAL_HANDLE_HPP

#include "internal/signal_base.hpp"

namespace RTT
{
    /**
     * @brief The Handle holds the information, and allows manipulation, of a connection
     * between an Event Handler function and the Event itself.
     *
     * It is returned by the connect() and setup() methods of Event and can
     * be used to (dis)connect a handler function from the event.
     * Handle objects may be assigned to each other and will always point
     * to the same connection. If the last Handle object is destroyed,
     * and is not connected, the connection is destroyed. Thus the
     * resource management (deallocation) is handled by Orocos itself.
     *
     * To inspect if the Handle points to a valid, existing connection, one can use :
     * @verbatim
     Handle handle;
     // ...
     if ( !handle ) {
        // not valid !
     }
     @endverbatim
     * @ingroup CoreLibEvents
     */
	class RTT_API Handle
	{
	private:
		typedef detail::signal_base           sender_t; //! the signal type
        typedef sender_t::connection_t    connection_t; //! the connection type for the slot of this Handle
	public:
        /**
         * Create an empty Handle.
         */
		Handle();

        Handle(connection_t conn);

        /**
         * Create a copy-equivalent Handle.
         */
        Handle(const Handle& hs);

        /**
         * No-op destructor, does not change signal/slot state.
         */
		~Handle();

        /**
         * (Re-)Connect the slot with the signal.
         * @retval true the slot is connected.
         * @retval false no valid signal or slot in this Handle
         */
		bool connect();

        /**
         * Disconnect the slot from the signal.
         * @retval true the slot is disconnected.
         * @retval false no valid signal or slot in this Handle
         */
		bool disconnect();

        /**
         * Inspect if this Handle represents a connected signal and slot.
         *
         * @return true if a connection is present.
         */
        bool connected() const;

        /**
         * Inspect if this Handle is pointing to a valid (existing) connection.
         * @return false if no connection is associated with this Handle.
         */
        operator bool() const;

        /**
         * Inspect if this handle is pointing to valid (existing) connection(s).
         * @return false if no connection(s) is associated with this handle.
         */
        bool ready() const;
	protected:
        /**
         * This is actually a smart pointer which always
         * points to an existing connection object.
         */
        connection_t  m_conn;
	};

    /**
     * A scoped connection Handle of a (connected) slot which
     * disconnects a slot from a signal in its destructor.
     */
	class RTT_API ScopedHandle
        :public Handle
	{
		ScopedHandle();
    public:

        ScopedHandle(const Handle& hs);

        /**
         * If connected, disconnect the slot from the signal.
         */
		~ScopedHandle();
    };

    /**
     * A connection Handle of a (connected) slot which
     * disconnects and cleans up (free all resources)
     * a slot from a signal in its destructor.
     */
	class RTT_API CleanupHandle
        :public Handle
	{
		CleanupHandle();
    public:

        CleanupHandle(const Handle& hs);

        /**
         * Cleanup all signal and slot connection resources.
         * If connected, disconnect the slot from the signal.
         */
		~CleanupHandle();
    };
}

#endif

/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:38 CET 2006  Handle.hpp 

                        Handle.hpp -  description
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
 
 
#ifndef ORO_CORELIB_SIGNAL_HANDLE_HPP
#define ORO_CORELIB_SIGNAL_HANDLE_HPP

#include "impl/signal_base.hpp"

namespace sigslot
{

    /**
     * A copy-able connection handle of a (connected) slot and a signal.
     */
	class handle
	{
	private:
		typedef detail::signal_base           sender_t; //! the signal type
        typedef sender_t::connection_t    connection_t; //! the connection type for the slot of this handle
	public:
        /**
         * Create an empty handle.
         */
		handle();

        handle(connection_t conn);

        /**
         * Create a copy-equivalent handle.
         */
        handle(const handle& hs);

        /**
         * No-op destructor, does not change signal/slot state.
         */
		~handle();

        /**
         * (Re-)Connect the slot with the signal.
         * @retval true the slot is connected.
         * @retval false no valid signal or slot in this handle
         */
		bool connect();

        /**
         * Disconnect the slot from the signal.
         * @retval true the slot is disconnected.
         * @retval false no valid signal or slot in this handle
         */
		bool disconnect();

        /** 
         * Inspect if this handle represents a connected signal and slot.
         * 
         * @return true if a connection is present.
         */
        bool connected() const;

        /**
         * Inspect if this handle is pointing to a valid (existing) connection.
         * @return false if no connection is associated with this handle.
         */
        operator bool() const;
	protected:
        /**
         * This is actually a smart pointer which always
         * points to an existing connection object.
         */
        connection_t  m_conn;
	};

    /**
     * A scoped connection handle of a (connected) slot which
     * disconnects a slot from a signal in its destructor.
     */
	class scoped_handle
        :public handle
	{
		scoped_handle();
    public:

        scoped_handle(const handle& hs);

        /**
         * If connected, disconnect the slot from the signal.
         */
		~scoped_handle();
    };

    /**
     * A connection handle of a (connected) slot which
     * disconnects and cleans up (free all resources)
     * a slot from a signal in its destructor.
     */
	class cleanup_handle
        :public handle
	{
		cleanup_handle();
    public:

        cleanup_handle(const handle& hs);

        /**
         * Cleanup all signal and slot connection resources.
         * If connected, disconnect the slot from the signal.
         */
		~cleanup_handle();
    };


}

namespace ORO_CoreLib
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
     * @endverbatim
     */
    class Handle
    {
        sigslot::handle _c;
        sigslot::handle _c2;
    public:
        Handle( const sigslot::handle & c,
                const sigslot::handle & c2 );

        Handle( const sigslot::handle & c );
        Handle( const Handle& h );
        Handle();
        ~Handle();

#if 0
        Handle& operator=(const Handle& h);

        bool operator==(const Handle& h) const;

        bool operator<(const Handle& h) const;

        operator sigslot::handle() const {
            return _c;
        }
#endif

        /**
         * Inspect if the connection(s) of this handle is connected
         * to an Event.
         * @return false if no valid connection exists or if the
         * connection is not connected to the event.
         */
        bool connected() const;

        /**
         * Disconnect syn and asyn handlers.
         * @return false if no valid connection exists.
         */
        bool disconnect();

        /**
         * (Re-)Connect syn and asyn handlers.
         * @return false if no valid connection exists.
         */
        bool connect();

        /**
         * Inspect if this handle is pointing to valid (existing) connection(s).
         * @return false if no connection(s) is associated with this handle.
         */
        operator bool() const;

    };
}
#endif

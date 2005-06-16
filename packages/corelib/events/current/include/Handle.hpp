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
     * @brief The Handle holds the information of a connection
     * between an Event Handler function and the Event itself.
     *
     * It is returned by the connect() and setup() methods of Event and can
     * be used to disconnect a handler function from the event.
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
#endif

        bool connected() const;

        /**
         * Disconnect syn and asyn handlers.
         */
        bool disconnect();

        /**
         * (Re-)Connect syn and asyn handlers.
         */
        bool connect();

    };
}
#endif

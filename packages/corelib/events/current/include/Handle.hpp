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
		handle()
            : m_conn(0)
		{
			;
		}

        handle(connection_t conn)
            : m_conn(conn)
        {
        }

        /**
         * Create a copy-equivalent handle.
         */
        handle(const handle& hs)
            : m_conn( hs.m_conn )
		{
		} 

        /**
         * No-op destructor, does not change signal/slot state.
         */
		~handle()
		{
		}

        /**
         * (Re-)Connect the slot with the signal.
         * @retval true the slot is connected.
         * @retval false no valid signal or slot in this handle
         */
		bool connect()
		{
            if ( connected() )
                return true;
            return m_conn->connect();
		}

        /**
         * Disconnect the slot from the signal.
         * @retval true the slot is disconnected.
         * @retval false no valid signal or slot in this handle
         */
		bool disconnect()
		{
            if ( !connected() )
                return true;
            return m_conn->disconnect();
		}

        /** 
         * Inspect if this handle represents a connected signal and slot.
         * 
         * @return true if a connection is present.
         */
        bool connected() const {
            return m_conn->connected();
        }
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

        scoped_handle(const handle& hs)
            : handle( hs )
		{
		} 


        /**
         * If connected, disconnect the slot from the signal.
         */
		~scoped_handle()
		{
            this->disconnect();
		}
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

        cleanup_handle(const handle& hs)
            : handle( hs )
		{
		} 

        /**
         * Cleanup all signal and slot connection resources.
         * If connected, disconnect the slot from the signal.
         */
		~cleanup_handle()
		{
            m_conn->destroy();
		}
    };


}

#endif

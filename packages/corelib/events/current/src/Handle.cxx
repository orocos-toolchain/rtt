#include "corelib/Handle.hpp"


namespace sigslot
{

    handle::handle()
        : m_conn(0)
    {
    }

    handle::handle(connection_t conn)
        : m_conn(conn)
    {
    }

    handle::handle(const handle& hs)
        : m_conn( hs.m_conn )
    {
    } 

    handle::~handle()
    {
    }

    bool handle::connect()
    {
        if ( connected() )
            return true;
        return m_conn->connect();
    }

    bool handle::disconnect()
    {
        if ( !connected() )
            return true;
        return m_conn->disconnect();
    }

    bool handle::connected() const {
        return m_conn && m_conn->connected();
    }

    scoped_handle::scoped_handle(const handle& hs)
        : handle( hs )
    {
    } 


    scoped_handle::~scoped_handle()
    {
        this->disconnect();
    }
    cleanup_handle::cleanup_handle(const handle& hs)
        : handle( hs )
    {
    } 

    cleanup_handle::~cleanup_handle()
    {
        if (m_conn)
            m_conn->destroy();
    }
}

namespace ORO_CoreLib
{
    Handle::Handle( const sigslot::handle & c,
                    const sigslot::handle & c2 )
        : _c(c), _c2(c2) {}
    Handle::Handle( const sigslot::handle & c ) : _c(c), _c2(c) {}
    Handle::Handle( const Handle& h ) : _c(h._c), _c2(h._c2) {}
    Handle::Handle() {}
    Handle::~Handle() {}

#if 0
    Handle& Handle::operator=(const Handle& h) {
        _c = h._c;
        _c2 = h._c2;
        return *this;
    }

    bool Handle::operator==(const Handle& h) const {
        return (_c == h._c) && (_c2 == h._c2);
    }

    bool Handle::operator<(const Handle& h) const {
        return (_c < h._c) && (_c2 < h._c2);
    }
#endif

    bool Handle::connected() const {
        return _c.connected() && _c2.connected();
    }

    bool Handle::disconnect() {
        return _c.disconnect() &&
            _c2.disconnect();
    }

    bool Handle::connect() {
        return _c.connect() &&
            _c2.connect();
    }

}

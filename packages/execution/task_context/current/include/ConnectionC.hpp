#ifndef ORO_EXECUTION_CONNECTIONC_HPP
#define ORO_EXECUTION_CONNECTIONC_HPP

#include <string>
#include "DataSource.hpp"
#include <corelib/Handle.hpp>

namespace ORO_CoreLib {
    class EventProcessor;
}

namespace ORO_Execution
{
    class EventService;

    /**
     * A user friendly callback connection to an event of a TaskContext.
     */
    class ConnectionC
    {
        /**
         * The 'd' pointer pattern.
         */
        class D;
        D* d;
        ORO_CoreLib::Handle h;
    public:
        /**
         * The default constructor.
         * Make a copy from another ConnectionC object
         * in order to make it usable.
         */
        ConnectionC();
        /**
         * The constructor.
         * @see EventService
         */
        ConnectionC( const EventService* gcf, const std::string& name,
                     boost::function<void(void)> func, ORO_CoreLib::EventProcessor* ep );

        /**
         * A ConnectionC is copyable by value.
         */
        ConnectionC(const ConnectionC& other);

        ~ConnectionC();

        /**
         * Add a datasource argument to the Connection.
         */
        ConnectionC& arg( ORO_CoreLib::DataSourceBase::shared_ptr a );

        /**
         * Add a reference argument to the Connection.
         */
        template< class ArgT >
        ConnectionC& arg( ArgT& a )
        {
            return this->arg(DataSourceBase::shared_ptr( new ReferenceDataSource<ArgT>( a ) ) );
        }

        /**
         * Get a Handle to the connection.
         */
        ORO_CoreLib::Handle handle() const;
    };
}

#endif

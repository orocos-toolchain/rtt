#ifndef ORO_EXECUTION_CONNECTION_TYPE_HPP
#define ORO_EXECUTION_CONNECTION_TYPE_HPP

namespace ORO_Execution
{

    /**
     * Enumerates all the types of connections supported by
     * this factory.
     */
    struct ConnectionTypes
    {
        enum ConnectionType {
            locked, /** Handle concurrency on basis of a Mutex. */
            lockfree /** Handle concurrency on basis of a LockFree algorithm. */
        };
    };

}

#endif


#ifndef ORO_CORELIB_LOCAL_EVENT_HPP
#define ORO_CORELIB_LOCAL_EVENT_HPP

#include "os/fosi.h"
#include "Signal.hpp"

namespace RTT
{
    namespace detail {

        /**
         * An Event which calls back local functions.
         *
         * @param _Signature The function type signature of the callback functions
         * this event will call. For example void(int, double) if you want the
         * event to call a function 'void foo(int x, double y)';
         */
        template<
            typename Signature // function type R (T1, T2, ..., TN)
        >
        class LocalEvent
            : public signal<Signature>
        {
        public:
            LocalEvent<Signature>* clone() const { return new LocalEvent(); }
        };
    }
}

#endif

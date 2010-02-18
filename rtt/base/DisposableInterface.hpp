#ifndef ORO_DISPOSABLEINTERFACE_HPP
#define ORO_DISPOSABLEINTERFACE_HPP

#include <boost/shared_ptr.hpp>

namespace RTT
{
    namespace base
    {
        /**
         * @brief An object that is executable and
         * is freed after execution.
         */
        class DisposableInterface
        {
        public:
            /**
             * Use this type for shared pointer storage of an
             * DisposableInterface object.
             */
            typedef boost::shared_ptr<DisposableInterface> shared_ptr;

            virtual ~DisposableInterface() {}

            /**
             * Execute functionality and free this object.
             * You may no longer
             * use this object after calling this method.
             */
            virtual void executeAndDispose() = 0;

            /**
             * Just free this object without executing it.
             */
            virtual void dispose() = 0;
        };
    }
}

#endif // EXECUTABLEINTERFACE_HPP

#ifndef EXECUTABLEINTERFACE_HPP
#define EXECUTABLEINTERFACE_HPP

namespace RTT
{
    namespace base
    {
        /**
         * @brief An object that is executable and
         * is freed after execution.
         * @todo: rename to DisposableInterface ?
         */
        class ExecutableInterface
        {
        public:
            /**
             * Use this type for shared pointer storage of an
             * ExecutableInterface object.
             */
            typedef boost::shared_ptr<ExecutableInterface> shared_ptr;

            virtual ~ExecutableInterface() {}

            /**
             * Execute functionality and free this object.
             * You may no longer
             * use this object after calling this method.
             * @todo rename to executeAndDispose() ?
             */
            virtual void execute() = 0;

            /**
             * Just free this object without executing it.
             */
            virtual void dispose() = 0;
        };
    }
}

#endif // EXECUTABLEINTERFACE_HPP

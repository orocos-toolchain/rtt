#ifndef EXECUTABLEINTERFACE_HPP
#define EXECUTABLEINTERFACE_HPP

namespace RTT
{
    namespace detail
    {
        /**
         * @brief An object that is executable and
         * is freed after execution.
         */
        class ExecutableInterface
        {
        public:
            virtual ~ExecutableInterface();

            /**
             * Execute functionality and free this object.
             * You may no longer
             * use this object after calling this method.
             */
            virtual void execute() = 0;
        };
    }
}

#endif // EXECUTABLEINTERFACE_HPP

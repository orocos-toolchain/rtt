#ifndef CALLBACKINTERFACE_HPP
#define CALLBACKINTERFACE_HPP

namespace ORO_CoreLib
{
    /**
     * A CallbackInterface allows you to signal an instance
     * that something has to be completed. This can be an Event,
     * or any other mechanism.
     */
    struct CallbackInterface
    {
        /**
         * Trigger a completion.
         * @note This method is not const, so can not be called on 
         *       a const pointer to CallbackInterface.
         */
        virtual void complete() = 0;
    };


}


#endif

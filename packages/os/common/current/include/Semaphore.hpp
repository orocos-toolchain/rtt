 

#ifndef ORO_OS_SEMAPHORE_HPP
#define ORO_OS_SEMAPHORE_HPP

#include <os/fosi.h>
#include <pkgconf/os.h>

namespace ORO_OS
{
    /**
     * @brief An object oriented wrapper around a counting semaphore.
     * It works like a traffic light on which a thread can \a wait() until
     * the sempahore's value becomes positive, otherwise it blocks. Another
     * thread then needs to \a signal() the semaphore. One thread which is
     * waiting will then be awakened, if none is waiting, the first thread 
     * calling \a wait() will continue directly (and decrease the value by 1).
     * @warning Semaphore instances should only be created in soft realtime,
     *          since the initialisation of a semaphore can not be done in hard realtime.
     *
     */
    class Semaphore 
    {
    private:
        rt_sem_t sem;
    public:
        /**
         * Initialize a Semaphore with an initial \a count.
         */
        Semaphore(int count)
        {
            rtos_sem_init( &sem, count );
        }

        /**
         * Destroy a Semaphore.
         */
        ~Semaphore()
        {
            rtos_sem_destroy( &sem );
        }

        void wait()
        {
            rtos_sem_wait( &sem );
        }

        void signal()
        {
            rtos_sem_signal( &sem );
        }

        /**
         * Try to wait on this semaphore
         *
         * @return true when the locking succeeded ( this->value() was > 0), false otherwise ( this->value() was = 0 )
         */
        bool trywait()
        {
            if ( rtos_sem_trywait( &sem ) == 0 )
                return true;
            return false;
        }

        int value()
        {
            return rtos_sem_value( &sem );
        }
    };
}

#endif

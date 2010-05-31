#ifndef OS_CONDITION_HPP
#define OS_CONDITION_HPP

#include "fosi.h"
#include "../rtt-config.h"
#include "Time.hpp"
#include "Mutex.hpp"
#ifdef ORO_OS_USE_BOOST_THREAD
// BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG is defined in rtt-config.h
#include <boost/thread/condition.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#endif

namespace RTT
{ namespace os {

    /**
     * @brief An object oriented wrapper around a condition variable.
     *
     * @see Mutex, MutexRecursive
     */
	class RTT_API Condition
    {
#ifndef ORO_OS_USE_BOOST_THREAD
	protected:
	    rt_cond_t c;
	public:
	    /**
	    * Initialize a Condition.
	    */
	    Condition()
	    {
	        rtos_cond_init( &c);
	    }

	    /**
	    * Destroy a Condition.
	    * If the Condition is still locked, the RTOS
	    * will not be asked to clean up its resources.
	    */
	    ~Condition()
	    {
	        rtos_cond_destroy( &c );
	    }

	    /**
	     * Wait forever until a condition occurs
	     * @param m The mutex you hold locked when calling this function.
	     * @return false if the waiting failed. This can only
	     * be caused by the OS aborting the waiting.
	     */
	    bool wait (Mutex& m)
	    {
	        return rtos_cond_wait( &c, &m.m ) == 0 ? true : false;
	    }

        /**
         * Wait forever until a condition occurs
         * @param m The mutex you hold locked when calling this function.
         * @param p A function object returning a boolean
         * @return \a false if the waiting failed. This can only
         * be caused by the OS aborting the waiting. Returns
         * \a true when p() has been true.
         */
	    template<class Predicate>
        bool wait (Mutex& m, Predicate& p)
        {
	        while( !p() )
	            if ( rtos_cond_wait( &c, &m.m ) != 0) return false;
	        return true;
        }
	    /**
	     * Wake all threads that are blocking in wait() or wait_until().
	     */
	    void broadcast()
	    {
	        rtos_cond_broadcast( &c );
	    }

        /**
	     * Wait for this condition, but don't wait longer for it
         * than the specified absolute time.
         *
         * @param m The mutex you hold locked when calling this function.
         * @param  abs_time The absolute time to wait until before the condition happens.
         * Use rtos_get_time_ns() to get the current time and Seconds_to_nsecs to add a
         * certain amount to the result.
         * @return true when the condition occured, false in case the timeout
         * happened.
         */
        bool wait_until(Mutex& m, nsecs abs_time)
        {
            if ( rtos_cond_timedwait( &c, &m.m, abs_time ) == 0 )
                return true;
            return false;
        }
#else
    protected:
        boost::condition_variable_any c;
    public:
        /**
        * Initialize a Condition.
        */
        Condition()
        {
        }

        /**
        * Destroy a Condition.
        * If the Condition is still locked, the RTOS
        * will not be asked to clean up its resources.
        */
        ~Condition()
        {
        }

        /**
         * Wait forever until a condition occurs
         * @param m The mutex you hold locked when calling this function.
         * @return false if the waiting failed. This can only
         * be caused by the OS aborting the waiting.
         */
        bool wait (Mutex& m)
        {
            c.wait(m.m);
            return true;
        }

        /**
         * Wait forever until a condition occurs
         * @param m The mutex you hold locked when calling this function.
         * @param p A function object returning a boolean
         * @return \a false if the waiting failed. This can only
         * be caused by the OS aborting the waiting. Returns
         * \a true when p() has been true.
         */
        template<class Predicate>
        bool wait (Mutex& m, Predicate& p)
        {
            c.wait(m.m, p);
            return true;
        }
        /**
         * Wake all threads that are blocking in wait() or wait_until().
         */
        void broadcast()
        {
            c.notify_all();
        }

        /**
         * Wait for this condition, but don't wait longer for it
         * than the specified absolute time.
         *
         * @param m The mutex you hold locked when calling this function.
         * @param  abs_time The absolute time to wait until before the condition happens.
         * Use rtos_get_time_ns() to get the current time and Seconds_to_nsecs to add a
         * certain amount to the result.
         * @return true when the condition occured, false in case the timeout
         * happened.
         */
        template<class Predicate>
        bool wait_until(Mutex& m, nsecs abs_time, Predicate& p)
        {
            // abs_time is since epoch, so set p_time to epoch, then add our abs_time.
            boost::posix_time::ptime p_time = boost::posix_time::from_time_t(0);
            boost::posix_time::nanosec abs_p_time = boost::posix_time::nanoseconds(abs_time);
            // wakeup time = epoch date + time since epoch
            p_time = p_time + abs_p_time;
            return c.timed_wait<boost::timed_mutex>(m.m, p_time, p);
        }
#endif

    };
}}

#endif

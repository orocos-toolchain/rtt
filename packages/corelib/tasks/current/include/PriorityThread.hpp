
#ifndef PRIORITYTHREAD_HPP
#define PRIORITYTHREAD_HPP

#include "TaskExecution.hpp"
#include "Time.hpp"

namespace ORO_CoreLib
{

    template< int Priority >
    class PriorityTask;

    /**
     * This is a Task Executing Thread with a fixed priority. It is used
     * by the PriorityTask for it's scheduling needs.
     *
     * It Uses the Singleton pattern, since there will be only one EVER.
     *
     * @see PeriodicThread
     */
    template< int Priority >
    class PriorityThread
        : public TaskExecution
    {
    public:
        /**
         * Create a PriorityThread with a certain periodicity.
         *
         * @note period is to be set with the periodSet() method
         * Before any tasks are added.
         *
         */
        static PriorityThread<Priority>* Instance()
        {
            if ( _instance == 0 )
                _instance = new PriorityThread<Priority>( 0.01 );// XXX use some DEFAULT_PRIO define
            return _instance;
        }

        /**
         * Releases the PriorityThread
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release()
        {
            if ( _instance != 0 )
                {
                    delete _instance;
                    _instance = 0;
                    return true;
                }

            return false;
        }

        /**
         * Add a PriorityTask<Priority> which is handled each n nanoseconds
         * 
         * @param t The task to handle each n nanoseconds
         * @param n handle every n nanoseconds
         */
        bool taskAdd( PriorityTask<Priority>* t, const nsecs n )
        {
            return TaskExecution::taskAdd( t->handler(), n);
        }

        /**
         * Remove a PriorityTask<Priority> from handleing
         *
         * @post <t> is no longer handled by this thread
         */
        void taskRemove( PriorityTask<Priority>* t )
        {
            TaskExecution::taskRemove( t->handler() );
        }

    protected:

        /**
         * Constructor
         * @param _period The period in seconds.
         */
        PriorityThread(double _period)
            : TaskExecution( Priority, 
                             "", 
                             _period )
        {
            if ( Priority < ORONUM_CORELIB_TASKS_PRIOLIMIT )
                makeHardRealtime();
        }


        /**
         * Destructor
         */
        virtual ~PriorityThread() {}

    private:

        /**
         * Our only instance of the PriorityThread
         */
        static PriorityThread<Priority>* _instance;

    };

    // The static class variables
    template< int Priority>
    PriorityThread<Priority>* PriorityThread<Priority>::_instance;

} // namespace ORO_CoreLib

#endif

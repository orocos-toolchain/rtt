
#ifndef PRIORITYTHREAD_HPP
#define PRIORITYTHREAD_HPP

#include "TaskExecution.hpp"


namespace ORO_CoreLib
{

    /**
     * This is a Task Executing Thread with a fixed priority. It is used
     * by the PriorityTask for it's scheduling needs.
     *
     * It Uses the Singleton pattern, since there will be only one EVER.
     *
     * @see ComponentThreaded
     */
    template< int Priority >
    class PriorityThread
        : public TaskExecution
    {
    public:
        /**
         * Create a PriorityThread with a certain periodicity.
         *
         * @note period is ignored if the thread was created
         * before. One can check if the correct period was
         * set with the periodGet() function call.
         *
         * @param period The periodicity of the thread in seconds
         */
        static PriorityThread<Priority>* Instance(double period)
        {
            if ( _instance == 0 )
                _instance = new PriorityThread<Priority>( period );
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
    PriorityThread<Priority>* PriorityThread<Priority>::_instance;

} // namespace ORO_CoreLib

#endif

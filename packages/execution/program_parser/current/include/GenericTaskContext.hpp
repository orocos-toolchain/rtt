#ifndef GENERIC_TASK_CONTEXT_HPP
#define GENERIC_TASK_CONTEXT_HPP
// our base class :
#include <execution/TaskContext.hpp>

namespace ORO_Execution
{
    /**
     * This class specialises the TaskContext to offer some
     * default methods and commands which are for controlling
     * the task (being peroidic or not). Most methods are NOT
     * REALTIME, since they access the disk and allocate memory.
     * Thus you need to call them from the TaskBrowser or 
     * a non realtime task.
     */
    class GenericTaskContext
        :public TaskContext
    {
    
    public:
        /**
         * The constructor sets up a task with \a name to have some
         * common methods.
         */
        GenericTaskContext(const std::string& name);

        /**
         * Start is a method which starts the Processor's task.
         * It can not be a command because if the Processor is not running,
         * it does not accept commands. Also, RunnableInterface::initialize()
         * is then called in the context of the caller.
         */
        bool start();
        
        /**
         * Stop is a method which stops the Processor's task.
         * RunnableInterface::finalize()
         * is called in the context of the caller.
         */
        bool stop();
  
        /**
         * DataSource to inspect if this Task is running.
         */
        bool isRunning() const;

        /**
         * Read this Task's properties from a file.
         */
        bool readProperties(const std::string& filename);

        /**
         * Write this Task's properties to a file.
         */
        bool writeProperties(const std::string& filename);

        /**
         * Load an Orocos Program Script from disk.
         */
        bool loadProgram(const std::string& filename);

        /**
         * Load an Orocos State Description from disk.
         */
        bool loadStateMachine(const std::string& filename);

        /**
         * Unload an Orocos Program Script from disk.
         */
        bool unloadProgram(const std::string& progname);

        /**
         * Unload an Orocos State Description from disk.
         */
        bool unloadStateMachine(const std::string& instancename);
    };
}
#endif

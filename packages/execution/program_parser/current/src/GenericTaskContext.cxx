
#include <execution/GenericTaskContext.hpp>

#include <execution/ProgramLoader.hpp>
#include <execution/PropertyLoader.hpp>

// To create commands, methods, etc :
#include <execution/TemplateFactories.hpp>

namespace ORO_Execution
{
    GenericTaskContext::GenericTaskContext(const std::string& name)
        : TaskContext( name )
    {
        // See Task Infrastructure Chapter.

        // Register our methods :
        TemplateMethodFactory<GenericTaskContext>* mfact = newMethodFactory( this );
        mfact->add( "start",
                    method( &GenericTaskContext::start, "Start this GenericTaskContext." ) );
        mfact->add( "stop",
                    method( &GenericTaskContext::stop, "Stop this GenericTaskContext." ) );
        mfact->add( "readProperties",
                    method( &GenericTaskContext::readProperties, "Read Properties from a file.",
                            "Filename", "A CPF formatted XML file.") );
        mfact->add( "writeProperties",
                    method( &GenericTaskContext::writeProperties, "Write or update Properties to a file.",
                            "Filename", "A CPF formatted XML file.") );
        mfact->add( "loadProgram",
                    method( &GenericTaskContext::loadProgram, "Load an Orocos Program Script from a file.",
                            "Filename", "An ops file.") );
        mfact->add( "loadStateMachine",
                    method( &GenericTaskContext::loadStateMachine, "Load an Orocos State Description from a file.",
                            "Filename", "An osd file.") );
        mfact->add( "unloadProgram",
                    method( &GenericTaskContext::unloadProgram, "Unload an Orocos Program Script from a file.",
                            "ProgramName", "The Program's name.") );
        mfact->add( "unloadStateMachine",
                    method( &GenericTaskContext::unloadStateMachine, "Unload an Orocos State Description from a file.",
                            "StateMachineName", "A name of a Root StateMachine instantiation.") );
        this->methodFactory.registerObject("this", mfact); 

        // Register our datasources :
        TemplateDataSourceFactory<GenericTaskContext>* dsfact = newDataSourceFactory( this );
        dsfact->add( "isRunning",
                     data( &GenericTaskContext::isRunning, "Is this GenericTaskContext started ?" ) );
        this->dataFactory.registerObject("this", dsfact); 
    }

    GenericTaskContext::~GenericTaskContext()
    {}

    /**
     * Start is a method which starts the Processor's task.
     * It can not be a command because if the Processor is not running,
     * it does not accept commands. Also, RunnableInterface::initialize()
     * is then called in the context of the caller.
     */
    bool GenericTaskContext::start() {
        if ( this->getProcessor()->getTask() == 0 )
            return false;
        return this->getProcessor()->getTask()->start();
    }
        
    /**
     * Stop is a method which stops the Processor's task.
     * RunnableInterface::finalize()
     * is called in the context of the caller.
     */
    bool GenericTaskContext::stop() {
        if ( this->getProcessor()->getTask() == 0 )
            return false;
        return this->getProcessor()->getTask()->stop();
    }
  
    /**
     * DataSource to inspect if this Task is running.
     */
    bool GenericTaskContext::isRunning() const {
        if ( this->getProcessor()->getTask() == 0 )
            return false;
        return this->getProcessor()->getTask()->isRunning();
    }

    /**
     * Read this Task's properties from a file.
     */
    bool GenericTaskContext::readProperties(const std::string& filename) {
        PropertyLoader p;
        return p.configure(filename, this);
    }

    /**
     * Write this Task's properties to a file.
     */
    bool GenericTaskContext::writeProperties(const std::string& filename) {
        PropertyLoader p;
        return p.save(filename, this);
    }

    /**
     * Load an Orocos Program Script from disk.
     */
    bool GenericTaskContext::loadProgram(const std::string& filename) {
        ProgramLoader p;
        return p.loadProgram(filename, this);
    }

    /**
     * Load an Orocos State Description from disk.
     */
    bool GenericTaskContext::loadStateMachine(const std::string& filename) {
        ProgramLoader p;
        return p.loadStateMachine(filename, this);
    }

    /**
     * Unload an Orocos Program Script from disk.
     */
    bool GenericTaskContext::unloadProgram(const std::string& progname) {
        ProgramLoader p;
        return p.unloadProgram(progname, this);
    }

    /**
     * Unload an Orocos State Description from disk.
     */
    bool GenericTaskContext::unloadStateMachine(const std::string& instancename) {
        ProgramLoader p;
        return p.unloadStateMachine(instancename, this);
    }

}


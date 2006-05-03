/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  GenericTaskContext.cxx 

                        GenericTaskContext.cxx -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

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
        mfact->add( "assert",
                    method( &GenericTaskContext::assertion, "Assert will get your program in the error state if the argument is false.",
                            "MustBeTrue", "The result of a boolean expression which must be true.") );
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
        mfact->add( "isRunning",
                    method( &GenericTaskContext::isRunning, "Is this GenericTaskContext started ?" ) );
        mfact->add( "update",
                    method( &GenericTaskContext::trigger, "Invoke the update method. Only succeeds if the task isRunning()." ) );
        this->methods()->registerObject("this", mfact); 
    }

    GenericTaskContext::~GenericTaskContext()
    {}

    bool GenericTaskContext::assertion(bool mbt)
    {
        return mbt;
    }

    bool GenericTaskContext::trigger()
    {
        return this->engine()->getActivity()->trigger();
    }

    /**
     * Start is a method which starts the Processor's task.
     * It can not be a command because if the Processor is not running,
     * it does not accept commands. Also, RunnableInterface::initialize()
     * is then called in the context of the caller.
     */
    bool GenericTaskContext::start() {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->start();
    }
        
    /**
     * Stop is a method which stops the Processor's task.
     * RunnableInterface::finalize()
     * is called in the context of the caller.
     */
    bool GenericTaskContext::stop() {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->stop();
    }
  
    /**
     * DataSource to inspect if this Task is running.
     */
    bool GenericTaskContext::isRunning() const {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->isRunning();
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


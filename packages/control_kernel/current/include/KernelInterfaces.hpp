/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  KernelInterfaces.hpp 

                        KernelInterfaces.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef MOTIONKERNELINTERFACES_HPP
#define MOTIONKERNELINTERFACES_HPP


#include <corelib/NameServerRegistrator.hpp>
#include <corelib/RunnableInterface.hpp>
#include <corelib/PropertyBag.hpp>
#include <corelib/PropertyComposition.hpp>
#include <corelib/Event.hpp>
#include "ModuleControlInterface.hpp"

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/CommandFactoryInterface.hpp"
#include "execution/DataSourceFactory.hpp"
#endif

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
    
    namespace detail 
    {

        /**
         * @brief This interface is the base class of all Extensions.
         *
         * An Extension to the ControlKernel has a name and properties,
         * which define the behaviour of the Extension. Extensions 
         * communicate with components through their ComponentAspectInterface.
         */
        struct ExtensionInterface
            :public NameServerRegistrator<ExtensionInterface*,std::string>
        {
            typedef NameServer<ExtensionInterface*> NameServerType;
            /**
             * Construct a not nameserved ExtensionInterface.
             */
            ExtensionInterface() {}

            /**
             * Construct an ExtensionInterface with name <name>, registred with
             * ExtensionInterface::nameserver
             *
             */
            ExtensionInterface(const std::string name) 
                : NameServerRegistrator<ExtensionInterface*, std::string>(nameserver,name,this)
            {}

            virtual ~ExtensionInterface() {}

            /**
             * Returns the name of this extension.
             */
            const std::string& getName() { return extensionName; }
        
            /**
             * Update the properties with configuration data.
             */
            virtual bool updateProperties( const PropertyBag& bag ) = 0;
        
            /**
             * Nameserver for this interface.
             */
            static NameServer<ExtensionInterface*> nameserver;
        protected:
            /**
             * Unique name of this extension.
             */
            std::string extensionName;
        };
    
        /**
         * @brief An Aspect is a base class of a component, implementing a
         * functionality like reporting, configuration,...
         *
         * A template class for Aspects, which can be used, optionally, by Extensions of the
         * ControlKernel. This interface instructs the Aspect to register itself with a certain
         * Extension. You must use this as a base class for every Aspect you want to provide
         * to a component.
         *
         * The base class of most Extensions is the ExtensionInterface.
         */
        template< class _Extension >
        struct ComponentAspectInterface
        {
            /**
             * The Extension (of the kernel) that this Aspect provides support for.
             */
            typedef _Extension Extension;
        
            /**
             * Initialize the ComponentAspect with the name of the Aspect.
             * Once set, a name can no longer be changed.
             */
            ComponentAspectInterface(const std::string& comp_name )
                : aspectName("Name", "The Name of this Component", comp_name) {}

            virtual ~ComponentAspectInterface() {}
        
            /**
             * Instructs the component to enable an aspect so that this aspect can
             * deliver a service to the Extension <ext> of the ControlKernel. 
             * This will be called when the component is connected/added to the kernel.
             */
            virtual bool enableAspect( Extension* ext ) = 0;

            /**
             * Disable this aspect and no longer use the Extension of the kernel.
             */
            virtual void disableAspect() = 0;

            /**
             * Return the name of this Aspect instance.
             */
            virtual const std::string& getName() { return aspectName.get(); }

        protected:
            const Property<std::string> aspectName;
        };
    }

    class KernelBaseFunction;

    /**
     * @brief The Base class of each ControlKernel Component
     *
     * The most fundamental aspect of a Component is that it belongs
     * to a ControlKernel and can be notified of the kernel's status.
     * This aspect introduces the ability to detect if the component
     * is placed in a ControlKernel and to return a pointer to this kernel.
     * It is the aspect of the KernelBaseFunction.
     * 
     */
    class ComponentBaseInterface 
        :public ModuleControlInterface,
         public detail::ComponentAspectInterface< KernelBaseFunction >
    {
    public:
        /**
         * Constructor.
         */
        ComponentBaseInterface(const std::string& name)
            : detail::ComponentAspectInterface< KernelBaseFunction >( name ),
              kern(0) {}

        virtual ~ComponentBaseInterface() {}
            
        /**
         * Inspect if this component is placed in a kernel.
         * 
         * @return true if it is so.
         */
        bool inKernel() { return kern != 0; }
            
        /**
         * Return the kernel this component belongs to.
         * 
         * @return The kernel it belongs to, zero if none.
         */
        KernelBaseFunction* kernel() { return kern; }

        virtual bool enableAspect(KernelBaseFunction* e);

        /**
         * This method is a hook which is called when the component
         * is loaded into the kernel.
         *
         * @return true on success, false otherwise
         */
        virtual bool componentLoaded() { return true;}
            
        /**
         * This method is a hook which is called when a component
         * was unloaded from the kernel.
         */
        virtual void componentUnloaded() {}

        /**
         * This method is a hook which is called when the kernel
         * is started and the component must initialise the data
         * objects with meaningfull data.
         *
         * @return true on success, false otherwise
         */
        virtual bool componentStartup() { return true; }

        /**
         * This method is a hook which is called when the kernel
         * is stopped and the component must return to a safe state.
         */
        virtual void componentShutdown() {}
            
        virtual void disableAspect();

    private:
        KernelBaseFunction* kern;
    };

    /**
     * @brief The DefaultBase is defined for
     * ControlKernels that do not know of 
     * Component Aspects. It will be used as default
     * template parameter for the components.
     *
     * A Component Aspect is the base class that a Component
     * must have to be able to be queried by the respective Kernel Extension.
     * As a consequence, Extensions themselves define the component base class they 
     * require. That class is passed through by the kernel to the Component.
     * This relieves the burden of figuring it out manually from the shoulders of the user.
     */
    typedef ComponentBaseInterface DefaultBase;

    /**
     * @brief The 'Extension' that updates the components.
     *
     * This Extension is actually the base functionality
     * of a ControlKernel, updating the components, storing general properties,
     * generating kernel events. The Aspect it uses is the 
     * ComponentBaseInterface.
     * 
     */
    struct KernelBaseFunction
        :public ORO_CoreLib::RunnableInterface
    {
        friend class ComponentBaseInterface;
        
        /**
         * The CommonBase is the ModuleControlInterface
         * and KernelConnectionInterface.
         */
        typedef ComponentBaseInterface CommonBase;

        KernelBaseFunction( KernelBaseFunction* _base=0 );

        virtual ~KernelBaseFunction();

        virtual bool initialize() ;
        virtual void step() ;
        virtual void finalize() ;

        virtual TaskInterface* getTask() const;

        virtual void setTask( TaskInterface* task );

        const std::string& getKernelName() const;

        /**
         * Returns true if the kernel is running, false
         * otherwise.
         */
        bool isRunning() const { return running; }

        /**
         * Get the running periodicity in seconds.
         *
         * @return the running periodicity in seconds
         */
        double getPeriod() const;

        /**
         * Set the running periodicity in seconds.
         *
         * @param p The periodicity in seconds.
         */
        void setPeriod( double p );

        template<class T>
        void enable(detail::ComponentAspectInterface<T>* c)
        {
            c->enableAspect(this);
        }

        template<class T>
        void disable(detail::ComponentAspectInterface<T>* c)
        {
            c->disableAspect();
        }

        bool updateProperties(const PropertyBag& bag);

        void startupComponents(const PropertyBag& bag);

        /**
         * This is the hook for user kernel properties.
         * Add properties to your kernel config file and they
         * will be passed to this function.
         */
        virtual bool updateKernelProperties(const PropertyBag& bag);

        /**
         * @brief Select a Sensor Component from the kernel.
         *
         * @param  name The name of the Sensor Component to select.
         * @return True if the Sensor Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectSensor( const std::string& name ) = 0;
        /**
         * Select a Estimator Component from the kernel.
         *
         * @param  name The name of the Estimator Component to select.
         * @return True if the Estimator Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectEstimator( const std::string& name ) = 0;
        /**
         * Select a Generator Component from the kernel.
         *
         * @param  name The name of the Generator Component to select.
         * @return True if the Generator Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectGenerator( const std::string& name ) = 0;
        /**
         * Select a Controller Component from the kernel.
         *
         * @param  name The name of the Controller Component to select.
         * @return True if the Controller Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectController( const std::string& name ) = 0;
        /**
         * @brief Select a Effector Component from the kernel.
         *
         * @param  name The name of the Effector Component to select.
         * @return True if the Effector Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectEffector( const std::string& name ) = 0;

        /**
         * @brief Query if a named Controller is selected.
         * @param  name The name of the Controller Component.
         * @return True if the Controller is selected
         *         False otherwise.
         */
        virtual bool isSelectedController( const std::string& name ) const = 0;
        /**
         * @brief Query if a named Generator is selected.
         * @param  name The name of the Generator Component.
         * @return True if the Generator is selected
         *         False otherwise.
         */
        virtual bool isSelectedGenerator( const std::string& name ) const = 0;
        /**
         * @brief Query if a named Estimator is selected.
         * @param  name The name of the Estimator Component.
         * @return True if the Estimator is selected
         *         False otherwise.
         */
        virtual bool isSelectedEstimator( const std::string& name ) const = 0;
        /**
         * @brief Query if a named Sensor is selected.
         * @param  name The name of the Sensor Component.
         * @return True if the Sensor is selected
         *         False otherwise.
         */
        virtual bool isSelectedSensor( const std::string& name ) const = 0;
        /**
         * @brief Query if a named Effector is selected.
         * @param  name The name of the Effector Component.
         * @return True if the Effector is selected
         *         False otherwise.
         */
        virtual bool isSelectedEffector( const std::string& name ) const = 0;

#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
        virtual ORO_Execution::CommandFactoryInterface* createCommandFactory();

        virtual ORO_Execution::DataSourceFactory* createDataSourceFactory();
#endif

        /**
         * This method can be called to abort the startup process 
         * of the kernel, when a not sane configuration was detected
         * by the extensions, or somewhere else in the kernel. The
         * result is that the finalize() methods are called of all
         * extensions.
         *
         * @note, maybe such a method (or event) must be made
         * available to the components too.
         */
        void abortKernel()
        {
            running = false;
        }
    
        HandlerRegistrationInterface* eventGet(const std::string& name);

    protected:

        void setKernelName( const std::string& _name);

        /**
         * Used by the ComponentBaseInterface to register itself to
         * this Extension.
         */
        void addComponent(ComponentBaseInterface* comp);

        /**
         * Used by the ComponentBaseInterface to deregister itself
         * from this Extension.
         */
        void removeComponent(ComponentBaseInterface* comp);

        /**
         * This is to be implemented by the derived Kernel.
         * We can not update the components in this class because
         * the order is unknown. The order in which the components
         * must be updated is known in the Kernel subclass.
         */
        virtual void updateComponents() = 0;
        
        /**
         * Flag to keep track of running state.
         */
        bool running;

    private:
        Property<std::string> name;

        /**
         * The periodicity of this kernel.
         */
        Property<double> frequency;

        std::vector<ComponentBaseInterface*> components;

        TaskInterface* mytask;
    protected:
        Event kernelStarted;
        Event kernelStopped;
        Event nullEvent;

        Property<std::string> startupSensor;
        Property<std::string> startupEstimator;
        Property<std::string> startupGenerator;
        Property<std::string> startupController;
        Property<std::string> startupEffector;
    };


    namespace detail 
    {

        /**
         * A class for composing Aspects (if you want more than
         * one Aspect in your component).
         * 
         */
        template<class First, class Second>
        struct CompositeAspect
            : public First, public Second
        {
            CompositeAspect( const std::string& _name ) 
                :First(_name), Second(_name) {}

            virtual ~CompositeAspect() {}

            // this removes compiler warnings, about
            // enableAspect being hidden, we want them
            // to be hidden, but hey, at least we lost the
            // warning...and this also works.
            using First::enableAspect;
            using Second::enableAspect;

            template< class Extension >
            bool enableAspect(Extension* ext) 
            {
                // Enable first aspect, if successful, enable second,
                // if also successful, return true, otherwise, disable first
                // aspect (~undo) and return false.
                return (First::enableAspect(ext) &&
                        ( Second::enableAspect(ext) || (First::disableAspect(), false) ) );
            }

            void disableAspect() 
            {
                First::disableAspect();
                Second::disableAspect();
            }

        };

    }
    
    /**
     * @brief A class for composing extensions (if you want more than
     * one extension in your kernel). 
     *
     * Composing allows the user to specify order of initialisation,
     * execution and cleanup of multiple Extensions in one kernel.
     * 
     * @param First The first Extension to be started and executed
     * relatively to the Second.
     * @param Second The second Extension to be started and executed
     * relatively to the First.
     */
    template<class First, class Second>
    struct CompositeExtension
        : public First, public Second
    {
        /**
         * The Aspects of Both Extensions are composed 
         * automatically in the same First,Second way.
         */
        typedef detail::CompositeAspect<typename First::CommonBase,typename Second::CommonBase> CommonBase;
            
        CompositeExtension() {}
        CompositeExtension( KernelBaseFunction* _base ) : First(_base), Second(_base) {}

        virtual ~CompositeExtension() {}

        virtual bool initialize() 
        {
            return (First::initialize() &&
                    (Second::initialize() || (First::finalize(),false) ) );
        }

        virtual void step() 
        {
            First::step();
            Second::step();
        }

        virtual void finalize() 
        {
            First::finalize();
            Second::finalize();
        }
    };

}

#endif

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
#include "DataObjectInterfaces.hpp"
#include "PortInterfaces.hpp"

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
    
    namespace detail 
    {

    /**
     * This interface allows us to manage extensions for configuration.
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
     * A template class for Aspects, which can be used, optionally, by Extensions of the
     * kernel. This interface instructs the Aspect to register itself with a certain
     * Extension. You must use this as a base class for every Aspect you want to provide
     * to a component.
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
         */
        ComponentAspectInterface(const std::string& comp_name ) : aspectName(comp_name) {}

        virtual ~ComponentAspectInterface() {}
        
        /**
         * Instructs the component to enable an aspect so that this aspect can
         * deliver a service to the Extension <ext> of thekernel. 
         * This will be called when the component is connected/added to the kernel.
         */
        virtual bool enableAspect( Extension* ext ) = 0;

        /**
         * Disable this aspect and no longer use the extension of the kernel.
         */
        virtual void disableAspect() = 0;

        virtual const std::string& getName() { return aspectName; }

        const std::string aspectName;
    };

    }

    class KernelBaseFunction;

    /**
     * The most logical aspect of a Component is that it belongs
     * to a Kernel and can be controlled.
     * This aspect introduces the ability to detect if the component
     * is placed in a kernel and to return a pointer to this kernel.
     * It is the aspect of the DefaultExtension.
     * 
     */
    class ComponentBaseInterface 
        :public ModuleControlInterface
    {
        public:
            /**
             * Constructor.
             */
            ComponentBaseInterface(const std::string&) : kern(0) {}

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
     * @brief The DefaultBase is defined for ControlKernels that do not know of 
     * Component Aspects.
     * It will be used as default template parameter for the components.
     *
     * A Component Aspect is the base classe that a Component
     * must have to be able to be queried by the respective Kernel Extension.
     * As a consequence, Extensions themselves define the base class they 
     * require. This base class is passed through by the kernel to the Component.
     * This relieves the burden of figuring it out manually from the shoulders of the user.
     */
    typedef ComponentBaseInterface DefaultBase;

    /**
     * Each extension to the BaseKernel needs 
     * to implement the RunnableInterface and define a
     * common Aspect (DefaultBase). 
     * This Extension is actually the base functionality
     * of a kernel, which can handle aspects.
     */
    struct KernelBaseFunction
        :public RunnableInterface
    {
        friend class ComponentBaseInterface;
        
        /**
         * The CommonBase is the ModuleControlInterface
         * and KernelConnectionInterface.
         */
        typedef ComponentBaseInterface CommonBase;

        KernelBaseFunction( KernelBaseFunction* _base=0 )
            : running(false), 
              frequency("frequency","The periodic execution frequency of this kernel",0),
              kernelStarted(Event::SYNASYN), kernelStopped(Event::SYNASYN), nullEvent(Event::SYNASYN)
        {}

        virtual ~KernelBaseFunction() {}

        virtual bool initialize() 
        { 
            running = true; 
            return true;
        }
        virtual void step() 
        { 
            // update the components 
            updateComponents();
        }
        virtual void finalize() 
        { 
            running = false; 
        }

        /**
         * Returns true if the kernel is running, false
         * otherwise.
         */
        bool isRunning() { return running; }

        /**
         * Get the running periodicity in seconds.
         *
         * @return the running periodicity in seconds
         */
        double getPeriod() { return 1./frequency; }

        /**
         * Set the running periodicity in seconds.
         *
         * @param p The periodicity in seconds.
         */
        void setPeriod( double p ) { frequency = 1./p; }

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

        virtual bool updateProperties(const PropertyBag& bag)
        {
           return composeProperty(bag, frequency);
        }
        
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
    
        HandlerRegistrationInterface* eventGet(const std::string& name)
        {
            if ( name == std::string("kernelStarted") )
                return &kernelStarted;
            if ( name == std::string("kernelStopped") )
                return &kernelStopped;
            return &nullEvent;
        }
    protected:
        /**
         * Used by the ComponentBaseInterface to register itself to
         * this Extension.
         */
        void addComponent(ComponentBaseInterface* comp)
        {
            components.push_back(comp);
        }

        /**
         * Used by the ComponentBaseInterface to deregister itself
         * from this Extension.
         */
        void removeComponent(ComponentBaseInterface* comp)
        {
            std::vector<ComponentBaseInterface*>::iterator itl 
                = std::find(components.begin(), components.end(), comp);
            if (itl != components.end() )
                components.erase(itl);
        }

        /**
         * This is to be implemented by the derived Kernel.
         * We can not update the components in this class because
         * the order is unknown. The order in which the components
         * must be updated is known in the Kernel subclass.
         */
        virtual void updateComponents() = 0;
        
    private:
        /**
         * Flag to keep track of running state.
         */
        bool running;

        /**
         * The periodicity of this kernel.
         */
        Property<double> frequency;

        std::vector<ComponentBaseInterface*> components;

    protected:
        Event kernelStarted;
        Event kernelStopped;
        Event nullEvent;
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
                    Second::initialize() );
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

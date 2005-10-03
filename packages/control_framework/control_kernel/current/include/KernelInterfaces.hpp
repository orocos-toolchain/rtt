/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  KernelInterfaces.hpp 

                        KernelInterfaces.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef KERNELINTERFACES_HPP
#define KERNELINTERFACES_HPP


#include <corelib/NameServerRegistrator.hpp>
#include <corelib/RunnableInterface.hpp>
#include <corelib/Property.hpp>
#include <corelib/Event.hpp>

#include <pkgconf/control_kernel.h>
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
#include "execution/CommandFactoryInterface.hpp"
#include "execution/DataSourceFactoryInterface.hpp"
#include "execution/MethodFactoryInterface.hpp"
#include "execution/AttributeRepository.hpp"
#endif

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    namespace detail 
    {
        struct ExtensionInterface;
        struct ComponentStateInterface;
    }

    class KernelBaseFunction;
    class ComponentBaseInterface;

    /**
     * @brief This is the base class of any kernel.
     * It holds a pointer to all extensions present.
     */
    struct ControlKernelInterface
        :public ORO_CoreLib::RunnableInterface
    {
        friend class detail::ExtensionInterface;

        typedef std::vector<detail::ExtensionInterface*> ExtensionList;

        virtual ~ControlKernelInterface();

        ControlKernelInterface(const std::string& kname);

        /**
         * @brief This is the hook for user kernel properties.
         *
         * Add properties to your kernel config file and they
         * will be passed to this function.
         */
        virtual bool updateKernelProperties(const PropertyBag& bag);

        /**
         * @brief Get a vector of all ExtensionInterfaces of this Kernel.
         */
        const ExtensionList& getExtensions() const
        {
            return extensions;
        }

        /**
         * Get an Extension of type T
         * @return a pointer to extension T or null if not present.
         */
        template<class T>
        T* getExtension() const {
            for ( ExtensionList::const_iterator ci = extensions.begin(); ci != extensions.end(); ++ci)
                if ( dynamic_cast<T*>( *ci ) )
                    return dynamic_cast<T*>( *ci );
            return 0;
        }

        const std::string& getKernelName() const;

        /**
         * Returns a pointer to the base interface of this kernel.
         * May return zero if not found.
         */
        KernelBaseFunction* base() const;

        void base(KernelBaseFunction* );
    protected:
       
        /**
         * Method for ExtensionInterface to register itself.
         */
        void addExtension( detail::ExtensionInterface* e) {
            extensions.push_back(e);
        }

        void setKernelName( const std::string& _name);

        ExtensionList extensions;

        Property<std::string> name;

        KernelBaseFunction* base_;
    };

    namespace detail 
    {

        /**
         * @brief This interface is the base class of all Extensions.
         *
         * An Extension to the ControlKernel has a name and properties,
         * which define the behaviour of the Extension. Extensions 
         * communicate with components through their ComponentFacetInterface.
         */
        struct ExtensionInterface
        {
            /**
             * @brief Construct an ExtensionInterface with name \a name
             *
             */
            ExtensionInterface(ControlKernelInterface* kernel, const std::string name) 
                :extensionName(name), _kernel(kernel)
            {
                kernel->addExtension(this);
            }

            virtual ~ExtensionInterface() {}

            /**
             * @brief Returns the name of this extension.
             */
            const std::string& getName() const { return extensionName; }
        
            /**
             * @brief Update the properties with configuration data.
             */
            virtual bool updateProperties( const PropertyBag& bag ) = 0;
        
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
            /**
             * @brief Export the properties with configuration data.
             */
            virtual bool exportProperties( ORO_Execution::AttributeRepository& bag ) { return true; };
        
            virtual ORO_Execution::CommandFactoryInterface* createCommandFactory() { return 0; }

            virtual ORO_Execution::DataSourceFactoryInterface* createDataSourceFactory()  { return 0; }

            virtual ORO_Execution::MethodFactoryInterface* createMethodFactory()  { return 0; }
#endif
        protected:
            /**
             * Return a pointer of the kernel of this Extension.
             */
            ControlKernelInterface* kernel() const { return _kernel; }

            /**
             * @brief This method is called when the kernel is started.
             * @return true if startup may proceed, false otherwise.
             */
            virtual bool initialize() = 0;
            
            /**
             * @brief This method is called on each periodic execution
             * of the kernel, in the order of the Extensions.
             */
            virtual void step() = 0;

            /**
             * @brief This method is called after the last
             * periodic step.
             */
            virtual void finalize() = 0;

        private:
            /**
             * @brief Unique name of this extension.
             */
            std::string extensionName;
            ControlKernelInterface* _kernel;
        };

    }

    /**
     * @brief The 'Extension' that updates the components.
     *
     * This Extension is actually the base functionality
     * of a ControlKernel, updating the components, storing general properties,
     * generating kernel events. The Facet it uses is the 
     * ComponentBaseInterface.
     * 
     */
    struct KernelBaseFunction
        :public detail::ExtensionInterface
    {
        friend class ComponentBaseInterface;
        
        /**
         * The CommonBase is the 
         * ComponentBaseInterface.
         */
        typedef ComponentBaseInterface CommonBase;

        KernelBaseFunction( ControlKernelInterface* _base );

        virtual ~KernelBaseFunction();

        virtual bool initialize() ;
        virtual void step() ;
        virtual void finalize() ;

        /**
         * Get the Task of the ControlKernelInterface.
         */
        TaskInterface* getTask() const;

        /**
         * Get the name of the ControlKernelInterface.
         */
        //const std::string& getKernelName() const;

        /**
         * @brief Returns true if the kernel is running, false
         * otherwise.
         */
        bool isRunning() const { return getTask() && getTask()->isRunning(); }

        /**
         * @brief Get the running periodicity in seconds.
         *
         * @return the running periodicity in seconds
         */
        double getPeriod() const;

        /**
         * @brief Set the running periodicity in seconds.
         *
         * @param p The periodicity in seconds.
         */
        void setPeriod( double p );

        /**
         * @brief Select a Component from the kernel.
         *
         * @param  name The name of the Component to select.
         * @return True if the Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectComponent( const std::string& name ) = 0;

        /**
         * @brief Query if a named Component is selected.
         * @param  name The name of the Component.
         * @return True if the Component is selected
         *         False otherwise.
         */
        virtual bool isSelected( const std::string& name ) const = 0;

        /**
         * @brief Query if a named Component is started.
         * @param  name The name of the Component
         * @return True if the Component is started
         *         False otherwise.
         */
        virtual bool isStarted( const std::string& name ) const = 0;

        /**
         * @brief Query if a named Component is loaded.
         * @param  name The name of the Component
         * @return True if the Component is loaded
         *         False otherwise.
         */
        virtual bool isLoaded( const std::string& name ) const = 0;

        bool updateProperties(const PropertyBag& bag);

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_PROPERTY
        /**
         * Load the Control Kernel and Extensions properties from
         * a given kernelconfig.xml file \a filename.
         */
        bool loadProperties( const std::string& filename );
#endif

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        /**
         * Inform the Kernel and its Extensions to reread their
         * properties and reconfigure if necessary.
         */
        bool refreshProperties();

        bool exportProperties( ORO_Execution::AttributeRepository& bag );

        virtual ORO_Execution::DataSourceFactoryInterface* createDataSourceFactory() ;

        virtual ORO_Execution::MethodFactoryInterface* createMethodFactory() ;
#endif

        /**
         * @brief Start a Component from the kernel.
         *
         * @param  name The name of the Component to start.
         * @return True if the Component could be found and started,
         *         False otherwise.
         */
        virtual bool startComponent( const std::string& name ) = 0;

        /**
         * @brief Stop a Component from the kernel.
         *
         * @param  name The name of the Component to stop.
         * @return True if the  Component could be found and stopped,
         *         False otherwise.
         */
        virtual bool stopComponent( const std::string& name ) = 0;

        /**
         * Start the task associated with this kernel.
         * @return false if no task associated or already running
         *         true if successful
         */
        bool startKernel() {
            if ( this->kernel()->getTask() )
                return this->kernel()->getTask()->start();
            return false;
        }

        /**
         * Stop the task associated with this kernel.
         * @return false if no task associated.
         *         true if successful
         */
        bool stopKernel() {
            if ( this->kernel()->getTask() ) {
                this->kernel()->getTask()->stop();
                return true;
            }
            return false;
        }

        /**
         * Abort the kernel task. An event handler will
         * be scheduled in the CompletionProcessor to
         * stop the kernel's task.
         */
        void abortKernel()
        {
            if ( getTask() )
                abortKernelEvent.fire(); // dispatch to completion processor
        }
    
        Event<void(void)>* eventGet(const std::string& name);


        /**
         * @name Generic Component Methods
         * @brief After a component is \a add() 'ed, these
         * methods can be called to change their state.
         * @{
         */

        /**
         * @brief Load a previously added Component.
         * @param c The Component to load.
         *
         * @return true if the component is present and could be loaded.
         */
         //virtual bool load( ComponentBaseInterface* c) = 0;

        /**
         * @brief Unload a previously added Component.
         * @param c The Component to unload.
         *
         * @return true if the component is present and could be unloaded.
         */
        virtual bool unload( ComponentBaseInterface* c) = 0;

        /**
         * @brief Reload a previously loaded Component.
         * @param c The Component to reload.
         *
         * @return true if the component is present and could be reloaded.
         */
        virtual bool reload( ComponentBaseInterface* c)  = 0;

        /**
         * @brief Shutdown (deselect) a previously added Component.
         * @param c The Component to shutdown.
         * @post  The default component will be selected.
         *
         * @return true if the component is present and could be shutdowned.
         */
        virtual bool shutdown( ComponentBaseInterface* c) = 0;

        /**
         * @brief Startup (select) a previously added Component.
         * @param c The Component to startup.
         *
         * @return true if the component is present and could be started.
         */
        virtual bool startup( ComponentBaseInterface* c) = 0;

        /**
         * @brief Restart (deselect + select) a previously added Component.
         * @param c The Component to restart.
         *
         * @return true if the component is present and could be restarted.
         */
        virtual bool restart( ComponentBaseInterface* c) = 0;

        /*
         * @brief Select ( startup ) a previously added Component.
         * @param c The Component to select.
         *
         * @return true if the component is present and could be restarted.
         */
        //virtual bool select( ComponentBaseInterface* c) = 0;

        /** @} */

        /**
         * @{
         * @brief Nameserved components
         *
         */
        NameServer<ComponentBaseInterface*> controllers;
        NameServer<ComponentBaseInterface*>  generators;
        NameServer<ComponentBaseInterface*>   effectors;
        NameServer<ComponentBaseInterface*>  estimators;
        NameServer<ComponentBaseInterface*>     sensors;
        NameServer<ComponentBaseInterface*>    supports;
        /* @} */

        typedef std::map<std::string,detail::ComponentStateInterface*> ComponentMap;

    protected:
        bool startupComponent(ComponentBaseInterface* c ) const;
        
        void shutdownComponent(ComponentBaseInterface* c ) const;
        
        /**
         * This is to be implemented by the derived Kernel.
         * We can not update the components in this class because
         * the order is unknown. The order in which the components
         * must be updated is known in the Kernel subclass.
         */
        virtual void updateComponents() = 0;

        /**
         * To be overridden in the Kernel implementations.
         * This is a hook which must allows kernels to place
         * code just before or after the componentLoaded/componentUnloaded
         * method is called on a component.
         */
        virtual void preLoad(ComponentBaseInterface* comp) {}

        /**
         * To be overridden in the Kernel implementations.
         * This is a hook which must allows kernels to place
         * code just before or after the componentLoaded/componentUnloaded
         * method is called on a component.
         */
        virtual void postLoad(ComponentBaseInterface* comp) {}

        /**
         * To be overridden in the Kernel implementations.
         * This is a hook which must allows kernels to place
         * code just before or after the componentLoaded/componentUnloaded
         * method is called on a component.
         */
        virtual void preUnload(ComponentBaseInterface* comp) {}

        /**
         * To be overridden in the Kernel implementations.
         * This is a hook which must allows kernels to place
         * code just before or after the componentLoaded/componentUnloaded
         * method is called on a component.
         */
        virtual void postUnload(ComponentBaseInterface* comp) {}

        /**
         * @brief Used to switch two components, unselecting the old
         * one and selecting the new one.
         * @return the newly selected component, zero in case
         * the switching completely failed (none of both could be started/restarted).
         */
        ComponentBaseInterface* switchComponent(ComponentBaseInterface* oldC, ComponentBaseInterface* newC ) const;

    private:
        /**
         * Used by the ComponentBaseInterface to register itself to
         * this Extension.
         */
        bool addComponent(ComponentBaseInterface* comp);

        /**
         * Used by the ComponentBaseInterface to deregister itself
         * from this Extension.
         */
        void removeComponent(ComponentBaseInterface* comp);

        /**
         * The periodicity of this kernel.
         */
        Property<double> frequency;

        void abortHandler() {
            if ( getTask() )
                getTask()->stop();
        }
    protected:
        ComponentMap components;

        Event<void(void)> abortKernelEvent;
        Event<void(void)> kernelStarted;
        Event<void(void)> kernelStopped;
        Event<void(void)> nullEvent;

        Property<std::string> startupSensor;
        Property<std::string> startupEstimator;
        Property<std::string> startupGenerator;
        Property<std::string> startupController;
        Property<std::string> startupEffector;
    };

}

#endif

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
#endif

#pragma interface

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    namespace detail 
    {
        struct ExtensionInterface;
    }

    /**
     * @brief This is the base class of any kernel.
     * It holds a pointer to all extensions present.
     */
    struct ControlKernelInterface
        :public ORO_CoreLib::RunnableInterface
    {
        friend class detail::ExtensionInterface;

        virtual ~ControlKernelInterface() {}

        ControlKernelInterface() : name("name","The name of the kernel.", "Default") {}

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
        const std::vector<detail::ExtensionInterface*>& getExtensions() const
        {
            return extensions;
        }

        const std::string& getKernelName() const;

    protected:
        /**
         * Method for ExtensionInterface to register itself.
         */
        void addExtension( detail::ExtensionInterface* e) {
            extensions.push_back(e);
        }

        void setKernelName( const std::string& _name);

        std::vector<detail::ExtensionInterface*> extensions;

        Property<std::string> name;
    };

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
        {
            /**
             * @brief Construct an ExtensionInterface with name \a name
             *
             */
            ExtensionInterface(ControlKernelInterface* kernel, const std::string name) 
                :extensionName(name)
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
            virtual ORO_Execution::CommandFactoryInterface* createCommandFactory() { return 0; }

            virtual ORO_Execution::DataSourceFactoryInterface* createDataSourceFactory()  { return 0; }
#endif
        protected:
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

            /**
             * @brief Unique name of this extension.
             */
            std::string extensionName;
        };

    }

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
        const std::string& getKernelName() const;

        /**
         * @brief Returns true if the kernel is running, false
         * otherwise.
         */
        bool isRunning() const { return running; }

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

        bool updateProperties(const PropertyBag& bag);

        /**
         * @brief Select a Sensor Component from the kernel.
         *
         * @param  name The name of the Sensor Component to select.
         * @return True if the Sensor Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectSensor( const std::string& name ) = 0;
        /**
         * @brief Select a Estimator Component from the kernel.
         *
         * @param  name The name of the Estimator Component to select.
         * @return True if the Estimator Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectEstimator( const std::string& name ) = 0;
        /**
         * @brief Select a Generator Component from the kernel.
         *
         * @param  name The name of the Generator Component to select.
         * @return True if the Generator Component could be found and selected,
         *         False otherwise.
         */
        virtual bool selectGenerator( const std::string& name ) = 0;
        /**
         * @brief Select a Controller Component from the kernel.
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

#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        virtual ORO_Execution::CommandFactoryInterface* createCommandFactory() ;

        virtual ORO_Execution::DataSourceFactoryInterface* createDataSourceFactory() ;
#endif

        /**
         * @brief Start a Sensor Component from the kernel.
         *
         * @param  name The name of the Sensor Component to start.
         * @return True if the Sensor Component could be found and started,
         *         False otherwise.
         */
        virtual bool startSensor( const std::string& name ) = 0;
        /**
         * @brief Start a Estimator Component from the kernel.
         *
         * @param  name The name of the Estimator Component to start.
         * @return True if the Estimator Component could be found and started,
         *         False otherwise.
         */
        virtual bool startEstimator( const std::string& name ) = 0;
        /**
         * @brief Start a Generator Component from the kernel.
         *
         * @param  name The name of the Generator Component to start.
         * @return True if the Generator Component could be found and started,
         *         False otherwise.
         */
        virtual bool startGenerator( const std::string& name ) = 0;
        /**
         * @brief Start a Controller Component from the kernel.
         *
         * @param  name The name of the Controller Component to start.
         * @return True if the Controller Component could be found and started,
         *         False otherwise.
         */
        virtual bool startController( const std::string& name ) = 0;
        /**
         * @brief Start a Effector Component from the kernel.
         *
         * @param  name The name of the Effector Component to start.
         * @return True if the Effector Component could be found and started,
         *         False otherwise.
         */
        virtual bool startEffector( const std::string& name ) = 0;


        /**
         * @brief Stop a Sensor Component from the kernel.
         *
         * @param  name The name of the Sensor Component to stop.
         * @return True if the Sensor Component could be found and stopped,
         *         False otherwise.
         */
        virtual bool stopSensor( const std::string& name ) = 0;
        /**
         * @brief Stop a Estimator Component from the kernel.
         *
         * @param  name The name of the Estimator Component to stop.
         * @return True if the Estimator Component could be found and stopped,
         *         False otherwise.
         */
        virtual bool stopEstimator( const std::string& name ) = 0;
        /**
         * @brief Stop a Generator Component from the kernel.
         *
         * @param  name The name of the Generator Component to stop.
         * @return True if the Generator Component could be found and stopped,
         *         False otherwise.
         */
        virtual bool stopGenerator( const std::string& name ) = 0;
        /**
         * @brief Stop a Controller Component from the kernel.
         *
         * @param  name The name of the Controller Component to stop.
         * @return True if the Controller Component could be found and stopped,
         *         False otherwise.
         */
        virtual bool stopController( const std::string& name ) = 0;
        /**
         * @brief Stop a Effector Component from the kernel.
         *
         * @param  name The name of the Effector Component to stop.
         * @return True if the Effector Component could be found and stopped,
         *         False otherwise.
         */
        virtual bool stopEffector( const std::string& name ) = 0;

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
        virtual bool load( ComponentBaseInterface* c) = 0;

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

    protected:
        bool true_gen() const { return true; }

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

        bool startComponent(ComponentBaseInterface* c ) const ;
        
        void stopComponent(ComponentBaseInterface* c ) const ;
        
        /**
         * Flag to keep track of running state.
         */
        bool running;

    private:

        /**
         * The periodicity of this kernel.
         */
        Property<double> frequency;

    protected:
        typedef std::map<std::string,ComponentBaseInterface*> ComponentMap;

        ComponentMap components;

        Event<void(void)> kernelStarted;
        Event<void(void)> kernelStopped;
        Event<void(void)> nullEvent;

        Property<std::string> startupSensor;
        Property<std::string> startupEstimator;
        Property<std::string> startupGenerator;
        Property<std::string> startupController;
        Property<std::string> startupEffector;

        ControlKernelInterface* cki;
    };

}

#endif

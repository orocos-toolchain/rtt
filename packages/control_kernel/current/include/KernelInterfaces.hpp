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
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
        virtual ORO_Execution::CommandFactoryInterface* createCommandFactory();

        virtual ORO_Execution::DataSourceFactoryInterface* createDataSourceFactory();
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

        /** @} */

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


    namespace detail {
        /**
         * @brief This is the base class of any kernel. It is
         * deliberately empty.
         */
        struct ControlKernelInterface {};
    }
}

#endif

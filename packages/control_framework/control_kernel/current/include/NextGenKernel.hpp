/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  NextGenKernel.hpp 

                        NextGenKernel.hpp -  description
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
 
#ifndef NEXTGEN_KERNEL_HPP
#define NEXTGEN_KERNEL_HPP

#include "KernelInterfaces.hpp"
#include <corelib/DataObjectInterfaces.hpp>
#include "PortInterfaces.hpp"
#include "BaseComponents.hpp"
#include "ControlKernelProcess.hpp"
#include "ComponentStateInterface.hpp"
#include "corelib/Logger.hpp"

#include <algorithm>
#include <boost/smart_ptr.hpp>

namespace ORO_ControlKernel
{
    namespace detail
    {
    
    /**
     * @brief The BaseKernel is for internal use only.
     *
     * The template parameters must be DataObject-type containers. Ports are dropped from the kernel
     * and defined at the component level. When the component is loaded, the kernel
     * asks it to create the data objects.
     *
     * @see KernelBaseFunction
     */
    template <class CPort, class SPort, class IPort, class MPort, class OPort , class _Extension = KernelBaseFunction >
    class BaseKernel
        :   public ControlKernelInterface,
            public _Extension
    {
    public:
        typedef _Extension Extension;
            
        /**
         * @brief The Facet, which serves as a common base class for all components,
         * is defined through the Extension.
         *
         * The Kernel specifies the port type, the user the extension and Facet.
         */
        typedef ComponentBaseInterface CommonBase;

        typedef CPort CommandPortType;
        typedef OPort OutputPortType;
        typedef IPort InputPortType;
        typedef SPort SetPointPortType;
        typedef MPort ModelPortType;

        typedef BaseKernel<CommandPortType, SetPointPortType, InputPortType, ModelPortType, OutputPortType, Extension> ThisType;

        /**
         * @name Default Component Definitions
         * @{
         */
        typedef Controller< Expects<NoInput>,
                            Expects<NoModel>,
                            Expects<NoSetPoint>,
                            Writes<NoOutput>, CommonBase> DefaultController;

        typedef Generator<Expects<NoInput>,
                          Expects<NoModel>,
                          Expects<NoCommand>,
                          Writes<NoSetPoint>, CommonBase> DefaultGenerator;
        typedef Estimator<Expects<NoInput>,
                          Expects<NoOutput>,
                          Writes<NoModel>, CommonBase> DefaultEstimator;
        typedef Effector<Expects<NoInput>,
			 Expects<NoModel>,
			 Expects<NoOutput>, CommonBase> DefaultEffector;
        typedef Sensor<Writes<NoInput>, CommonBase> DefaultSensor;
        typedef SupportComponent< CommonBase > DefaultSupport;
        /**
         * @}
         */
            
        /**
         * @brief Set up the nextGen kernel.
         *
         * Optionally, specify the names of the data objects.
         *
         * @param kernel_name The name of this kernel
         */
        BaseKernel(const std::string& kernel_name,
                   const std::string& _inp_prefix,
                   const std::string& _mod_prefix,
                   const std::string& _com_prefix,
                   const std::string& _setp_prefix,
                   const std::string& _outp_prefix)
            : ControlKernelInterface( kernel_name ),
              _Extension(this),
              dummy_controller("DefaultController"),
              dummy_generator("DefaultGenerator"),
              dummy_estimator("DefaultEstimator"), dummy_effector("DefaultEffector"),
              dummy_sensor("DefaultSensor"),
              controller(&dummy_controller),
              generator(&dummy_generator),
              estimator(&dummy_estimator), effector(&dummy_effector), sensor(&dummy_sensor),

              // getKernelName() was initialised to "Default" by the KernelBaseFunction base class
              // I am thinking about loosing the prefix in the DO constructor
              // and taking the ::... part as prefix. (above, "Default" would change to "SetPoints",...
              // The first parameter is the DataObject name, or DataObjectServer name
              // in case nameserving is used. The prefix is used only by the server
              // to scope its DataObjects away from (or into !) the global namespace.
              // dObj servers sharing the prefix, can access each others dataobjects.
              // By default, the prefix equals the name ! (good since name is unique).
              inp_prefix( _inp_prefix ),
              mod_prefix( _mod_prefix ),
              com_prefix( _com_prefix ),
              setp_prefix( _setp_prefix ),
              outp_prefix( _outp_prefix )
        {
            // Load the default (empty) components.
            loadComponent(&dummy_controller);
            loadComponent(&dummy_generator);
            loadComponent(&dummy_estimator);
            loadComponent(&dummy_effector);
            loadComponent(&dummy_sensor);
            // Select the default components for execution.
            selectComponent(&dummy_controller);
            selectComponent(&dummy_generator);
            selectComponent(&dummy_estimator);
            selectComponent(&dummy_effector);
            selectComponent(&dummy_sensor);

            Logger::log() << Logger::Info << "Creating ControlKernelProcess as default Process."<<Logger::endl;
            ControlKernelProcess* ckp = new ControlKernelProcess("DefaultProcess");
            this->loadComponent(ckp);
            this->default_process = ckp;
            this->process_owner = true;

            std::vector<detail::ExtensionInterface*>::const_iterator it = this->getExtensions().begin();
            while ( it != this->getExtensions().end() ) {
                (*it)->finishConstruction();
                ++it;
            }
        }

        ~BaseKernel()
        {

            if ( this->process_owner ) {
                unload(this->default_process);
                delete this->default_process;
                this->default_process = 0;
                this->process_owner = false;
            }

            // If kernel destroyed and components present : BAD !
            // User did not cleanup.
            // do not touch the components, they might also be destructed
            // in the meantime. only cleanup internal structures.
            while ( ! this->components.empty() ) {
                delete this->components.begin()->second;
                this->components.erase( this->components.begin() );
            }
        }

        virtual bool isSelected( const std::string& name ) const
        {
            KernelBaseFunction::ComponentMap::const_iterator it = this->components.find( name );
            if ( it != this->components.end() ) {
                ComponentBaseInterface* c = it->second->component();
                return c == sensor || c == estimator || c == generator || c == controller || c == effector;
            }
            return false;
        }

        virtual bool initialize() 
        { 
            return Extension::initialize();
        }

        virtual void step() 
        {
            Extension::step();
        }

        virtual void finalize() 
        {
            Extension::finalize();
        }

        /**
         * @name Name Based Select methods
         * @{ */

        /**
         * @brief Select a previously loaded Component.
         *
         * This will only succeed if isLoaded(\a c) and
         * this->isActive(). Furthermore, if the Components 
         * componentStartup() method returns false, the previous
         * selected component is again started.
         *
         */
        virtual bool selectComponent( const std::string& name ) {
            KernelBaseFunction::ComponentMap::iterator it = this->components.find( name );
            if ( it != this->Extension::components.end() )
                return it->second->select();
            return false;
        }

        /*
        virtual bool selectSupport( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->supports.getObject( name )) )
                return selectSupport(c);
            return false;
            }*/
        /**
         * @}
         */

        /**
         * @name Name Based Start methods
         * @{ */

        /**
         * @brief Start a previously loaded Component.
         *
         * This will only succeed if isLoaded(\a c).
         *
         */
        virtual bool startComponent( const std::string& name ) {
            KernelBaseFunction::ComponentMap::iterator it = this->components.find( name );
            if ( it != this->Extension::components.end() )
                return startup( it->second->component() );
            return false;
        }

        /**
         * Equivalent to bool startup( ComponentBaseInterface* c ).
         */
        virtual bool startComponent( ComponentBaseInterface* c ) {
            return startup( c );
        }

        /**
         * @}
         */

        /**
         * @name Name Based Stop methods
         * @{ */

        /**
         * @brief Stop a previously loaded Component.
         *
         * This will only succeed if isLoaded(\a c).
         *
         */
        virtual bool stopComponent( const std::string& name ) {
            KernelBaseFunction::ComponentMap::iterator it = this->components.find( name );
            if ( it != this->Extension::components.end() )
                return shutdown( it->second->component() );
            return false;
        }

        /**
         * Equivalent to bool shutdown( ComponentBaseInterface* c ).
         */
        virtual bool stopComponent( ComponentBaseInterface* c ) {
            return shutdown( c );
        }

        /**
         * @}
         */


        /**
         * @name Name Based Load Query
         * @{ */

        /**
         * Query if a Component is loaded in the kernel.
         *
         * @param  name The name of the Component to query.
         * @return True if the Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoaded( const std::string& name ) const {
            KernelBaseFunction::ComponentMap::const_iterator it = this->components.find( name );
            return it != this->Extension::components.end();
        }
        /**
         * @}
         */

        /**
         * @name Component Loaded Queries
         * @{ */

        /**
         * @brief Query if a Component is loaded in the kernel.
         * @param c The component
         * @return True if it is loaded.
         */
        bool isLoaded( ComponentBaseInterface* c) const {
            KernelBaseFunction::ComponentMap::const_iterator it = this->components.find( c->getName() );
            return it != this->Extension::components.end();
        }
        /**
         * @}
         */

        /**
         * @name General Component State Inspection Methods
         * @(
         */

        /**
         * @brief Check if a component is loaded or started.
         *
         * There is no difference with the full \a isLoadedX() and
         * \a isSelectedX() methods.
         */
        ComponentBaseInterface* findComponent( const std::string& name ) const
        {
            typename KernelBaseFunction::ComponentMap::const_iterator it = this->Extension::components.find( name );
            if ( it != this->Extension::components.end() )
                return it->second->component();
            return 0;
        }

#if 0
        bool isLoaded( const std::string& name) const {
            ComponentBaseInterface* c = findComponent( name );
            if (c)
                return isLoaded(c);
            else
                return false;
        }

        bool isLoaded( ComponentBaseInterface* c) const {
            return isLoadedSensor(c) || isLoadedEstimator(c)
                || isLoadedGenerator(c) || isLoadedController(c) || isLoadedEffector(c);
        }
#endif

        bool isStarted( const std::string& name ) const {
            ComponentBaseInterface* c = findComponent( name );
            if (c)
                return c->isSelected();
            else
                return false;
        }

        bool isStarted( ComponentBaseInterface* c) const {
            return c->isSelected();
        }
        /**
         * @}
         */

        /**
         * @name Select a Data Flow Component
         * @{
         */

        /**
         * @brief Select a previously loaded Component.
         *
         * Selecting a Component enables it to write to its
         * Data Objects. A Component may expect meaningfull
         * values in the Data Objects it reads from and is
         * expected to write a meaningfull value in the 
         * Data Objects it writes to.
         *
         * This will only succeed if isLoadedComponent(\a c).
         * Furthermore, if the Component's
         * componentStartup() method returns false, the previous
         * selected component is again started.
         *
         */
        bool selectComponent(ComponentBaseInterface* c) { 
            if ( ! isLoaded(c) )
                return false;
            
            return this->selectComponent( c->getName() );
        }

        // we need these special cases for callback purposes.
        bool selectSensor(ComponentBaseInterface* c) { 
            if ( ! isLoaded(c) )
                return false;

            sensor = this->switchComponent( sensor, c );
            if (sensor == 0) {
                // In case switching completely failed : 
                this->startupComponent( &dummy_sensor );
                sensor = &dummy_sensor;
            }
            return sensor == c;
        }

        bool selectEstimator(ComponentBaseInterface* c) { 
            if ( ! isLoaded(c) )
                return false;

            estimator = this->switchComponent( estimator, c );
            if (estimator == 0) {
                // In case switching completely failed : 
                this->startupComponent( &dummy_estimator );
                estimator = &dummy_estimator;
            }
            return estimator == c;
        }

        bool selectGenerator(ComponentBaseInterface* c) { 
            if ( ! isLoaded(c) )
                return false;

            generator = this->switchComponent( generator, c );
            if (generator == 0) {
                // In case switching completely failed : 
                this->startupComponent( &dummy_generator );
                generator = &dummy_generator;
            }
            return generator == c;
        }

        bool selectController(ComponentBaseInterface* c) { 
            if ( ! isLoaded(c) )
                return false;

            controller = this->switchComponent( controller, c );
            if (controller == 0) {
                // In case switching completely failed : 
                this->startupComponent( &dummy_controller );
                controller = &dummy_controller;
            }
            return controller == c;
        }

        bool selectEffector(ComponentBaseInterface* c) { 
            if ( ! isLoaded(c) )
                return false;

            effector = this->switchComponent( effector, c );
            if (effector == 0) {
                // In case switching completely failed : 
                this->startupComponent( &dummy_effector );
                effector = &dummy_effector;
            }
            return effector == c;
        }

        /**
         * @}
         */

        /**
         * @name Generic Component Methods
         * @brief After a component is \a add() 'ed, these
         * methods can be called to change their state.
         * @{
         */

        /**
         * @brief Unload a previously added Component.
         * @param c The Component to unload.
         *
         * @return true if the component is present and could be unloaded.
         */
        bool unload( ComponentBaseInterface* c) {
            return ( this->components.count( c->getName() ) != 0) && this->components[ c->getName() ]->unload();
        }

        /**
         * @brief Reload a previously loaded Component.
         * @param c The Component to reload.
         *
         * @return true if the component is present and could be reloaded.
         */
        bool reload( ComponentBaseInterface* c) {
            return ( this->components.count(  c->getName() ) != 0) && this->components[ c->getName() ]->reload();
        }

        /**
         * @brief Shutdown a previously added Component.
         * @param c The Component to shutdown.
         * @post  The default component will be selected.
         *
         * @return true if the component is present and could be shutdowned.
         */
        bool shutdown( ComponentBaseInterface* c) {
            if (this->isLoaded( c ) && this->isStarted( c ) ) {
                this->shutdownComponent(c);
                return true;
            }
            return false;
        }

        /**
         * @brief Startup a previously added Component.
         * @param c The Component to startup.
         *
         * @return true if the component is present and could be started.
         */
        bool startup( ComponentBaseInterface* c) {
            return this->isLoaded(c) && !this->isStarted(c) && this->startupComponent( c );
        }

        /**
         * @brief Restart (shutdown + startup) a previously added Component.
         * @param c The Component to restart.
         *
         * @return true if the component is present and could be restarted.
         */
        bool restart( ComponentBaseInterface* c) {
            if ( this->isStarted(c) ) {
                this->shutdownComponent(c) ;
                return this->startupComponent(c);
            }
            return false;
        }

        /**
         * @brief Select (switch) a previously added Component.
         * Equivalent to bool selectComponent( ComponentBaseInterface* c)
         * @param c The Component to select.
         *
         * @return true if the component is present and could be selected.
         * false indicates that selection failed and that the previously selected component
         * is running again.
         */
        bool select( ComponentBaseInterface* c) {
            return this->selectComponent( c );
        }


        /** @} */

        /**
         * @name Component Load Methods
         * @{
         */

        /**
         * @brief Load a Component in the Control Kernel.
         *
         * When a Component is loaded, it is initialised, registered
         * to all the Kernel Extensions and available for selection
         * when the kernel is started.
         */
        template< class _Component>
        bool loadComponent(_Component* c) {
            if ( this->isRunning() ) {
                Logger::log() << Logger::Error << "Tried to load Component " << c->getName() <<" in running kernel !" << Logger::endl;
                return false;
            }

            // detect user defined DefaultProcess.
            if ( c->getName() == "DefaultProcess" && this->process_owner ) {
                // if so, destroy our default, such that loadComponent can proceed.
                this->unload(this->default_process);
                delete this->default_process;
                this->default_process = 0;
                this->process_owner = false;
            }

            if ( this->findComponent( c->getName() ) ){
                Logger::log() << Logger::Error << "Tried to load Component " << c->getName() <<" twice in kernel !" << Logger::endl;
                return false;
            }
                
            c->createPorts( this );
            this->components.insert( std::make_pair( c->getName(), new ComponentC<ThisType, _Component>(this, c) ));
            if ( ! c->enableFacets(this) )
                {
                    this->components.erase( c->getName() );
                    c->erasePorts();
                    //this->removeComponent( c );
                    Logger::log() << Logger::Error << "Trying to load Component " << c->getName();
                    Logger::log() << Logger::Error << " : failed !" << Logger::endl;

                    // In case we just deleted our own DefaultProcess, re-create it:
                    if ( c->getName() == "DefaultProcess" ) {
                        ControlKernelProcess* ckp = new ControlKernelProcess("DefaultProcess");
                        this->loadComponent(ckp);
                        this->default_process = ckp;
                        this->process_owner = true;
                    }

                    return false;
                }
            else
                {
                    Logger::log() << Logger::Info << "Trying to load Component " << c->getName();
                    Logger::log() << Logger::Info << " : success !" << Logger::endl;
                    return true;
                }
        }
        /**
         * @}
         */

        /**
         * @name Component Reload Methods
         * @{
         */

        /**
         * @brief Reload a Component in the Control Kernel.
         *
         * When a Component is reloaded, all its Facets are disabled,
         * the DataObject(s) it writes recreated and then again enabled.
         * This is not equivalent to unloadSensor/loadSensor but is meant
         * to allow adding/removing DataObject instances during 
         * component configuration.
         *
         * A reload can fail. Failure means could not be loaded after
         * the unload, probably due to a misconfiguration.
         *
         * The reload methods are only here for
         * a more complex range of Components such as the GenericSensor
         * and AxisSensor, which get loaded, reconfigured and must be
         * reloaded to reflect the reconfiguration in the DataObjects.
         * Most components will never need a reload.
         *
         * Since the Effector and Support Components do not have a Write Port,
         * a reload does not cause any DataObject recreation.
         *
         * @param c The Component
         */
        template< class _Component>
        bool reloadComponent(_Component* c) {
            if ( this->isRunning() || !this->isLoaded( c ) )
                return false;

            c->disableFacets(this);
            if ( ! c->enableFacets(this) )
                {
                    KernelBaseFunction::ComponentMap::iterator it = this->Extension::components.find( c->getName() );
                    delete it->second;
                    this->components.erase( c->getName() );
                    c->erasePorts();
                    return false;
                }
            return true;
        }
        /** @} */

        /**
         * @name Component Unload Methods
         * @{
         */

        /**
         * @brief Unload a Component from the Control Kernel.
         *
         * When a Component is unloaded, it is deregistered from
         * all Extensions and disconnected from the DataObjects.
         * @param c The Component
         */
        template< class _Component>
        bool unloadComponent(_Component* c) {
            if ( this->isRunning() )
                return false;
            
            KernelBaseFunction::ComponentMap::iterator it = this->Extension::components.find( c->getName() );
            if (it != this->Extension::components.end() )
                {
                    delete it->second;
                    this->Extension::components.erase( it );
                    c->disableFacets(this);
                    c->erasePorts();
                    return true;
                }
            return false;
        }
        /**
         * @}
         */
        const std::string& getInputPrefix() const { return inp_prefix; }
        const std::string& getOutputPrefix() const { return outp_prefix; }
        const std::string& getModelPrefix() const { return mod_prefix; }
        const std::string& getSetPointPrefix() const { return setp_prefix; }
        const std::string& getCommandPrefix() const { return com_prefix; }
    protected:
        virtual void preLoad(ComponentBaseInterface* comp) {
            this->components[ comp->getName() ]->create();
        }

        //virtual void postLoad(ComponentBaseInterface* comp) {}

        //virtual void preUnload(ComponentBaseInterface* comp) { }

        virtual void postUnload(ComponentBaseInterface* ) {
            // erase semantics are wrong and delete to much
            // if multiple comps write the same DO
            //components[ comp->getName() ]->erase();
        }

        /* @{ */
        /**
         * @brief The default Components, they do not write to dataobjects.
         */
        DefaultController dummy_controller;
        DefaultGenerator dummy_generator;
        DefaultEstimator dummy_estimator;
        DefaultEffector dummy_effector;
        DefaultSensor dummy_sensor;
        /* @} */

        /* @{ */
        /**
         * @brief Pointers to the Components which are selected
         */
        ComponentBaseInterface *controller;
        ComponentBaseInterface  *generator;
        ComponentBaseInterface  *estimator;
        ComponentBaseInterface   *effector;
        ComponentBaseInterface     *sensor;
        /* @} */

        std::string inp_prefix;
        std::string mod_prefix;
        std::string com_prefix;
        std::string setp_prefix;
        std::string outp_prefix;

    };
    }
}

#endif

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
#include "DataObjectInterfaces.hpp"
#include "PortInterfaces.hpp"
#include "BaseComponents.hpp"
#include "ComponentStateInterface.hpp"

#include <algorithm>
#include <boost/smart_ptr.hpp>

namespace ORO_ControlKernel
{

    namespace detail
    {
        using boost::shared_ptr;

        namespace {
            // helper class for dataobject creation
            struct DOCreationInterface {
                virtual ~DOCreationInterface() {}
                virtual void create() = 0;
                virtual void erase() = 0;
            };
            // Write Port Interface, DataObjectType container
            template<class WPI, class DOT>
            struct DOCreator : public DOCreationInterface {
                std::string name;
                // prefix
                std::string pref;
                WPI* wpi;
                DOCreator(WPI* wp, std::string _n, std::string _p ) :  name(_n), pref(_p), wpi(wp) {}
                virtual void create() {
                    // Create the DataObject Server
                    wpi->createDataObject( name, pref, DOT() );
                }
                virtual void erase() {
                    wpi->eraseDataObject();
                }
                
            };
            struct NOPCreator : public DOCreationInterface {
                virtual void create() {}
                virtual void erase() {}
            };
        }


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
        typedef std::map<ComponentBaseInterface*, std::pair<shared_ptr<ComponentStateInterface>,
                                                            shared_ptr<DOCreationInterface> > > ComponentStateMap;
        using ControlKernelInterface::getKernelName;

    public:
        typedef _Extension Extension;
            
        /**
         * @brief The Aspect, which serves as a common base class for all components,
         * is defined through the Extension.
         *
         * The Kernel specifies the port type, the user the extension and aspect.
         */
        typedef typename Extension::CommonBase CommonBase;

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
                          Writes<NoModel>, CommonBase> DefaultEstimator;
        typedef Effector<Expects<NoOutput>, CommonBase> DefaultEffector;
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
            :
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
            loadController(&dummy_controller);
            loadGenerator(&dummy_generator);
            loadEstimator(&dummy_estimator);
            loadEffector(&dummy_effector);
            loadSensor(&dummy_sensor);
            // Select the default components for execution.
            this->running = true;  // quite ok workaround
            selectController(&dummy_controller);
            selectGenerator(&dummy_generator);
            selectEstimator(&dummy_estimator);
            selectEffector(&dummy_effector);
            selectSensor(&dummy_sensor);
            this->running = false;

            ControlKernelInterface::setKernelName( kernel_name );
        }

        ~BaseKernel()
        {
        }

        virtual bool isSelectedController( const std::string& name ) const
        {
            return this->controllers.getObject( name ) == controller;
        }

        virtual bool isSelectedGenerator( const std::string& name ) const
        {
            return this->generators.getObject( name ) == generator;
        }

        virtual bool isSelectedEstimator( const std::string& name ) const
        {
            return this->estimators.getObject( name ) == estimator;
        }

        virtual bool isSelectedSensor( const std::string& name ) const
        {
            return this->sensors.getObject( name ) == sensor;
        }

        virtual bool isSelectedEffector( const std::string& name ) const
        {
            return this->effectors.getObject( name ) == effector;
        }

        virtual bool initialize() 
        { 
            // First, startup all the support components
            NameServer<ComponentBaseInterface*>::value_iterator itl = this->supports.getValueBegin();
            for( ; itl != this->supports.getValueEnd(); ++itl)
                this->startComponent( *itl );

            if ( !Extension::initialize() )
                {
                    for( itl = this->supports.getValueBegin(); itl != this->supports.getValueEnd(); ++itl)
                        this->stopComponent( *itl );
                    return false;
                }
                
            // initial startup of all components
            this->kernelStarted.fire();

            return true;
        }

        virtual void step() 
        {
            // Check if we are in running state ( !aborted )
            if ( this->isRunning() )
                Extension::step();
            else
                KernelBaseFunction::finalize(); // select default components
        }

        virtual void finalize() 
        {
            // This is safe as long as the task is stopped from a lower
            // priority thread than this task is running in. If not,
            // it is possible that step() is still executing (preempted)
            // while the finalize() is called from within the HP stop().
            // stop() (aka taskRemove) could block on step() if step()
            // is strictly non blocking (what it should be), otherwise
            // it will lead to deadlocks.
            Extension::finalize();
            // Last, shutdown all the remaining components
            ComponentStateMap::iterator itl;
            for( itl = this->componentStates.begin(); itl != this->componentStates.end(); ++itl)
                this->shutdown( itl->first );
            NameServer<ComponentBaseInterface*>::value_iterator its = this->supports.getValueBegin();
            for( its = this->supports.getValueBegin(); its != this->supports.getValueEnd(); ++its)
                this->stopComponent( *its );

            this->kernelStopped.fire();
        }

        /**
         * @name Name Based Select methods
         * @{
         * @brief Select a previously loaded Controller Component.
         *
         * This will only succeed if isLoadedController(\a c) and
         * this->isRunning(). Furthermore, if the Controller 
         * componentStartup() method returns false, the previous
         * selected controller is again started.
         *
         */
        virtual bool selectController( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->controllers.getObjectByName( name )) )
                return selectController(c);
            return false;
        }

        virtual bool selectSensor( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->sensors.getObjectByName( name )) )
                return selectSensor(c);
            return false;
        }

        virtual bool selectGenerator( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->generators.getObjectByName( name )) )
                return selectGenerator(c);
            return false;
        }

        virtual bool selectEstimator( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->estimators.getObjectByName( name )) )
                return selectEstimator(c);
            return false;
        }

        virtual bool selectEffector( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->effectors.getObjectByName( name )) )
                return selectEffector(c);
            return false;
        }

        /*
        virtual bool selectSupport( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->supports.getObjectByName( name )) )
                return selectSupport(c);
            return false;
            }*/
        /**
         * @}
         */

        /**
         * @name Name Based Start methods
         * @{
         * @brief Start a previously loaded Controller Component.
         *
         * This will only succeed if isLoadedController(\a c).
         *
         */
        virtual bool startController( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->controllers.getObjectByName( name )) )
                return startup(c);
            return false;
        }

        virtual bool startSensor( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->sensors.getObjectByName( name )) )
                return startup(c);
            return false;
        }

        virtual bool startGenerator( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->generators.getObjectByName( name )) )
                return startup(c);
            return false;
        }

        virtual bool startEstimator( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->estimators.getObjectByName( name )) )
                return startup(c);
            return false;
        }

        virtual bool startEffector( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->effectors.getObjectByName( name )) )
                return startup(c);
            return false;
        }

        /**
         * @}
         */

        /**
         * @name Name Based Stop methods
         * @{
         * @brief Stop a previously loaded Controller Component.
         *
         * This will only succeed if isLoadedController(\a c).
         *
         */
        virtual bool stopController( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->controllers.getObjectByName( name )) )
                return shutdown(c);
            return false;
        }

        virtual bool stopSensor( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->sensors.getObjectByName( name )) )
                return shutdown(c);
            return false;
        }

        virtual bool stopGenerator( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->generators.getObjectByName( name )) )
                return shutdown(c);
            return false;
        }

        virtual bool stopEstimator( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->estimators.getObjectByName( name )) )
                return shutdown(c);
            return false;
        }

        virtual bool stopEffector( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = this->effectors.getObjectByName( name )) )
                return shutdown(c);
            return false;
        }

        /**
         * @}
         */


        /**
         * @name Name Based Load Query
         * @{
         * Query if a Component is loaded in the kernel.
         *
         * @param  name The name of the Component to query.
         * @return True if the Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoadedSensor( const std::string& name ) const {
            return this->sensors.isNameRegistered(name);
        }

        bool isLoadedEstimator( const std::string& name ) const {
            return this->estimators.isNameRegistered(name);
        }

        bool isLoadedGenerator( const std::string& name ) const {
            return this->generators.isNameRegistered(name);
        }

        bool isLoadedController( const std::string& name ) const {
            return this->controllers.isNameRegistered(name);
        }

        bool isLoadedEffector( const std::string& name ) const {
            return this->effectors.isNameRegistered(name);
        }

        bool isLoadedSupport( const std::string& name ) const {
            return this->supports.isNameRegistered( name );
        }
        /**
         * @}
         */

        /**
         * @name Component Loaded Queries
         * @{
         * @brief Query if a Component is loaded in the kernel.
         * @param c The component
         * @return True if it is loaded.
         */
        bool isLoadedController( ComponentBaseInterface* c) const {
            return this->controllers.isObjectRegistered(c);
        }

        bool isLoadedGenerator(ComponentBaseInterface* c) const{
            return this->generators.isObjectRegistered(c);
        }

        bool isLoadedEstimator(ComponentBaseInterface* c) const {
            return this->estimators.isObjectRegistered(c);
        }

        bool isLoadedSensor(ComponentBaseInterface* c) const {
            return this->sensors.isObjectRegistered(c);
        }

        bool isLoadedEffector(ComponentBaseInterface* c) const {
            return this->effectors.isObjectRegistered(c);
        }

        bool isLoadedSupport(ComponentBaseInterface* c) const {
            return this->supports.isObjectRegistered( c );
        }

        /**
         * @}
         */

        /**
         * @name General Component State Inspection Methods
         * @(
         * @brief Check if a component is loaded or started.
         *
         * There is no difference with the full \a isLoadedX() and
         * \a isSelectedX() methods.
         */
        ComponentBaseInterface* findComponent( const std::string& name ) const
        {
            typename Extension::KernelBaseFunction::ComponentMap::const_iterator it = this->components.find( name );
            if ( it != this->components.end() )
                return it->second;
            return 0;
        }

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
        bool selectSensor(ComponentBaseInterface* c) { 
            if ( ! isLoadedSensor(c) )
                return false;

            sensor = this->switchComponent( sensor, c );
            if (sensor == 0) {
                // In case switching completely failed : 
                this->startComponent( &dummy_sensor );
                sensor = &dummy_sensor;
            }
            return sensor == c;
        }

        bool selectEstimator(ComponentBaseInterface* c) { 
            if ( ! isLoadedEstimator(c) )
                return false;

            estimator = this->switchComponent( estimator, c );
            if (estimator == 0) {
                // In case switching completely failed : 
                this->startComponent( &dummy_estimator );
                estimator = &dummy_estimator;
            }
            return estimator == c;
        }

        bool selectGenerator(ComponentBaseInterface* c) { 
            if ( ! isLoadedGenerator(c) )
                return false;

            generator = this->switchComponent( generator, c );
            if (generator == 0) {
                // In case switching completely failed : 
                this->startComponent( &dummy_generator );
                generator = &dummy_generator;
            }
            return generator == c;
        }

        bool selectController(ComponentBaseInterface* c) { 
            if ( ! isLoadedController(c) )
                return false;

            controller = this->switchComponent( controller, c );
            if (controller == 0) {
                // In case switching completely failed : 
                this->startComponent( &dummy_controller );
                controller = &dummy_controller;
            }
            return controller == c;
        }

        bool selectEffector(ComponentBaseInterface* c) { 
            if ( ! isLoadedEffector(c) )
                return false;

            effector = this->switchComponent( effector, c );
            if (effector == 0) {
                // In case switching completely failed : 
                this->startComponent( &dummy_effector );
                effector = &dummy_effector;
            }
            return effector == c;
        }

        /* Not usefull, ComponentStartup has DataObject semantics.
        bool selectSupport(ComponentBaseInterface* c) { 
            if ( ! isLoadedSupport(c) )
                return false;

            return this->startComponent( c );
        }

        bool deselectSupport( ComponentBaseInterface* c) { 
            if ( ! isLoadedSupport(c) || !this->isRunning() )
                return false;

            this->stopComponent( c );
            return true;
        }
        */
        /**
         * @}
         */



        /**
         * @name Component Registration Methods
         * @brief Registering a Component allows it to change
         * it's state (load, startup,...) in the kernel.
         *
         * After the \a add method, the "Generic Component Methods" can be used.
         * After the \a remove method, the "Generic Component Methods" can no longer be used.
         *
         * @param c The Sensor to be removed.
         * @return true if it could be removed.
         * @{
         */

        template< class _Sensor>
        bool addSensor(_Sensor* c) {
            if ( componentStates.count( c ) != 0)
                return false;
            componentStates.insert( std::make_pair(c, std::make_pair(new SensorC< ThisType, _Sensor>(this, c),
                                                                new DOCreator< typename _Sensor::Input, InputPortType>(c, this->getKernelName() + "::Inputs",this->inp_prefix ) ) ) );
            return true;
        }

        template< class _Sensor>
        bool removeSensor(_Sensor* c) {
            if ( componentStates.count( c ) == 0)
                return false;
            componentStates.erase(c);
            return true;
        }

        template< class _Estimator>
        bool addEstimator(_Estimator* c) {
            if ( componentStates.count( c ) != 0)
                return false;
            componentStates.insert( std::make_pair(c, std::make_pair(new EstimatorC< ThisType, _Estimator>(this, c),
                                                                new DOCreator< typename _Estimator::Model, ModelPortType>(c,  this->getKernelName() + "::Models",this->mod_prefix ) ) ) );
            return true;
        }

        template< class _Estimator>
        bool removeEstimator(_Estimator* c) {
            if ( componentStates.count( c ) == 0)
                return false;
            componentStates.erase(c);
            return true;
        }

        template< class _Generator>
        bool addGenerator(_Generator* c) {
            if ( componentStates.count( c ) != 0)
                return false;
            componentStates.insert( std::make_pair(c, std::make_pair(new GeneratorC< ThisType, _Generator>(this, c),
                                                                new DOCreator< typename _Generator::SetPoint, SetPointPortType>(c,  this->getKernelName() + "::SetPoints",this->setp_prefix )) ));
            return true;
        }

        template< class _Generator>
        bool removeGenerator(_Generator* c) {
            if ( componentStates.count( c ) == 0)
                return false;
            componentStates.erase(c);
            return true;
        }

        template< class _Controller>
        bool addController(_Controller* c) {
            if ( componentStates.count( c ) != 0)
                return false;
            componentStates.insert( std::make_pair(c, std::make_pair(new ControllerC< ThisType, _Controller>(this, c),
                                                                new DOCreator< typename _Controller::Output, OutputPortType>(c,  this->getKernelName() + "::Outputs",this->outp_prefix )) ) );
            return true;
        }

        template< class _Controller>
        bool removeController(_Controller* c) {
            if ( componentStates.count( c ) == 0)
                return false;
            componentStates.erase(c);
            return true;
        }

        template< class _Effector>
        bool addEffector(_Effector* c) {
            if ( componentStates.count( c ) != 0)
                return false;
            componentStates.insert( std::make_pair(c, std::make_pair(new EffectorC< ThisType, _Effector>(this, c), new NOPCreator() ) ) );
            return true;
        }

        template< class _Effector>
        bool removeEffector(_Effector* c) {
            if ( componentStates.count( c ) == 0)
                return false;
            componentStates.erase(c);
            return true;
        }

        template< class _Support>
        bool addSupport(_Support* c) {
            if ( componentStates.count( c ) != 0)
                return false;
            componentStates.insert( std::make_pair(c, std::make_pair(new SupportC< ThisType, _Support>(this, c), new NOPCreator() ) ) );
            return true;
        }

        template< class _Support>
        bool removeSupport(_Support* c) {
            if ( componentStates.count( c ) == 0)
                return false;
            componentStates.erase(c);
            return true;
        }

        /** @}*/

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
        bool load( ComponentBaseInterface* c) {
            return ( componentStates.count( c ) != 0) && componentStates[c].first->load();
        }

        /**
         * @brief Unload a previously added Component.
         * @param c The Component to unload.
         *
         * @return true if the component is present and could be unloaded.
         */
        bool unload( ComponentBaseInterface* c) {
            return ( componentStates.count( c ) != 0) && componentStates[c].first->unload();
        }

        /**
         * @brief Reload a previously loaded Component.
         * @param c The Component to reload.
         *
         * @return true if the component is present and could be reloaded.
         */
        bool reload( ComponentBaseInterface* c) {
            return ( componentStates.count( c ) != 0) && componentStates[c].first->reload();
        }

        /**
         * @brief Shutdown (deselect) a previously added Component.
         * @param c The Component to shutdown.
         * @post  The default component will be selected.
         *
         * @return true if the component is present and could be shutdowned.
         */
        bool shutdown( ComponentBaseInterface* c) {
            if (this->isLoaded(c) && this->isStarted(c) ) {
                this->stopComponent(c);
                return true;
            }
            return false;
        }

        /**
         * @brief Startup (select) a previously added Component.
         * @param c The Component to startup.
         *
         * @return true if the component is present and could be started.
         */
        bool startup( ComponentBaseInterface* c) {
            return this->isLoaded(c) && !this->isStarted(c) && this->startComponent( c );
        }

        /**
         * @brief Restart (deselect + select) a previously added Component.
         * @param c The Component to restart.
         *
         * @return true if the component is present and could be restarted.
         */
        bool restart( ComponentBaseInterface* c) {
            if ( this->isStarted(c) ) {
                this->stopComponent(c) ;
                return this->startComponent(c);
            }
            return false;
        }


        /** @} */

        /**
         * @name Component Load Methods
         * @{
         * @brief Load a Component in the Control Kernel.
         *
         * When a Component is loaded, it is initialised, registered
         * to all the Kernel Extensions and available for selection
         * when the kernel is started.
         */
        template< class _Sensor>
        bool loadSensor(_Sensor* c) {
            if ( this->isRunning() )
                return false;

            this->addSensor( c );

            // Create the frontend ( dObj() )
            c->_Sensor::Input::createPort( this->getKernelName() + "::Inputs",this->inp_prefix );

            if ( ! c->enableAspect(this) )
                {
                    c->_Sensor::Input::erasePort();
                    return false;
                }
            else
                {
                    this->sensors.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template< class _Estimator>
        bool loadEstimator(_Estimator* c) {
            if ( this->isRunning() )
                return false;
            this->addEstimator(c);

            c->_Estimator::Model::createPort( this->getKernelName() + "::Models",mod_prefix );
            c->_Estimator::Input::createPort( this->getKernelName() + "::Inputs",inp_prefix );
            if ( ! c->enableAspect(this) )
                {
                    c->_Estimator::Model::erasePort();
                    c->_Estimator::Input::erasePort();
                    return false;
                }
            else
                {
                    this->estimators.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template<class _Generator >
        bool loadGenerator(_Generator* c) {
            if ( this->isRunning() )
                return false;
            this->addGenerator(c);

            c->_Generator::Command::createPort( this->getKernelName() + "::Commands", com_prefix);
            c->_Generator::Model::createPort( this->getKernelName() + "::Models",mod_prefix );
            c->_Generator::Input::createPort( this->getKernelName() + "::Inputs",inp_prefix );
            c->_Generator::SetPoint::createPort( this->getKernelName() + "::SetPoints",setp_prefix  );
            if ( ! c->enableAspect(this) )
                {
                    c->_Generator::Command::erasePort();
                    c->_Generator::SetPoint::erasePort();
                    c->_Generator::Model::erasePort();
                    c->_Generator::Input::erasePort();
                    return false;
                }
            else
                {
                    this->generators.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template< class _Controller >
        bool loadController( _Controller* c) {
            if ( this->isRunning() )
                return false;
            this->addController(c);

            c->_Controller::SetPoint::createPort( this->getKernelName() + "::SetPoints",setp_prefix );
            c->_Controller::Model::createPort( this->getKernelName() + "::Models",mod_prefix );
            c->_Controller::Input::createPort( this->getKernelName() + "::Inputs",inp_prefix );
            c->_Controller::Output::createPort( this->getKernelName() + "::Outputs", outp_prefix );
            if ( ! c->enableAspect(this) )
                {
                    c->_Controller::Output::erasePort();
                    c->_Controller::SetPoint::erasePort();
                    c->_Controller::Model::erasePort();
                    c->_Controller::Input::erasePort();
                    return false;
                }
            else
                {
                    this->controllers.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template< class _Effector >
        bool loadEffector(_Effector* c) {
            if ( this->isRunning() )
                return false;
            this->addEffector(c);

            c->_Effector::Output::createPort( this->getKernelName() + "::Outputs", outp_prefix );
            if ( ! c->enableAspect(this) )
                {
                    c->_Effector::Output::erasePort();
                    return false;
                }
            else
                {
                    this->effectors.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template< class _Support >
        bool loadSupport(_Support* c) {
            if ( this->isRunning() )
                return false;
            this->addSupport(c);

            if ( ! c->enableAspect(this) )
                {
                    return false;
                }
            else
                {
                    this->supports.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }
        /**
         * @}
         */

        /**
         * @name Component Reload Methods
         * @{
         * @brief Reload a Component in the Control Kernel.
         *
         * When a Component is reloaded, all its aspects are disabled,
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
        template< class _Sensor>
        bool reloadSensor(_Sensor* c) {
            if ( this->isRunning() || !this->sensors.isObjectRegistered( c ) )
                return false;

            c->disableAspect();
            //c->_Sensor::Input::reloadDataObject( InputPortType() );
            if ( ! c->enableAspect(this) )
                {
                    this->sensors.unregisterObject( c );
                    c->_Sensor::Input::erasePort();
                    return false;
                }
            return true;
        }

        template< class _Estimator>
        bool reloadEstimator(_Estimator* c) {
            if ( this->isRunning() || !this->estimators.isObjectRegistered( c ) )
                return false;

            c->disableAspect();
            //c->_Estimator::Model::reloadDataObject( ModelPortType() );
            if ( ! c->enableAspect(this) )
                {
                    this->estimators.unregisterObject( c );
                    c->_Estimator::Input::erasePort();
                    c->_Estimator::Model::erasePort();
                    return false;
                }
            return true;
        }

        template< class _Generator>
        bool reloadGenerator(_Generator* c) {
            if ( this->isRunning() || !this->generators.isObjectRegistered( c ) )
                return false;

            c->disableAspect();
            //c->_Generator::SetPoint::reloadDataObject( SetPointPortType() );
            if ( ! c->enableAspect(this) )
                {
                    this->generators.unregisterObject( c );
                    c->_Generator::Input::erasePort();
                    c->_Generator::Model::erasePort();
                    c->_Generator::Command::erasePort();
                    c->_Generator::SetPoint::erasePort();
                    return false;
                }
            return true;
        }

        template< class _Controller>
        bool reloadController(_Controller* c) {
            if ( this->isRunning() || !this->controllers.isObjectRegistered( c ) )
                return false;

            c->disableAspect();
            //c->_Controller::Output::reloadDataObject( OutputPortType() );
            if ( ! c->enableAspect(this) )
                {
                    this->controllers.unregisterObject( c );
                    c->_Controller::Input::erasePort();
                    c->_Controller::Model::erasePort();
                    c->_Controller::Output::erasePort();
                    return false;
                }
            return true;
        }

        template< class _Effector>
        bool reloadEffector(_Effector* c) {
            if ( this->isRunning() || !this->effectors.isObjectRegistered( c ) )
                return false;

            c->disableAspect();
            if ( ! c->enableAspect(this) )
                {
                    this->effectors.unregisterObject( c );
                    return false;
                }
            return true;
        }

        template< class _Support>
        bool reloadSupport(_Support* c) {
            if ( this->isRunning() || !this->supports.isObjectRegistered( c ) )
                return false;

            c->disableAspect();
            if ( ! c->enableAspect(this) )
                {
                    this->supports.unregisterObject( c );
                    return false;
                }
            return true;
        }

        /** @} */

        /**
         * @name Component Unload Methods
         * @{
         * @brief Unload a Component from the Control Kernel.
         *
         * When a Component is unloaded, it is deregistered from
         * all Extensions and disconnected from the DataObjects.
         * @param c The Component
         */
        template< class _Sensor>
        bool unloadSensor(_Sensor* c) {
            if ( this->isRunning() )
                return false;
            if ( this->sensors.isObjectRegistered( c ) )
                {
                    this->sensors.unregisterObject( c );
                    c->disableAspect();
                    c->_Sensor::Input::erasePort();
                    return true;
                }
            return false;
        }

        template< class _Estimator>
        bool unloadEstimator(_Estimator* c) {
            if ( this->isRunning() )
                return false;
            if ( this->estimators.isObjectRegistered(c) )
                {
                    this->estimators.unregisterObject( c );
                    c->_Estimator::Model::erasePort();
                    c->_Estimator::Input::erasePort();
                    return true;
                }
            return false;
        }

        template< class _Generator >
        bool unloadGenerator(_Generator* c) {
            if ( this->isRunning() )
                return false;
            if (  this->generators.isObjectRegistered( c ) )
                {
                    this->generators.unregisterObject( c );
                    c->disableAspect();
                    c->_Generator::Command::erasePort();
                    c->_Generator::SetPoint::erasePort();
                    c->_Generator::Model::erasePort();
                    c->_Generator::Input::erasePort();
                    return true;
                }
            return false;
        }

        template< class _Controller >
        bool unloadController(_Controller* c) {
            if ( this->isRunning() )
                return false;
            if ( this->controllers.isObjectRegistered( c ) )
                {
                    this->controllers.unregisterObject( c );
                    c->disableAspect();
                    c->_Controller::Output::erasePort();
                    c->_Controller::SetPoint::erasePort();
                    c->_Controller::Model::erasePort();
                    c->_Controller::Input::erasePort();
                    return true;
                }
            return false;
        }

        template< class _Effector >
        bool unloadEffector(_Effector* c) {
            if ( this->isRunning() )
                return false;
            if ( this->effectors.isObjectRegistered( c ) )
                {
                    this->effectors.unregisterObject( c ); 
                    c->disableAspect();
                    c->_Effector::Output::erasePort();
                    return true;
                }
            return false;
        }

        template<class _Support>
        bool unloadSupport(_Support* c) {
            if ( this->isRunning() )
                return false;
            if ( this->supports.isObjectRegistered( c ) )
                {
                    this->supports.unregisterObject( c );
                    c->disableAspect();
                    return true;
                }
            return false;
        }
        /**
         * @}
         */
    protected:
        virtual void preLoad(ComponentBaseInterface* comp) {
            componentStates[comp].second->create();
        }

        //virtual void postLoad(ComponentBaseInterface* comp) {}

        //virtual void preUnload(ComponentBaseInterface* comp) { }

        virtual void postUnload(ComponentBaseInterface* comp) {
            // erase semantics are wrong and delete to much
            // if multiple comps write the same DO
            //componentStates[comp].second->erase();
        }

        /**
         * @{
         * The default Components, they do not write to dataobjects.
         */
        DefaultController dummy_controller;
        DefaultGenerator dummy_generator;
        DefaultEstimator dummy_estimator;
        DefaultEffector dummy_effector;
        DefaultSensor dummy_sensor;
        /* @} */

        /**
         * @{
         * @brief Pointers to the Components which are selected
         */
        ComponentBaseInterface *controller;
        ComponentBaseInterface  *generator;
        ComponentBaseInterface  *estimator;
        ComponentBaseInterface   *effector;
        ComponentBaseInterface     *sensor;
        /* @} */

        /**
         * Every loaded component is placed
         * in this map.
         */
        ComponentStateMap componentStates;

        std::string inp_prefix;
        std::string mod_prefix;
        std::string com_prefix;
        std::string setp_prefix;
        std::string outp_prefix;

    };
    }
}

#endif

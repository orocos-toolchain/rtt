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

#include <algorithm>

namespace ORO_ControlKernel
{

    struct NoSetPoint : public ServedTypes<> {};
    struct NoInput : public ServedTypes<> {};
    struct NoOutput : public ServedTypes<> {};
    struct NoCommand : public ServedTypes<> {};
    struct NoModel : public ServedTypes<> {};

    namespace detail
    {
    /**
     * @brief The NextGenKernel is for internal use only.
     *
     * The template parameters must be type containers. Ports are dropped from the kernel
     * and defined at the component level. When the component is loaded, the kernel
     * asks it to create the data objects. This means that loading components is now
     * an ordered (sensor to effector) operation (cfr selecting).
     */
    template <class CPort, class SPort, class IPort, class MPort, class OPort , class _Extension = KernelBaseFunction >
    class NextGenKernel
        : public _Extension
    {
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

        /**
         * @name Default Component Definitions
         * @{
         */
        typedef Controller< Expects<NoSetPoint>,
                            Expects<NoInput>,
                            Expects<NoModel>,
                            Writes<NoOutput>, CommonBase> DefaultController;

        typedef Generator<Expects<NoCommand>, Expects<NoInput>, Expects<NoModel>, Writes<NoSetPoint>, CommonBase> DefaultGenerator;
        typedef Estimator<Expects<NoInput>, Writes<NoModel>, CommonBase> DefaultEstimator;
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
        NextGenKernel(const std::string& kernel_name=std::string("Default"),
                   const std::string& _inp_prefix=std::string("Default"),
                   const std::string& _mod_prefix=std::string("Default"),
                   const std::string& _com_prefix=std::string("Default"),
                   const std::string& _setp_prefix=std::string("Default"),
                   const std::string& _outp_prefix=std::string("Default"))
            : _Extension(this),
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

            KernelBaseFunction::setKernelName( kernel_name );
        }

        virtual bool isSelectedController( const std::string& name ) const
        {
            return controllers.getObject( name ) == controller;
        }

        virtual bool isSelectedGenerator( const std::string& name ) const
        {
            return generators.getObject( name ) == generator;
        }

        virtual bool isSelectedEstimator( const std::string& name ) const
        {
            return estimators.getObject( name ) == estimator;
        }

        virtual bool isSelectedSensor( const std::string& name ) const
        {
            return sensors.getObject( name ) == sensor;
        }

        virtual bool isSelectedEffector( const std::string& name ) const
        {
            return effectors.getObject( name ) == effector;
        }

        virtual bool initialize() 
        { 
            // First, startup all the support components
            std::for_each(supports.getValueBegin(), supports.getValueEnd(),
                          std::mem_fun( &DefaultSupport::componentStartup ));

            if ( !Extension::initialize() )
                {
                    std::for_each(supports.getValueBegin(), supports.getValueEnd(),
                                  std::mem_fun( &DefaultSupport::componentShutdown ));
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
            // Last, shutdown all the support components
            std::for_each(supports.getValueBegin(), supports.getValueEnd(),
                          std::mem_fun( &DefaultSupport::componentShutdown ));
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
            if ( (c = controllers.getObjectByName( name )) )
                return selectController(c);
            return false;
        }

        virtual bool selectSensor( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = sensors.getObjectByName( name )) )
                return selectSensor(c);
            return false;
        }

        virtual bool selectGenerator( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = generators.getObjectByName( name )) )
                return selectGenerator(c);
            return false;
        }

        virtual bool selectEstimator( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = estimators.getObjectByName( name )) )
                return selectEstimator(c);
            return false;
        }

        virtual bool selectEffector( const std::string& name ) {
            ComponentBaseInterface* c;
            if ( (c = effectors.getObjectByName( name )) )
                return selectEffector(c);
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
        bool isLoadedSensor( const std::string& name ) {
            return sensors.isNameRegistered(name);
        }

        bool isLoadedEstimator( const std::string& name ) {
            return estimators.isNameRegistered(name);
        }

        bool isLoadedGenerator( const std::string& name ) {
            return generators.isNameRegistered(name);
        }

        bool isLoadedController( const std::string& name ) {
            return controllers.isNameRegistered(name);
        }

        bool isLoadedEffector( const std::string& name ) {
            return effectors.isNameRegistered(name);
        }

        bool isLoadedSupport( const std::string& name ) {
            return supports.isNameRegistered( name );
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
        bool isLoadedController( ComponentBaseInterface* c) {
            return controllers.isObjectRegistered(c);
        }

        bool isLoadedGenerator(ComponentBaseInterface* c) {
            return generators.isObjectRegistered(c);
        }

        bool isLoadedEstimator(ComponentBaseInterface* c) {
            return estimators.isObjectRegistered(c);
        }

        bool isLoadedSensor(ComponentBaseInterface* c) {
            return sensors.isObjectRegistered(c);
        }

        bool isLoadedEffector(ComponentBaseInterface* c) {
            return effectors.isObjectRegistered(c);
        }

        bool isLoadedSupport(ComponentBaseInterface* c) {
            return supports.isObjectRegistered( c );
        }

        /**
         * @}
         */

        /**
         * @name Select a Component
         * @{
         * @brief Select a previously loaded Component.
         *
         * This will only succeed if isLoadedComponent(\a c) and
         * this->isRunning(). Furthermore, if the Component's
         * componentStartup() method returns false, the previous
         * selected component is again started.
         *
         */
        bool selectSensor(ComponentBaseInterface* c) { 
            if ( ! isLoadedSensor(c) || !this->isRunning() )
                return false;

            sensor = this->switchComponent( sensor, c );
            return sensor == c;
        }

        bool selectEstimator(ComponentBaseInterface* c) { 
            if ( ! isLoadedEstimator(c) || !this->isRunning() )
                return false;

            estimator = this->switchComponent( estimator, c );
            return estimator == c;
        }

        bool selectGenerator(ComponentBaseInterface* c) { 
            if ( ! isLoadedGenerator(c) || !this->isRunning() )
                return false;

            generator = this->switchComponent( generator, c );
            return generator == c;
        }

        bool selectController(ComponentBaseInterface* c) { 
            if ( ! isLoadedController(c) || !this->isRunning() )
                return false;

            controller = this->switchComponent( controller, c );
            return controller == c;
        }

        bool selectEffector(ComponentBaseInterface* c) { 
            if ( ! isLoadedEffector(c) || !this->isRunning() )
                return false;

            effector = this->switchComponent( effector, c );
            return effector == c;
        }
        /**
         * @}
         */

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
            c->_Sensor::Input::createPort( KernelBaseFunction::getKernelName() + "::Inputs",inp_prefix, InputPortType() );
            if ( ! c->enableAspect(this) )
                {
                    c->_Sensor::Input::erasePort();
                    return false;
                }
            else
                {
                    sensors.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template< class _Estimator>
        bool loadEstimator(_Estimator* c) {
            if ( this->isRunning() )
                return false;
            c->_Estimator::Model::createPort( KernelBaseFunction::getKernelName() + "::Models",mod_prefix, ModelPortType() );
            c->_Estimator::Input::createPort( KernelBaseFunction::getKernelName() + "::Inputs",inp_prefix );
            if ( ! c->enableAspect(this) )
                {
                    c->_Estimator::Model::erasePort();
                    c->_Estimator::Input::erasePort();
                    return false;
                }
            else
                {
                    estimators.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template<class _Generator >
        bool loadGenerator(_Generator* c) {
            if ( this->isRunning() )
                return false;
            c->_Generator::Command::createPort( KernelBaseFunction::getKernelName() + "::Commands", outp_prefix);
            c->_Generator::SetPoint::createPort( KernelBaseFunction::getKernelName() + "::SetPoints",setp_prefix, SetPointPortType()  );
            c->_Generator::Model::createPort( KernelBaseFunction::getKernelName() + "::Models",mod_prefix );
            c->_Generator::Input::createPort( KernelBaseFunction::getKernelName() + "::Inputs",inp_prefix );
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
                    generators.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template< class _Controller >
        bool loadController( _Controller* c) {
            if ( this->isRunning() )
                return false;

            c->_Controller::Output::createPort( KernelBaseFunction::getKernelName() + "::Outputs", outp_prefix, OutputPortType() );
            c->_Controller::SetPoint::createPort( KernelBaseFunction::getKernelName() + "::SetPoints",setp_prefix );
            c->_Controller::Model::createPort( KernelBaseFunction::getKernelName() + "::Models",mod_prefix );
            c->_Controller::Input::createPort( KernelBaseFunction::getKernelName() + "::Inputs",inp_prefix );

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
                    controllers.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template< class _Effector >
        bool loadEffector(_Effector* c) {
            if ( this->isRunning() )
                return false;
            c->_Effector::Output::createPort( KernelBaseFunction::getKernelName() + "::Outputs", outp_prefix );
            if ( ! c->enableAspect(this) )
                {
                    c->_Effector::Output::erasePort();
                    return false;
                }
            else
                {
                    effectors.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        template< class _Support >
        bool loadSupport(_Support* c) {
            if ( this->isRunning() )
                return false;
            if ( ! c->enableAspect(this) )
                {
                    return false;
                }
            else
                {
                    supports.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }
        /**
         * @}
         */

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
            if ( sensors.isObjectRegistered( c ) )
                {
                    sensors.unregisterObject( c );
                    c->disableAspect();
                    c->_Sensor::Input::erasePort();
                    return true;
                }
            return false;
        }

        template< class _Estimator>
        bool unloadEstimator(DefaultEstimator* c) {
            if ( this->isRunning() )
                return false;
            if ( estimators.isObjectRegistered(c) )
                {
                    estimators.unregisterObject( c );
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
            if (  generators.isObjectRegistered( c ) )
                {
                    generators.unregisterObject( c );
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
            if ( controllers.isObjectRegistered( c ) )
                {
                    controllers.unregisterObject( c );
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
            if ( effectors.isObjectRegistered( c ) )
                {
                    effectors.unregisterObject( c ); 
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
            if ( supports.isObjectRegistered( c ) )
                {
                    supports.unregisterObject( c );
                    c->disableAspect();
                    return true;
                }
            return false;
        }
        /**
         * @}
         */
    protected:

        /**
         * @{
         * The default Components, They write defaults to the DataObjects.
         */
        DefaultController dummy_controller;
        DefaultGenerator dummy_generator;
        DefaultEstimator dummy_estimator;
        DefaultEffector dummy_effector;
        DefaultSensor dummy_sensor;
        /* @} */

        /**
         * @{
         * Pointers to the Component we will actually use.
         */
        ComponentBaseInterface *controller;
        ComponentBaseInterface  *generator;
        ComponentBaseInterface  *estimator;
        ComponentBaseInterface   *effector;
        ComponentBaseInterface     *sensor;
        /* @} */

        /**
         * @{
         * @brief Nameserved components
         *
         */
        NameServer<ComponentBaseInterface* > controllers;
        NameServer<ComponentBaseInterface*>  generators;
        NameServer<ComponentBaseInterface*>   effectors;
        NameServer<ComponentBaseInterface*>  estimators;
        NameServer<ComponentBaseInterface*>     sensors;
        NameServer<ComponentBaseInterface*>    supports;
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

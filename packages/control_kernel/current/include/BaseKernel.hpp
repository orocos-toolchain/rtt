/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  BaseKernel.hpp 

                        BaseKernel.hpp -  description
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
 
 
#ifndef BASE_KERNEL_HPP
#define BASE_KERNEL_HPP

#include "KernelInterfaces.hpp"
#include "DataObjectInterfaces.hpp"
#include "BaseComponents.hpp"

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif

#include <algorithm>

namespace ORO_ControlKernel
{

    namespace detail
    {
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
        using namespace ORO_Execution;
#endif
    /**
     * @brief The BaseKernel is for internal use only.
     *
     * It is the base class for all kinds of kernels which have
     * all 5 data objects of the pattern for control. The aim is to
     * provide the kernel developer with the most common functions
     * which each specialised kernel will need. 
     *
     */
    template <class _CommandPort, class _SetPointPort, class _InputPort, class _ModelPort, class _OutputPort, class _Extension = KernelBaseFunction>
    class BaseKernel
        : public _Extension
    {
    public:
        /**
         * @defgroup data_obj The Data Object Types
         * @{
         */
        typedef typename _CommandPort::DataObjectType CommandData;
        typedef typename _SetPointPort::DataObjectType SetPointData;
        typedef typename _InputPort::DataObjectType InputData;
        typedef typename _ModelPort::DataObjectType ModelData;
        typedef typename _OutputPort::DataObjectType OutputData;
        typedef _Extension Extension;
        /**
         * @}
         */
            
        /**
         * @brief The Aspect, which serves as a common base class for all components,
         * is defined through the Extension.
         *
         * The Kernel specifies the port type, the user the extension and aspect.
         */
        typedef typename Extension::CommonBase CommonBase;

        /**
         * @defgroup data_types The Data Types for each DataObject
         * Determine here the different DataObject kinds of this kernel.
         * @{
         */
        typedef typename SetPointData::DataType SetPointType;
        typedef typename CommandData::DataType  CommandType;
        typedef typename InputData::DataType    InputType;
        typedef typename ModelData::DataType    ModelType;
        typedef typename OutputData::DataType   OutputType;
        /**
         * @}
         */

        /**
         * @defgroup def_comp Default Component Definitions
         * @{
         */
        typedef Controller<_SetPointPort, _InputPort, _ModelPort, _OutputPort, CommonBase> DefaultController;
        typedef Generator<_CommandPort, _InputPort, _ModelPort, _SetPointPort, CommonBase> DefaultGenerator;
        typedef Estimator<_InputPort, _ModelPort, CommonBase> DefaultEstimator;
        typedef Effector<_OutputPort, CommonBase> DefaultEffector;
        typedef Sensor<_InputPort, CommonBase> DefaultSensor;
        typedef SupportComponent< CommonBase > DefaultSupport;
        /**
         * @}
         */
            
        /**
         * @brief Set up the base kernel.
         *
         * Optionally, specify the names of the data objects.
         */
        BaseKernel(const std::string& inp_name=std::string("Inputs"),
                   const std::string& mod_name=std::string("Models"),
                   const std::string& com_name=std::string("Commands"),
                   const std::string& setp_name=std::string("SetPoints"),
                   const std::string& out_name=std::string("Outputs") )
            : _Extension(this),
              controller(&dummy_controller), generator(&dummy_generator),
              estimator(&dummy_estimator), effector(&dummy_effector), sensor(&dummy_sensor),

              local_setpoints(setp_name), local_commands(com_name),
              local_inputs(inp_name), local_models(mod_name), local_outputs(out_name),

              setpoints(&local_setpoints), commands(&local_commands),
              inputs(&local_inputs), models(&local_models), outputs(&local_outputs),

              startup(false), externalInputs(false), externalOutputs(false),
              externalModels(false), externalSetPoints(false), externalCommands(false)
        {
            // Load the default (empty) components.
            loadController(controller);
            loadGenerator(generator);
            loadEstimator(estimator);
            loadEffector(effector);
            loadSensor(sensor);
            // Select the default components for execution.
            selectController(controller);
            selectGenerator(generator);
            selectEstimator(estimator);
            selectEffector(effector);
            selectSensor(sensor);
        }

#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
        typedef BaseKernel<_CommandPort,_SetPointPort, _InputPort, _ModelPort,_OutputPort,Extension > ThisType;

        bool isSelectedController( const std::string& name )
        {
            return ThisType::DefaultController::nameserver.getObject( name ) == controller;
        }

        bool isSelectedGenerator( const std::string& name )
        {
            return ThisType::DefaultGenerator::nameserver.getObject( name ) == generator;
        }

        bool isSelectedEstimator( const std::string& name )
        {
            return ThisType::DefaultEstimator::nameserver.getObject( name ) == estimator;
        }

        bool isSelectedSensor( const std::string& name )
        {
            return ThisType::DefaultSensor::nameserver.getObject( name ) == sensor;
        }

        bool isSelectedEffector( const std::string& name )
        {
            return ThisType::DefaultEffector::nameserver.getObject( name ) == effector;
        }

        virtual CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< ThisType  >* ret =
                newCommandFactory( this );
            ret->add( "selectController", 
                      command<ThisType, bool,  const std::string&>( &ThisType::selectController ,
                               &ThisType::isSelectedController,
                               "Select a Controller Component", "Name", "The name of the Controller" ) );
            ret->add( "selectGenerator", 
                      command<ThisType, bool,  const std::string&>( &ThisType::selectGenerator ,
                               &ThisType::isSelectedGenerator,
                               "Select a Generator Component", "Name", "The name of the Generator" ) );
            ret->add( "selectEstimator", 
                      command<ThisType, bool,  const std::string&>( &ThisType::selectEstimator ,
                               &ThisType::isSelectedEstimator,
                               "Select a Estimator Component", "Name", "The name of the Estimator" ) );
            ret->add( "selectSensor", 
                      command<ThisType, bool,  const std::string&>( &ThisType::selectSensor ,
                               &ThisType::isSelectedSensor,
                               "Select a Sensor Component", "Name", "The name of the Sensor" ) );
            ret->add( "selectEffector", 
                      command<ThisType, bool,  const std::string&>( &ThisType::selectEffector ,
                               &ThisType::isSelectedEffector,
                               "Select a Effector Component", "Name", "The name of the Effector" ) );
            return ret;
        }

        virtual DataSourceFactory* createDataSourceFactory()
        {
            TemplateDataSourceFactory< ThisType >* ret =
                newDataSourceFactory( this );
            ret->add( "usingGenerator", 
                      data( &ThisType::isSelectedGenerator, "Check if this generator is used.",
                            "Name", "The name of the Generator") );
            ret->add( "usingController", 
                      data( &ThisType::isSelectedController, "Check if this controller is used.",
                            "Name", "The name of the Controller") );
            ret->add( "usingEstimator", 
                      data( &ThisType::isSelectedEstimator, "Check if this estimator is used.",
                            "Name", "The name of the Estimator") );
            ret->add( "usingEffector", 
                      data( &ThisType::isSelectedEffector, "Check if this effector is used.",
                            "Name", "The name of the Effector") );
            ret->add( "usingSensor", 
                      data( &ThisType::isSelectedSensor, "Check if this sensor is used.",
                            "Name", "The name of the Sensor") );
            return ret;
       }
#endif

        virtual bool initialize() 
        { 
            if ( !Extension::initialize() )
                return false;
                
            // initial startup of all components
            kernelStarted.fire();
            
            startup = true;

            return true;
        }

        virtual void step() 
        {
            // Check if we are in running state ( !aborted )
            if ( isRunning() )
                {
                    if (startup)
                        {
                            // First, startup all the support components
                            std::for_each(supports.begin(), supports.end(),
                                    std::mem_fun( &DefaultSupport::componentStartup ));
                            // This startup sequence starts up all components
                            // and shuts them down again if one failed.
                            // sx = true : ok ; sx = false : failure
                            bool s1 = false, s2 = false, s3 = false, s4 = false ,s5 = false;
                            if ( (s1 = sensor->componentStartup()) )
                                if ( (s2 = estimator->componentStartup()) )
                                    if ( (s3 = generator->componentStartup()) )
                                        if ( (s4 = controller->componentStartup()) )
                                            s5 = effector->componentStartup();

                            startup = !(s1 && s2 && s3 && s4 && s5); // startup=false if all starts were successful

                            // On failure startup == true, shutdown started components
                            if (startup && s1)
                                sensor->componentShutdown();
                            if (startup && s2)
                                estimator->componentShutdown();
                            if (startup && s3)
                                generator->componentShutdown();
                            if (startup && s4)
                                controller->componentShutdown();
                            if (startup && s5)
                                effector->componentShutdown();
                            if (startup)
                                {
                                    abortKernel(); // if we are still in startup phase, abort the kernel.
                                    return;        // abort the step() loop.
                                }
                        }
                            
                    // Call the extension (eg : reporting, execution engine, command interpreter... )
                    Extension::step();
                }
            else
                {
                    // Aborted :
                    // safe stop in abort() after a successful startup
                    if (!startup)
                        {
                            startup = true;
                            sensor->componentShutdown();
                            estimator->componentShutdown();
                            generator->componentShutdown();
                            controller->componentShutdown();
                            effector->componentShutdown();
                            // Last, shutdown all the support components
                            std::for_each(supports.begin(), supports.end(),
                                    std::mem_fun( &DefaultSupport::componentShutdown ));
                        }
                }
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
            if (!startup)
                {
                    startup = true;
                    sensor->componentShutdown();
                    estimator->componentShutdown();
                    generator->componentShutdown();
                    controller->componentShutdown();
                    effector->componentShutdown();
                }
            kernelStopped.fire();
            Extension::finalize();
        }
            
        /**
         * @brief This method is for updating the properties of this kernel.
         *
         * Each application kernel will have different properties here.
         *
         * @param bag A PropertyBag containing the properties of this kernel.
         * @return true if a valid bag was given.
         *
         * @see KernelConfig.hpp
         */
        virtual bool updateKernelProperties( const PropertyBag& bag )
        {
            return KernelBaseFunction::updateProperties(bag);
        }

        /**
         * @brief Load a Controller Component into the kernel.
         *
         * @param  name The name of the Controller Component.
         * @return True if the Controller Component could be found and loaded,
         *         False otherwise.
         */
        bool loadController( const std::string& name ) {
            DefaultController* c;
            if ( (c = DefaultController::nameserver.getObjectByName( name )) )
                return loadController(c);
            return false;
        }

        /**
         * UnLoad a Controller Component from the kernel.
         *
         * @param  name The name of the Controller Component.
         * @return True if the Controller Component could be found and unloaded,
         *         False otherwise.
         */
        bool unloadController( const std::string& name ) {
            DefaultController* c;
            if ( (c = DefaultController::nameserver.getObjectByName( name )) )
                return unloadController(c);
            return false;
        }

        /**
         * Select a Controller Component from the kernel.
         *
         * @param  name The name of the Controller Component to select.
         * @return True if the Controller Component could be found and selected,
         *         False otherwise.
         */
        bool selectController( const std::string& name ) {
            DefaultController* c;
            if ( (c = DefaultController::nameserver.getObjectByName( name )) )
                return selectController(c);
            return false;
        }

        /**
         * Query if a Controller Component is loaded in the kernel.
         *
         * @param  name The name of the Controller Component to query.
         * @return True if the Controller Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoadedController( const std::string& name ) {
            DefaultController* c;
            if ( (c = DefaultController::nameserver.getObjectByName( name )) )
                return isLoadedController(c);
            return false;
        }

        bool loadController(DefaultController* c) {
            if ( isRunning() )
                return false;
            c->writeTo(outputs);
            c->readFrom(setpoints);
            c->readFrom(models);
            c->readFrom(inputs);
            if ( ! c->enableAspect(this) )
                {
                    c->disconnect(models);
                    c->disconnect(outputs);
                    c->disconnect(setpoints);
                    c->disconnect(inputs);
                    return false;
                }
            else
                {
                    controllers.push_back( c );
                    return true;
                }
        }

        bool unloadController(DefaultController* c) {
            if ( isRunning() )
                return false;
            typename std::vector<DefaultController*>::iterator itl = std::find( controllers.begin(), controllers.end(), c);
            if ( itl != controllers.end() )
                {
                    controllers.erase( c );
                    c->disableAspect();
                    c->disconnect(models);
                    c->disconnect(outputs);
                    c->disconnect(setpoints);
                    c->disconnect(inputs);
                    return true;
                }
            return false;
        }

        bool isLoadedController(DefaultController* c) {
            return ( std::find(controllers.begin(), controllers.end(), c) != controllers.end() );
        }

        bool selectController(DefaultController* c) { 
            if ( ! isLoadedController(c) )
                return false;

            if ( this->isRunning() )
                {
                    controller->componentShutdown();
                    c->componentStartup();
                }

            controller=c;
            return true;
        }
            
        /**
         * Load a Generator Component into the kernel.
         *
         * @param  name The name of the Generator Component.
         * @return True if the Generator Component could be found and loaded,
         *         False otherwise.
         */
        bool loadGenerator( const std::string& name ) {
            DefaultGenerator* c;
            if ( (c = DefaultGenerator::nameserver.getObjectByName( name )) )
                return loadGenerator(c);
            return false;
        }

        /**
         * UnLoad a Generator Component from the kernel.
         *
         * @param  name The name of the Generator Component.
         * @return True if the Generator Component could be found and unloaded,
         *         False otherwise.
         */
        bool unloadGenerator( const std::string& name ) {
            DefaultGenerator* c;
            if ( (c = DefaultGenerator::nameserver.getObjectByName( name )) )
                return unloadGenerator(c);
            return false;
        }

        /**
         * Select a Generator Component from the kernel.
         *
         * @param  name The name of the Generator Component to select.
         * @return True if the Generator Component could be found and selected,
         *         False otherwise.
         */
        bool selectGenerator( const std::string& name ) {
            DefaultGenerator* c;
            if ( (c = DefaultGenerator::nameserver.getObjectByName( name )) )
                return selectGenerator(c);
            return false;
        }

        /**
         * Query if a Generator Component is loaded in the kernel.
         *
         * @param  name The name of the Generator Component to query.
         * @return True if the Generator Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoadedGenerator( const std::string& name ) {
            DefaultGenerator* c;
            if ( (c = DefaultGenerator::nameserver.getObjectByName( name )) )
                return isLoadedGenerator(c);
            return false;
        }

        bool loadGenerator(DefaultGenerator* c) {
            if ( isRunning() )
                return false;
            c->writeTo(setpoints);
            c->readFrom(models);
            c->readFrom(inputs);
            c->readFrom(commands);
            if ( ! c->enableAspect(this) )
                {
                    c->disconnect(models);
                    c->disconnect(commands);
                    c->disconnect(setpoints);
                    c->disconnect(inputs);
                    return false;
                }
            else
                {
                    generators.push_back( c );
                    return true;
                }
        }

        bool unloadGenerator(DefaultGenerator* c) {
            if ( isRunning() )
                return false;
            typename std::vector<DefaultGenerator*>::iterator itl = std::find( generators.begin(), generators.end(), c);
            if ( itl != generators.end() )
                {
                    generators.erase( c );
                    c->disableAspect();
                    c->disconnect(models);
                    c->disconnect(setpoints);
                    c->disconnect(commands);
                    c->disconnect(inputs);
                    return true;
                }
            return false;
        }

        bool isLoadedGenerator(DefaultGenerator* c) {
            return ( std::find(generators.begin(), generators.end(), c) != generators.end() );
        }

        bool selectGenerator(DefaultGenerator* c) { 
            if ( ! isLoadedGenerator(c) )
                return false;
            if ( this->isRunning() )
                {
                    generator->componentShutdown();
                    c->componentStartup();
                }

            generator=c;
            return true;
        }

        /**
         * Load a Estimator Component into the kernel.
         *
         * @param  name The name of the Estimator Component.
         * @return True if the Estimator Component could be found and loaded,
         *         False otherwise.
         */
        bool loadEstimator( const std::string& name ) {
            DefaultEstimator* c;
            if ( (c = DefaultEstimator::nameserver.getObjectByName( name )) )
                return loadEstimator(c);
            return false;
        }

        /**
         * UnLoad a Estimator Component from the kernel.
         *
         * @param  name The name of the Estimator Component.
         * @return True if the Estimator Component could be found and unloaded,
         *         False otherwise.
         */
        bool unloadEstimator( const std::string& name ) {
            DefaultEstimator* c;
            if ( (c = DefaultEstimator::nameserver.getObjectByName( name )) )
                return unloadEstimator(c);
            return false;
        }

        /**
         * Select a Estimator Component from the kernel.
         *
         * @param  name The name of the Estimator Component to select.
         * @return True if the Estimator Component could be found and selected,
         *         False otherwise.
         */
        bool selectEstimator( const std::string& name ) {
            DefaultEstimator* c;
            if ( (c = DefaultEstimator::nameserver.getObjectByName( name )) )
                return selectEstimator(c);
            return false;
        }

        /**
         * Query if a Estimator Component is loaded in the kernel.
         *
         * @param  name The name of the Estimator Component to query.
         * @return True if the Estimator Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoadedEstimator( const std::string& name ) {
            DefaultEstimator* c;
            if ( (c = DefaultEstimator::nameserver.getObjectByName( name )) )
                return isLoadedEstimator(c);
            return false;
        }

        bool loadEstimator(DefaultEstimator* c) {
            if ( isRunning() )
                return false;
            c->writeTo(models);
            c->readFrom(inputs);
            if ( ! c->enableAspect(this) )
                {
                    c->disconnect(models);
                    c->disconnect(inputs);
                    return false;
                }
            else
                {
                    estimators.push_back( c );
                    return true;
                }
        }

        bool unloadEstimator(DefaultEstimator* c) {
            if ( isRunning() )
                return false;
            typename std::vector<DefaultEstimator*>::iterator itl = std::find( estimators.begin(), estimators.end(), c);
            if ( itl != estimators.end() )
                {
                    estimators.erase( c );
                    c->disableAspect();
                    c->disconnect(models);
                    c->disconnect(inputs);
                    return true;
                }
            return false;
        }

        bool isLoadedEstimator(DefaultEstimator* c) {
            return ( std::find(estimators.begin(), estimators.end(), c) != estimators.end() );
        }

        bool selectEstimator(DefaultEstimator* c) { 
            if ( ! isLoadedEstimator(c) )
                return false;
            if ( this->isRunning() )
                {
                    estimator->componentShutdown();
                    c->componentStartup();
                }
            estimator=c;
            return true;
        }
            
        /**
         * @brief Load a Sensor Component into the kernel.
         *
         * @param  name The name of the Sensor Component.
         * @return True if the Sensor Component could be found and loaded,
         *         False otherwise.
         */
        bool loadSensor( const std::string& name ) {
            DefaultSensor* c;
            if ( (c = DefaultSensor::nameserver.getObjectByName( name )) )
                return loadSensor(c);
            return false;
        }

        /**
         * @brief UnLoad a Sensor Component from the kernel.
         *
         * @param  name The name of the Sensor Component.
         * @return True if the Sensor Component could be found and unloaded,
         *         False otherwise.
         */
        bool unloadSensor( const std::string& name ) {
            DefaultSensor* c;
            if ( (c = DefaultSensor::nameserver.getObjectByName( name )) )
                return unloadSensor(c);
            return false;
        }

        /**
         * @brief Select a Sensor Component from the kernel.
         *
         * @param  name The name of the Sensor Component to select.
         * @return True if the Sensor Component could be found and selected,
         *         False otherwise.
         */
        bool selectSensor( const std::string& name ) {
            DefaultSensor* c;
            if ( (c = DefaultSensor::nameserver.getObjectByName( name )) )
                return selectSensor(c);
            return false;
        }

        /**
         * @brief Query if a Sensor Component is loaded in the kernel.
         *
         * @param  name The name of the Sensor Component to query.
         * @return True if the Sensor Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoadedSensor( const std::string& name ) {
            DefaultSensor* c;
            if ( (c = DefaultSensor::nameserver.getObjectByName( name )) )
                return isLoadedSensor(c);
            return false;
        }

        bool loadSensor(DefaultSensor* c) {
            if ( isRunning() )
                return false;
            c->writeTo(inputs);
            if ( ! c->enableAspect(this) )
                {
                    c->disconnect(inputs);
                    return false;
                }
            else
                {
                    sensors.push_back( c );
                    return true;
                }
        }

        bool unloadSensor(DefaultSensor* c) {
            if ( isRunning() )
                return false;
            typename std::vector<DefaultSensor*>::iterator itl = std::find( sensors.begin(), sensors.end(), c);
            if ( itl != sensors.end() )
                {
                    sensors.erase( c );
                    c->disableAspect();
                    c->disconnect(inputs);
                    return true;
                }
            return false;
        }

        bool isLoadedSensor(DefaultSensor* c) {
            return ( std::find(sensors.begin(), sensors.end(), c) != sensors.end() );
        }

        bool selectSensor(DefaultSensor* c) { 
            if ( ! isLoadedSensor(c) )
                return false;
            if ( this->isRunning() )
                {
                    sensor->componentShutdown();
                    c->componentStartup();
                }

            sensor=c;
            return true;
        }
            
        /**
         * @brief Load a Effector Component into the kernel.
         *
         * @param  name The name of the Effector Component.
         * @return True if the Effector Component could be found and loaded,
         *         False otherwise.
         */
        bool loadEffector( const std::string& name ) {
            DefaultEffector* c;
            if ( (c = DefaultEffector::nameserver.getObjectByName( name )) )
                return loadEffector(c);
            return false;
        }

        /**
         * @brief UnLoad a Effector Component from the kernel.
         *
         * @param  name The name of the Effector Component.
         * @return True if the Effector Component could be found and unloaded,
         *         False otherwise.
         */
        bool unloadEffector( const std::string& name ) {
            DefaultEffector* c;
            if ( (c = DefaultEffector::nameserver.getObjectByName( name )) )
                return unloadEffector(c);
            return false;
        }

        /**
         * @brief Select a Effector Component from the kernel.
         *
         * @param  name The name of the Effector Component to select.
         * @return True if the Effector Component could be found and selected,
         *         False otherwise.
         */
        bool selectEffector( const std::string& name ) {
            DefaultEffector* c;
            if ( (c = DefaultEffector::nameserver.getObjectByName( name )) )
                return selectEffector(c);
            return false;
        }

        /**
         * @brief Query if a Effector Component is loaded in the kernel.
         *
         * @param  name The name of the Effector Component to query.
         * @return True if the Effector Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoadedEffector( const std::string& name ) {
            DefaultEffector* c;
            if ( (c = DefaultEffector::nameserver.getObjectByName( name )) )
                return isLoadedEffector(c);
            return false;
        }

        bool loadEffector(DefaultEffector* c) {
            if ( isRunning() )
                return false;
            c->readFrom(outputs);
            if ( ! c->enableAspect(this) )
                {
                    c->disconnect(outputs);
                    return false;
                }
            else
                {
                    effectors.push_back( c );
                    return true;
                }
        }

        bool unloadEffector(DefaultEffector* c) {
            if ( isRunning() )
                return false;
            typename std::vector<DefaultEffector*>::iterator itl = std::find( effectors.begin(), effectors.end(), c);
            if ( itl != effectors.end() )
                {
                    effectors.erase( c ); 
                    c->disableAspect();
                    c->disconnect(outputs);
                    return true;
                }
            return false;
        }

        bool isLoadedEffector(DefaultEffector* c) {
            return ( std::find(effectors.begin(), effectors.end(), c) != effectors.end() );
        }

        bool selectEffector(DefaultEffector* c) { 
            if ( ! isLoadedEffector(c) )
                return false;
            if ( this->isRunning() )
                {
                    effector->componentShutdown();
                    c->componentStartup();
                }

            effector=c;
            return true;
        }

        /**
         * @brief Load a Support Component into the kernel.
         *
         * @param  name The name of the Support Component.
         * @return True if the Support Component could be found and loaded,
         *         False otherwise.
         */
        bool loadSupport( const std::string& name ) {
            DefaultSupport* c;
            if ( (c = DefaultSupport::nameserver.getObjectByName( name )) )
                return loadSupport(c);
            return false;
        }

        /**
         * @brief UnLoad a Support Component from the kernel.
         *
         * @param  name The name of the Support Component.
         * @return True if the Support Component could be found and unloaded,
         *         False otherwise.
         */
        bool unloadSupport( const std::string& name ) {
            DefaultSupport* c;
            if ( (c = DefaultSupport::nameserver.getObjectByName( name )) )
                return unloadSupport(c);
            return false;
        }

        /**
         * @brief Query if a Support Component is loaded in the kernel.
         *
         * @param  name The name of the Support Component to query.
         * @return True if the Support Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoadedSupport( const std::string& name ) {
            DefaultSupport* c;
            if ( (c = DefaultSupport::nameserver.getObjectByName( name )) )
                return isLoadedSupport(c);
            return false;
        }

        bool loadSupport(DefaultSupport* c) {
            if ( isRunning() )
                return false;
            if ( ! c->enableAspect(this) )
                {
                    return false;
                }
            else
                {
                    supports.push_back( c );
                    return true;
                }
        }

        bool unloadSupport(DefaultSupport* c) {
            if ( isRunning() )
                return false;
            typename std::vector<DefaultSupport*>::iterator itl = std::find( supports.begin(), supports.end(), c);
            if ( itl != supports.end() )
                {
                    supports.erase( c );
                    c->disableAspect();
                    return true;
                }
            return false;
        }

        bool isLoadedSupport(DefaultSupport* c) {
            return ( std::find(supports.begin(), supports.end(), c) != supports.end() );
        }

        /**
         * @brief Returns the commands DataObject for this ControlKernel.
         */
        CommandData* getCommands() { return commands; }

        /**
         * @brief Returns the setpoints DataObject for this ControlKernel.
         */
        SetPointData* getSetpoints() { return setpoints; }

        /**
         * @brief Returns the models DataObject for this ControlKernel.
         */
        ModelData* getModels() { return models; }

        /**
         * @brief Returns the inputs DataObject for this ControlKernel.
         */
        InputData* getInputs() { return inputs; }

        /**
         * @brief Returns the outputs DataObject for this ControlKernel.
         */
        OutputData* getOutputs() { return outputs; }

        /**
         * @brief Sets the commands DataObject for this ControlKernel.
         */
        void setCommands(CommandData* c) { externalCommands=true; commands = c ; }

        /**
         * @brief Sets the setpoints DataObject for this ControlKernel.
         */
        void setSetpoints(SetPointData* s) { externalSetPoints=true; setpoints = s; }

        /**
         * @brief Sets the models DataObject for this ControlKernel.
         */
        void setModels(ModelData* m) { externalModels=true; models = m; }

        /**
         * @brief Sets the inputs DataObject for this ControlKernel.
         */
        void setInputs(InputData* i) { externalInputs=true; inputs = i; }

        /**
         * @brief Sets the outputs DataObject for this ControlKernel.
         */
        void setOutputs(OutputData* o) { externalOutputs=true; outputs = o; }
    protected:

        /**
         * The default Components, They write defaults to the DataObjects.
         */
        DefaultController dummy_controller;
        DefaultGenerator dummy_generator;
        DefaultEstimator dummy_estimator;
        DefaultEffector dummy_effector;
        DefaultSensor dummy_sensor;

        /**
         * Pointers to the Component we will actually use.
         */
        DefaultController *controller;
        DefaultGenerator  *generator;
        DefaultEstimator  *estimator;
        DefaultEffector   *effector;
        DefaultSensor     *sensor;

        /**
         * @brief The local (default) Data Objects.
         *
         * These are our local instances. The user
         * can assign others to the kernel of the
         * same type.
         */
        SetPointData local_setpoints;
        CommandData  local_commands;
        InputData    local_inputs;
        ModelData    local_models;
        OutputData   local_outputs;

        /**
         * @brief The user specified Data Objects.
         *
         */
        SetPointData* setpoints;
        CommandData*  commands;
        InputData*    inputs;
        ModelData*    models;
        OutputData*   outputs;

        bool startup;

        std::vector<DefaultController*> controllers;
        std::vector<DefaultGenerator*>  generators;
        std::vector<DefaultEffector*>   effectors;
        std::vector<DefaultEstimator*>  estimators;
        std::vector<DefaultSensor*>     sensors;
        std::vector<DefaultSupport*>    supports;

        bool externalInputs;
        bool externalOutputs;
        bool externalModels;
        bool externalSetPoints;
        bool externalCommands;

    };
    }
}

#endif

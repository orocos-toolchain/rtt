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
#include "PortInterfaces.hpp"
#include "BaseComponents.hpp"

#include <algorithm>

namespace ORO_ControlKernel
{

    namespace detail
    {
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
         * The Data Object Types
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
         * The Data Types for each DataObject
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
         * Default Component Definitions
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
         *
         * @param kernel_name The name of this kernel
         */
        BaseKernel(const std::string& kernel_name=std::string("Default"),
                   const std::string& inp_prefix=std::string("Default"),
                   const std::string& mod_prefix=std::string("Default"),
                   const std::string& com_prefix=std::string("Default"),
                   const std::string& setp_prefix=std::string("Default"),
                   const std::string& out_prefix=std::string("Default"))
            : _Extension(this),
              dummy_controller("DefaultController"), dummy_generator("DefaultGenerator"),
              dummy_estimator("DefaultEstimator"), dummy_effector("DefaultEffector"),
              dummy_sensor("DefaultSensor"),
              controller(&dummy_controller), generator(&dummy_generator),
              estimator(&dummy_estimator), effector(&dummy_effector), sensor(&dummy_sensor),

              // getKernelName() was initialised to "Default" by the KernelBaseFunction base class
              // I am thinking about loosing the prefix in the DO constructor
              // and taking the ::... part as prefix. (above, "Default" would change to "SetPoints",...
              // The first parameter is the DataObject name, or DataObjectServer name
              // in case nameserving is used. The prefix is used only by the server
              // to scope its DataObjects away from (or into !) the global namespace.
              // dObj servers sharing the prefix, can access each others dataobjects.
              // By default, the prefix equals the name ! (good since name is unique).
              local_setpoints(kernel_name+"::SetPoints",setp_prefix),
              local_commands(kernel_name+"::Commands",com_prefix),
              local_inputs(kernel_name+"::Inputs",inp_prefix),
              local_models(kernel_name+"::Models",mod_prefix),
              local_outputs(kernel_name+"::Outputs",out_prefix),

              setpoints(&local_setpoints), commands(&local_commands),
              inputs(&local_inputs), models(&local_models), outputs(&local_outputs),

              externalInputs(false), externalOutputs(false),
              externalModels(false), externalSetPoints(false), externalCommands(false)
        {
            // Load the default (empty) components.
            loadController(controller);
            loadGenerator(generator);
            loadEstimator(estimator);
            loadEffector(effector);
            loadSensor(sensor);
            // Select the default components for execution.
            this->running = true;  // quite ok workaround
            selectController(controller);
            selectGenerator(generator);
            selectEstimator(estimator);
            selectEffector(effector);
            selectSensor(sensor);
            this->running = false;

            setKernelName( kernel_name );
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
            kernelStarted.fire();

            return true;
        }

        virtual void step() 
        {
            // Check if we are in running state ( !aborted )
            if ( isRunning() )
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
            kernelStopped.fire();
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
            if ( (c = controllers.getObjectByName( name )) )
                return unloadController(c);
            return false;
        }

        virtual bool selectController( const std::string& name ) {
            DefaultController* c;
            if ( (c = controllers.getObjectByName( name )) )
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
            return controllers.isNameRegistered(name);
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
                    controllers.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        bool unloadController(DefaultController* c) {
            if ( isRunning() )
                return false;
            if ( controllers.isObjectRegistered( c ) )
                {
                    controllers.unregisterObject( c );
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
            return controllers.isObjectRegistered(c);
        }

        /**
         * @brief Select a previously loaded Controller Component.
         *
         * This will only succeed if isLoadedController(\a c) and
         * this->isRunning(). Furthermore, if the Controller 
         * componentStartup() method returns false, the previous
         * selected controller is again started.
         */
        bool selectController(DefaultController* c) { 
            if ( ! isLoadedController(c) || !this->isRunning() )
                return false;

            controller->componentShutdown();
            if ( c->componentStartup() )
                controller=c;
            else
                controller->componentStartup();

            return controller == c;
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
            if ( (c = generators.getObjectByName( name )) )
                return unloadGenerator(c);
            return false;
        }

        virtual bool selectGenerator( const std::string& name ) {
            DefaultGenerator* c;
            if ( (c = generators.getObjectByName( name )) )
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
            return generators.isNameRegistered( name );
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
                    generators.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        bool unloadGenerator(DefaultGenerator* c) {
            if ( isRunning() )
                return false;
            if (  generators.isObjectRegistered( c ) )
                {
                    generators.unregisterObject( c );
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
            return generators.isObjectRegistered(c);
        }

        /**
         * @brief Select a previously loaded Generator Component.
         *
         * This will only succeed if isLoadedGenerator(\a c) and
         * this->isRunning(). Furthermore, if the Generator 
         * componentStartup() method returns false, the previous
         * selected generator is again started.
         */
        bool selectGenerator(DefaultGenerator* c) { 
            if ( ! isLoadedGenerator(c) || !this->isRunning() )
                return false;

            generator->componentShutdown();
            if ( c->componentStartup() )
                generator=c;
            else
                generator->componentStartup();

            return generator == c;
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
            if ( (c = estimators.getObjectByName( name )) )
                return unloadEstimator(c);
            return false;
        }

        virtual bool selectEstimator( const std::string& name ) {
            DefaultEstimator* c;
            if ( (c = estimators.getObjectByName( name )) )
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
            return estimators.isNameRegistered(name);
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
                    estimators.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        bool unloadEstimator(DefaultEstimator* c) {
            if ( isRunning() )
                return false;
            if ( estimators.isObjectRegistered(c) )
                {
                    estimators.unregisterObject( c );
                    c->disableAspect();
                    c->disconnect(models);
                    c->disconnect(inputs);
                    return true;
                }
            return false;
        }

        bool isLoadedEstimator(DefaultEstimator* c) {
            return estimators.isObjectRegistered( c );
        }

        /**
         * @brief Select a previously loaded Estimator Component.
         *
         * This will only succeed if isLoadedEstimator(\a c) and
         * this->isRunning(). Furthermore, if the Estimator 
         * componentStartup() method returns false, the previous
         * selected estimator is again started.
         */
        bool selectEstimator(DefaultEstimator* c) { 
            if ( ! isLoadedEstimator(c) || !this->isRunning() )
                return false;

            estimator->componentShutdown();
            if ( c->componentStartup() )
                estimator=c;
            else
                estimator->componentStartup();

            return estimator == c;
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
            if ( (c = sensors.getObjectByName( name )) )
                return unloadSensor(c);
            return false;
        }

        virtual bool selectSensor( const std::string& name ) {
            DefaultSensor* c;
            if ( (c = sensors.getObjectByName( name )) )
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
            return sensors.isNameRegistered( name );
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
                    sensors.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        bool unloadSensor(DefaultSensor* c) {
            if ( isRunning() )
                return false;
            if ( sensors.isObjectRegistered( c ) )
                {
                    sensors.unregisterObject( c );
                    c->disableAspect();
                    c->disconnect(inputs);
                    return true;
                }
            return false;
        }

        bool isLoadedSensor(DefaultSensor* c) {
            return sensors.isObjectRegistered(c);
        }

        /**
         * @brief Select a previously loaded Sensor Component.
         *
         * This will only succeed if isLoadedSensor(\a c) and
         * this->isRunning(). Furthermore, if the Sensor 
         * componentStartup() method returns false, the previous
         * selected sensor is again started.
         */
        bool selectSensor(DefaultSensor* c) { 
            if ( ! isLoadedSensor(c) || !this->isRunning() )
                return false;

            sensor->componentShutdown();
            if ( c->componentStartup() )
                sensor=c;
            else
                sensor->componentStartup();

            return sensor == c;
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
            if ( (c = effectors.getObjectByName( name )) )
                return unloadEffector(c);
            return false;
        }

        virtual bool selectEffector( const std::string& name ) {
            DefaultEffector* c;
            if ( (c = effectors.getObjectByName( name )) )
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
            return effectors.isNameRegistered( name );
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
                    effectors.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        bool unloadEffector(DefaultEffector* c) {
            if ( isRunning() )
                return false;
            if ( effectors.isObjectRegistered( c ) )
                {
                    effectors.unregisterObject( c ); 
                    c->disableAspect();
                    c->disconnect(outputs);
                    return true;
                }
            return false;
        }

        bool isLoadedEffector(DefaultEffector* c) {
            return effectors.isObjectRegistered( c );
        }

        /**
         * @brief Select a previously loaded Effector Component.
         *
         * This will only succeed if isLoadedEffector(\a c) and
         * this->isRunning(). Furthermore, if the Effector 
         * componentStartup() method returns false, the previous
         * selected effector is again started.
         */
        bool selectEffector(DefaultEffector* c) { 
            if ( ! isLoadedEffector(c) || !this->isRunning() )
                return false;

            effector->componentShutdown();
            if ( c->componentStartup() )
                effector=c;
            else
                effector->componentStartup();

            return effector == c;
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
            if ( (c = supports.getObjectByName( name )) )
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
            return supports.isNameRegistered( name );
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
                    supports.registerObject( c, c->ComponentBaseInterface::getName() );
                    return true;
                }
        }

        bool unloadSupport(DefaultSupport* c) {
            if ( isRunning() )
                return false;
            if ( supports.isObjectRegistered( c ) )
                {
                    supports.unregisterObject( c );
                    c->disableAspect();
                    return true;
                }
            return false;
        }

        bool isLoadedSupport(DefaultSupport* c) {
            return supports.isObjectRegistered( c );
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

        NameServer<DefaultController*> controllers;
        NameServer<DefaultGenerator*>  generators;
        NameServer<DefaultEffector*>   effectors;
        NameServer<DefaultEstimator*>  estimators;
        NameServer<DefaultSensor*>     sensors;
        NameServer<DefaultSupport*>    supports;

        bool externalInputs;
        bool externalOutputs;
        bool externalModels;
        bool externalSetPoints;
        bool externalCommands;

    };
    }
}

#endif

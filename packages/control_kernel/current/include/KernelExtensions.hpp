#ifndef MOTIONKERNELEXTENSIONS_HPP
#define MOTIONKERNELEXTENSIONS_HPP

#include "Kernel.hpp"

namespace ORO_ControlKernel
{

    using namespace ORO_CoreLib;

    /**
     * This 'Component' defines the basic interfaces to the Generator, Estimator 
     * and Controller Components as if they were one component. The internal
     * interactions through Setpoints and the Model are not exported. This
     * will be used in the IntegratedControlKernel together with the Integrate<>
     * class.
     */
    template <class _CommandType, class _InputType, class _OutputType, class _Aspect = DefaultAspect>
    class IntegratedControl
        : public _Aspect::Port::ReadPort<DataObjectLocked<_CommandType> >,
        public _Aspect::Port::ReadPort<DataObject<_InputType> >,
        public _Aspect::Port::WritePort<DataObject<_OutputType> >,
        public _Aspect::BaseClass
    {
        public:
            typedef _CommandType CommandType;
            typedef _InputType InputType;
            typedef _OutputType OutputType;
            typedef _Aspect Aspect;
            
            typedef typename Aspect::Port::WritePort< DataObject<OutputType> > Output;
            typedef typename Aspect::Port::ReadPort< DataObjectLocked<CommandType> > Command;
            typedef typename Aspect::Port::ReadPort< DataObject<InputType> > Input;

            using Command::readFrom;
            using Input::readFrom;

            IntegratedControl() : Aspect::BaseClass( "IntegratedControl" ) {}
            
            /**
             * Wire the internal components to the exported ports.
             */
            virtual void wire() {}
    };


    /**
     * An IntegratedControlKernel allows the switching of Generator, Controller
     * and Estimator at once, hence leaving the ModelType and SetPointType unspecified
     * and open to the designer of these components. This can be used when generator,
     * controller and estimator are tightly coupled.
     * The following classes are then available:
     * <il>
     * <li> IntegratedControlKernel::DefaultControlCore</li>
     * <li> IntegratedControlKernel::DefaultSensor</li>
     * <li> IntegratedControlKernel::DefaultEffector</li>
     * </il>
     * Where the ControlCore will have access to the Commands, the Inputs and the Outputs.
     * 
     */
    template <class _CommandType, class _InputType, class _OutputType, class _Extension = KernelBaseFunction >
    class IntegratedControlKernel
        : public _Extension
    {
        public:
            typedef _CommandType CommandType;
            typedef _InputType InputType;
            typedef _OutputType OutputType;
            typedef _Extension Extension;

            template< class _CommonBase >
            struct BaseAspect
            {
                typedef StandardPort Port;
                typedef _CommonBase BaseClass;
            };

            /**
             * This class is a dirty hack to initialize the ControlCore.
             * It is an aspect that calls the enableCore() and
             * disableCore() functions in the derived class.
             * I needed to use a member in aspect, which violates all
             * principles of good design, but it is an aspect you never want to
             * remove anyway, it's a whole part of this class, the user does not
             * notice it.
             */
            struct CoreInitializer
            {
                CoreInitializer(const std::string& name) {}

                bool enableAspect( Extension* e ) 
                {
                        kernel = e;
                        return enableCore();
                }

                virtual bool enableCore() 
                {return true;}

                virtual void disableCore() 
                {}

                void disableAspect() 
                { disableCore(); }

                Extension* kernel;
            };

            typedef CompositeAspect< typename Extension::CommonBase, CoreInitializer > CoreBase;
            
            typedef BaseAspect<typename Extension::CommonBase> CommonAspect;
            typedef BaseAspect<CoreBase>                       CoreAspect;

            // This is a black box Component.
            typedef IntegratedControl<CommandType, InputType, OutputType, CoreAspect> DefaultControlCore;
            typedef Effector<OutputType, CommonAspect> DefaultEffector;
            typedef Sensor<InputType, CommonAspect> DefaultSensor;
            
            /**
             * Set up a control kernel.
             */
            IntegratedControlKernel()
            : dummy_effector(), effector(&dummy_effector), sensor(&dummy_sensor), core(&dummy_core)
            {
               setEffector(&dummy_effector);
               setSensor(&dummy_sensor);
               setCore(&dummy_core);
            }

            bool updateKernelProperties(const PropertyBag& bag)
            {
                return true;
            }

            /**
             * You must use this template to construct the integrated components
             * you whish to use in the IntegratedControlKernel. You can consider this class as a tool
             * for constructing Generator-Estimator-Controller tripples which will form one component
             * which you can plug in or out the IntegratedControlKernel.
             */
            template< class _ModelType, class _SetPointType >
            class ControlCore
                : public DefaultControlCore
            {
                public:
                    typedef _SetPointType SetPointType;
                    typedef _ModelType ModelType;

                    // The default components.
                    typedef Controller<SetPointType, typename DefaultControlCore::InputType, ModelType, typename DefaultControlCore::OutputType, CommonAspect> DefaultController;
                    typedef Generator<typename DefaultControlCore::CommandType, typename DefaultControlCore::InputType, ModelType, SetPointType, CommonAspect> DefaultGenerator;
                    typedef Estimator<typename DefaultControlCore::InputType, ModelType, CommonAspect> DefaultEstimator;

                    // The ports
                    typedef typename DefaultControlCore::Output Output;
                    typedef typename DefaultControlCore::Input Input;
                    typedef typename DefaultControlCore::Command Command;
                    typedef typename DefaultControlCore::Aspect Aspect;
                    
                    ControlCore()
                        : controller(&dummy_controller), generator(&dummy_generator), estimator(&dummy_estimator)
                        {
                            setController(controller);
                            setGenerator(generator);
                            setEstimator(estimator);
                        }
                    virtual void update() 
                    {
                        // dispatch the update command
                        estimator->update();
                        generator->update();
                        controller->update();
                    }

                    virtual void pull() 
                    {
                        // dispatch the pull command
                        estimator->pull();
                        generator->pull();
                        controller->pull();
                    }

                    virtual void calculate() 
                    {
                        // dispatch the calculate command
                        estimator->calculate();
                        generator->calculate();
                        controller->calculate();
                    }

                    virtual void push() 
                    {
                        // dispatch the push command
                        estimator->push();
                        generator->push();
                        controller->push();
                    }

                    /**
                     * Returns the setpoints DataObject for this ControlKernel.
                     */
                    DataObject<SetPointType>* getSetpoints() { return &setpoints; }

                    /**
                     * Returns the models DataObject for this ControlKernel.
                     */
                    DataObject<ModelType>* getModels() { return &models; }

                    void setController(DefaultController* c) { 
                        controller->disableAspect();
                        controller=c;
                        controller->writeTo( Output::dObj() );
                        controller->readFrom(&setpoints);
                        controller->readFrom(&models);
                        controller->readFrom( Input::dObj() );
                    }
                    
                    void setGenerator(DefaultGenerator* c) { 
                        generator->disableAspect();
                        generator=c;
                        generator->writeTo(&setpoints);
                        generator->readFrom(&models);
                        generator->readFrom( Input::dObj() );
                        generator->readFrom( Command::dObj() );
                    }
                    
                    void setEstimator(DefaultEstimator* c) { 
                        estimator->disableAspect();
                        estimator=c;
                        estimator->writeTo(&models);
                        estimator->readFrom( Input::dObj() );
                    }

                    /**
                     * Connects all exported ports to the data 
                     * objects of the parent.
                     */
                    virtual void wire()
                    {
                        controller->writeTo( Output::dObj() );
                        controller->readFrom( Input::dObj() );
                        generator->readFrom( Input::dObj() );
                        generator->readFrom( Command::dObj() );
                        estimator->readFrom( Input::dObj() );
                    }

                    /**
                     * Does not get called, already present in base class :-(
                     * and this version is templated so not present in the virtual 
                     * function table.
                     */
                    virtual bool enableCore()
                    {
                        return (estimator->enableAspect(CoreInitializer::kernel) &&
                                generator->enableAspect(CoreInitializer::kernel) &&
                                controller->enableAspect(CoreInitializer::kernel) );
                    }

                    virtual void disableCore()
                    {
                        estimator->disableAspect();
                        generator->disableAspect();
                        controller->disableAspect();
                    }

                    DefaultController dummy_controller;
                    DefaultGenerator  dummy_generator;
                    DefaultEstimator  dummy_estimator;
                    
                protected: 
                    DataObject<SetPointType> setpoints;
                    DataObject<ModelType>    models;

                    DefaultController *controller;
                    DefaultGenerator  *generator;
                    DefaultEstimator  *estimator;
            };

            /**
             * A utility function for creating an integrated
             * component from three separate components, derived from the
             * IntegratedControlKernel::ControlCore<> classes default components.
             */
            template< class G, class E, class C>
            static DefaultControlCore* integrate(G& gen, E& est, C& contr) 
            {
                ControlCore<typename E::ModelType, typename G::SetPointType>* tmp =
                     new ControlCore<typename E::ModelType, typename G::SetPointType>();
                tmp->setGenerator(&gen);
                tmp->setEstimator(&est);
                tmp->setController(&contr);
                // XXX although they are set, since the DefaultControlCore is not yet
                // inside a kernel, its dataobjects are invalid.
                return tmp;
            }
            
            virtual bool initialize() { return Extension::initialize(); }

            virtual void step() 
            {
                // one step is one control cycle
                // The figure is a unidirectional graph
                sensor->update();
                core->update();
                effector->update();

                Extension::step();
            }

            virtual void finalize() { Extension::finalize(); }
            
            void setSensor(DefaultSensor* c) { 
                sensor->disableAspect();
                sensor=c;
                sensor->writeTo(&inputs);
                sensor->enableAspect(this);
            }
            
            void setEffector(DefaultEffector* c) { 
                effector->disableAspect();
                effector=c;
                effector->readFrom(&outputs);
                effector->enableAspect(this);
            }
            
            void setCore(DefaultControlCore* c) { 
                core->disableAspect();
                core=c;
                core->writeTo(&outputs);
                core->readFrom(&commands);
                core->readFrom(&inputs);
                core->wire();
                core->enableAspect(this);
            }
            
            /**
             * Returns the commands DataObject for this ControlKernel.
             */
            DataObjectLocked<CommandType>* getCommands() { return &commands; }

            /**
             * Returns the inputs DataObject for this ControlKernel.
             */
            DataObject<InputType>* getInputs() { return &inputs; }

            /**
             * Returns the outputs DataObject for this ControlKernel.
             */
            DataObject<OutputType>* getOutputs() { return &outputs; }

            DefaultEffector dummy_effector;
            DefaultSensor dummy_sensor;
            DefaultControlCore dummy_core;
        protected:    
            DefaultEffector   *effector;
            DefaultSensor     *sensor;
            DefaultControlCore *core;

            DataObjectLocked<CommandType>  commands;
            DataObject<InputType>    inputs;
            DataObject<OutputType>   outputs;

    };

#if 0
#warning development on hold, so outdated.
    /**
     * The CascadedControlKernel adds an additional outer loop to an inner loop,
     * which type must be given as a template parameter. Further, you have to specify
     * the CascadedControlKernel's CommandType and SetPointType. It is then possible
     * to inherit from :
     * <il>
     * <li> CascadedControlKernel::CascadedController</li>
     * <li> CascadedControlKernel::CascadedGenerator</li>
     * </il>
     * One can use the Estimator and model from the InnerLoop.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _InnerLoopType>
    class CascadedControlKernel
        : public RunnableInterface
    {
        public:
            /**
             * The type of the InnerLoop used.
             */
            typedef _InnerLoopType DefaultInnerLoop;
            /**
             * Our outputs are commands for the inner loop.
             */
            typedef typename DefaultInnerLoop::CommandType OutputType;
            /**
             * Reuse the InnerLoop model.
             */
            typedef _ModelType ModelType;
            /**
             * Reuse the InnerLoop inputs.
             */
            typedef _InputType InputType;

            typedef _CommandType CommandType;
            typedef _SetPointType SetPointType;
            
            /**
             * The CascadedController.
             */
            typedef Controller<SetPointType, InputType, ModelType, OutputType> CascadedController;
            
            /**
             * The CascadedGenerator.
             */
            typedef Generator<CommandType, InputType, ModelType, SetPointType> CascadedGenerator;
            typedef Estimator<InputType, ModelType> CascadedEstimator;
            typedef Sensor<InputType> CascadedSensor;
                                   
            /**
             * Set up a control kernel.
             */
            CascadedControlKernel()
                :controller(&dummy_controller), generator(&dummy_generator), sensor(&dummy_sensor), estimator(&dummy_estimator), innerloop(&dummy_innerloop)
            {
                // connecting the defaults.
                setInnerLoop(innerloop); 
                setController(controller);
                setGenerator(generator);
                setEstimator(estimator);
                setSensor(sensor);
            }

            virtual bool initialize() { return true; }

            virtual void step() 
            {
                // one step is one control cycle
                // The figure is a unidirectional graph
                sensor->update();
                estimator->update();
                generator->update();
                controller->update();
                innerloop->step();
            }

            virtual void finalize() {}
            
            void setController( CascadedController* c) 
            { 
                controller=c; 
                controller->writeTo( innerloop->getCommands() );
                controller->readFrom( &setpoints);
                controller->readFrom( &models );
                controller->readFrom( &inputs );
            }

            void setGenerator( CascadedGenerator* c) 
            { 
                generator=c;
                generator->writeTo( &setpoints );
                generator->readFrom( &models );
                generator->readFrom( &inputs );
                generator->readFrom( &commands );
            }
            
            void setEstimator(CascadedEstimator* c) {
                estimator=c;
                estimator->writeTo(&models);
                estimator->readFrom(&inputs);
            }
            
            void setSensor(CascadedSensor* c) {
                sensor=c;
                sensor->writeTo(&inputs);
            }

            void setInnerLoop(DefaultInnerLoop* il)
            {
                innerloop = il;
                setController(controller);
                setGenerator(generator);
            }
            

            /**
             * Returns the command object this CascadedControlKernel reads from.
             */
            DataObject<CommandType>* getCommands() { return &commands; }

            /**
             * Returns the setpoints DataObject for this ControlKernel.
             */
            DataObject<SetPointType>* getSetpoints() { return &setpoints; }

            /**
             * Returns the models DataObject for this ControlKernel.
             */
            DataObject<ModelType>* getModels() { return &models; }

            /**
             * Returns the inputs DataObject for this ControlKernel.
             */
            DataObject<InputType>* getInputs() { return &inputs; }

            /**
             * Returns the outputs DataObject for this ControlKernel.
             */
            DataObject<OutputType>* getOutputs() { return innerLoop->getCommands(); }

        protected:
            /**
             * The default Components, They write defaults to the DataObjects.
             */
            CascadedController dummy_controller;
            CascadedGenerator  dummy_generator;
            CascadedEstimator  dummy_estimator;
            CascadedSensor     dummy_sensor;
            DefaultInnerLoop   dummy_innerloop;

            /**
             * Pointers to the Component we will actually use.
             */
            CascadedController *controller;
            CascadedGenerator  *generator;
            CascadedSensor     *sensor;
            CascadedEstimator  *estimator;
            DefaultInnerLoop   *innerloop;

            /**
             * The user specified Data Objects.
             */
            DataObject<SetPointType> setpoints;
            DataObject<CommandType>  commands;
            DataObject<ModelType>    models;
            DataObject<InputType>  inputs;
    };

    // Possible other extensions :
    class SingleModelCascaded;
    class SingleSensorCascaded;
#endif
}


#endif

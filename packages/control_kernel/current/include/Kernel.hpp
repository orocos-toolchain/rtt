#ifndef MOTIONKERNEL_HPP
#define MOTIONKERNEL_HPP

#include "KernelInterfaces.hpp"
#include "DataObjectInterfaces.hpp"
#include <corelib/RunnableInterface.hpp>

namespace ORO_ControlKernel
{

    using namespace ORO_CoreLib;

    /**
     * A controller defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Aspect = DefaultAspect >
    class Controller
        : public _Aspect::Port::ReadPort< _SetPointType >,
          public _Aspect::Port::ReadPort< _ModelType >,
          public _Aspect::Port::ReadPort< _InputType >,
          public _Aspect::Port::WritePort< _OutputType >,
          public _Aspect::BaseClass,
          public NameServerRegistrator< Controller<_SetPointType, _InputType, _ModelType, _OutputType, _Aspect>* >
    {
    public:
        typedef typename _SetPointType::DataType SetPointType;
        typedef typename _InputType::DataType InputType;
        typedef typename _ModelType::DataType ModelType;
        typedef typename _OutputType::DataType OutputType;
        typedef _Aspect Aspect;

        typedef typename Aspect::Port::WritePort< OutputType > Output;
        typedef typename Aspect::Port::ReadPort< SetPointType > SetPoint;
        typedef typename Aspect::Port::ReadPort< InputType > Input;
        typedef typename Aspect::Port::ReadPort< ModelType > Model;

        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Controller() : Aspect::BaseClass( "Controller" ) {}
        Controller(const std::string& name ) 
            : NameServerRegistrator< Controller<_SetPointType, _InputType, _ModelType, _OutputType, _Aspect>* >(nameserver,name,this), Aspect::BaseClass( "Controller" )  
        {}
            

        /*
          void addController( Controller* c)
          {
          if ( inKernel() )
          {
          c.readFrom(Input::dObj);
          c.readFrom(Setpoint::dObj);
          c.readFrom(Model::dObj);
          c.writeTo (Output::dObj);
          }
          plugins.push_back(c);
          }

          void readFrom( const InputType* _data)
          {
          for (it
            

          void removeController( Controller* c)
          {
          vector<Controller*>::iterator it = find(plugins.begin(), plugins.end(), c);
          if (it != plugins.end() )
          plugins.erase(it);
          }*/
            
        /**
         * Method Overloading is not done across scopes.
         * These lines introduce the methods anyway in this class.
         */
        using Input::readFrom;
        using Model::readFrom;
        using SetPoint::readFrom;
        using Input::disconnect;
        using Model::disconnect;
        using SetPoint::disconnect;
        using Output::disconnect;
            
            
        /**
         * The Controller nameserver.
         */
        static NameServer< Controller<_SetPointType, _InputType, _ModelType, _OutputType, _Aspect>* > nameserver;
            
    };
    
    template <class S, class I, class M, class O, class A >
    NameServer<Controller<S,I,M,O,A>*> Controller<S,I,M,O,A>::nameserver;

    /**
     * A generator defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _CommandType, class _InputType, class _ModelType, class _SetPointType, class _Aspect = DefaultAspect >
    class Generator
        : public _Aspect::Port::ReadPort< _CommandType >,
          public _Aspect::Port::ReadPort< _InputType >,
          public _Aspect::Port::ReadPort< _ModelType >,
          public _Aspect::Port::WritePort< _SetPointType >,
          public _Aspect::BaseClass,
          public NameServerRegistrator< Generator<_CommandType, _InputType, _ModelType, _SetPointType, _Aspect>* >
            
    {
    public:
        typedef typename _CommandType::DataType CommandType;
        typedef typename _SetPointType::DataType SetPointType;
        typedef typename _InputType::DataType InputType;
        typedef typename _ModelType::DataType ModelType;
        typedef _Aspect Aspect;

        typedef typename Aspect::Port::ReadPort< CommandType >   Command;
        typedef typename Aspect::Port::ReadPort< InputType >     Input;
        typedef typename Aspect::Port::ReadPort< ModelType >  Model ;
        typedef typename Aspect::Port::WritePort< SetPointType > SetPoint;
            
        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Generator( ) : Aspect::BaseClass( "Generator" ) {}
        Generator(const std::string& name ) 
            : Aspect::BaseClass( "Generator" ), 
              NameServerRegistrator< Generator<_CommandType, _InputType, _ModelType, _SetPointType, _Aspect>* >(nameserver,name,this) 
        {}
            
        /**
         * Method Overloading is not done across scopes.
         * These lines introduce the methods anyway in this class.
         */
        using Input::readFrom;
        using Model::readFrom;
        using Command::readFrom;
        using Input::disconnect;
        using Model::disconnect;
        using Command::disconnect;
        using SetPoint::disconnect;

        /**
         * The Generator nameserver.
         */
        static NameServer< Generator<_CommandType, _InputType, _ModelType, _SetPointType, _Aspect>* > nameserver;
    };

    template <class C, class I, class M, class S, class A >
    //Generator<C,I,M,S,A>::NameServer<Generator<C,I,M,S,A>* > nameserver;
	NameServer<Generator<C,I,M,S,A>*> Generator<C,I,M,S,A>::nameserver;

    /**
     * An estimator defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _InputType, class _ModelType, class _Aspect = DefaultAspect >
    class Estimator
        : public _Aspect::Port::ReadPort< _InputType >,
          public _Aspect::Port::WritePort< _ModelType >,
          public _Aspect::BaseClass,
          public NameServerRegistrator< Estimator<_InputType, _ModelType, _Aspect>* >
        
    {
    public:
        typedef typename _InputType::DataType InputType;
        typedef typename _ModelType::DataType ModelType;
        typedef _Aspect Aspect;

        typedef typename Aspect::Port::ReadPort< InputType > Input;
        typedef typename Aspect::Port::WritePort< ModelType > Model;
            
        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Estimator() : Aspect::BaseClass( "Estimator" ) {}
        Estimator(const std::string& name ) 
            : Aspect::BaseClass( "Estimator" ), 
              NameServerRegistrator< Estimator<_InputType, _ModelType,  _Aspect>* >(nameserver,name,this) 
        {}
            

        using Input::disconnect;
        using Model::disconnect;
            
        /**
         * The Estimator nameserver.
         */
        static NameServer< Estimator<_InputType, _ModelType, _Aspect>* > nameserver;
    };

    template <class I, class M, class A >
    NameServer<Estimator<I,M,A>*> Estimator<I,M,A>::nameserver;


    /**
     * An effector defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _OutputType, class _Aspect = DefaultAspect >
    class Effector
        : public _Aspect::Port::ReadPort< _OutputType >,
          public _Aspect::BaseClass,
          public NameServerRegistrator< Effector<_OutputType, _Aspect>* >
    {
    public:
        typedef typename _OutputType::DataType OutputType;
        typedef _Aspect Aspect;

        typedef typename Aspect::Port::ReadPort< OutputType > Output;

        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Effector() : Aspect::BaseClass( "Effector" ) {}
        Effector(const std::string& name ) 
            : Aspect::BaseClass( "Effector" ), 
              NameServerRegistrator< Effector<_OutputType, _Aspect>* >(nameserver,name,this) 
        {}
            
        /**
         * The Effector nameserver.
         */
        static NameServer< Effector<_OutputType, _Aspect>* > nameserver;
            
    };
    template <class O, class A >
    //Effector<O,A>::NameServer<Effector<O,A>* > nameserver;
    NameServer<Effector<O,A>*> Effector<O,A>::nameserver;

    /**
     * A sensor defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _InputType, class _Aspect = DefaultAspect >
    class Sensor
        : public _Aspect::Port::WritePort< _InputType >,
          public _Aspect::BaseClass,
          public NameServerRegistrator< Sensor<_InputType, _Aspect>* >
        
    {
    public:
        typedef typename _InputType::DataType InputType;
        typedef _Aspect Aspect;
            
        typedef typename Aspect::Port::WritePort< InputType > Input;
            
        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Sensor() : Aspect::BaseClass( "Sensor" ) {}
        Sensor(const std::string& name ) 
            : Aspect::BaseClass( "Sensor" ), 
              NameServerRegistrator< Sensor<_InputType, _Aspect>* >(nameserver,name,this) 
        {}
            
        /**
         * The Sensor nameserver.
         */
        static NameServer< Sensor<_InputType, _Aspect>* > nameserver;
    };

    template <class I, class A >
	//Sensor<I,A>::NameServer<Sensor<I,A>* > nameserver;
    NameServer<Sensor<I,A>*> Sensor<I,A>::nameserver;

    /**
     * DefaultControlKernel is an example container class with default Components and user specified DataObjects.
     * This is the main class you should be using as a starting point. It is the most simple implementation of
     * a control kernel, new features will be added here first.
     *
     * The aim is such : 
     * First, create a ControlKernel with al the types of the DataObjects filled in. Then you get access
     * to the default components which are generated automatically by the ControlKernel. You inherit from
     * these Components to fill in the specific functionality you wish to put in.
     *
     * Extensions : An extension allows the DefaultControlKernel to extend the Components with methods and members
     * of arbitrary interfaces. These Component extensions are called 'Aspects'. Furthermore, the extension defines
     * functionality which will be executed after the DefaultControlKernel's functionality is done. This allows you
     * to add data reporting, logic control etc to be executed after the control loop has finished.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseExtension>
    class DefaultControlKernel
        : public _Extension,
          public NameServerRegistrator< DefaultControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:
        typedef _CommandType CommandType;
        typedef _SetPointType SetPointType;
        typedef _InputType InputType;
        typedef _ModelType ModelType;
        typedef _OutputType OutputType;
        typedef _Extension Extension;
            
        /**
         * The Aspect is defined by Kernel and user, through the Extension.
         * The Kernel specifies the port type, the user the extension and aspect.
         */
        template< class E >
        struct BaseAspect
        {
            typedef StandardPort Port;

            typedef typename E::CommonBase BaseClass;
        };

        typedef BaseAspect<Extension> CommonAspect;

        /**
         * Determine here the different DataObject kinds of this kernel.
         */
        typedef DataObject<SetPointType> SetPointData;
        typedef DataObjectLocked<CommandType>  CommandData;
        typedef DataObject<InputType>    InputData;
        typedef DataObject<ModelType>    ModelData;
        typedef DataObject<OutputType>   OutputData;


        typedef Controller<SetPointData, InputData, ModelData, OutputData, CommonAspect> DefaultController;
        typedef Generator<CommandData, InputData, ModelData, SetPointData, CommonAspect> DefaultGenerator;
        typedef Estimator<InputData, ModelData, CommonAspect> DefaultEstimator;
        typedef Effector<OutputData, CommonAspect> DefaultEffector;
        typedef Sensor<InputData, CommonAspect> DefaultSensor;
            
        /**
         * Set up a control kernel.
         */
        DefaultControlKernel()
            : _Extension(this), controller(&dummy_controller), generator(&dummy_generator), estimator(&dummy_estimator),
              effector(&dummy_effector), sensor(&dummy_sensor)
        {
            setController(controller);
            setGenerator(generator);
            setEstimator(estimator);
            setEffector(effector);
            setSensor(sensor);
        }
        DefaultControlKernel(const std::string& name)
            :NameServerRegistrator< DefaultControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this),
             controller(&dummy_controller), generator(&dummy_generator), estimator(&dummy_estimator),
             effector(&dummy_effector), sensor(&dummy_sensor)
        {
            setController(controller);
            setGenerator(generator);
            setEstimator(estimator);
            setEffector(effector);
            setSensor(sensor);
        }
            
        virtual bool initialize() 
        { 
            if ( !Extension::initialize() )
                return false;
                
            // initial startup of all components
            sensor->kernelStarted();
            estimator->kernelStarted();
            generator->kernelStarted();
            controller->kernelStarted();
            effector->kernelStarted();
            return true;
        }

        virtual void step() 
        {
            if ( isRunning() )
                {
                    // one step is one control cycle
                    // The figure is a unidirectional graph
                    sensor->update();
                    estimator->update();
                    generator->update();
                    controller->update();
                    effector->update();

                    // Call the extension (eg : reporting, execution engine, command interpreter... )
                    Extension::step();
                }  
        }

        virtual void finalize() 
        { 
            // termination of all components
            sensor->kernelStopped();
            estimator->kernelStopped();
            generator->kernelStopped();
            controller->kernelStopped();
            effector->kernelStopped();
            Extension::finalize();
        }
            
        /**
         * This method can be used to update the properties of this kernel.
         * Each application kernel will have different properties here.
         *
         * @param bag A PropertyBag containing the properties of this kernel.
         * @return true if a valid bag was given.
         *
         * @see KernelConfig.hpp
         */
        virtual bool updateKernelProperties( const PropertyBag& bag )
        {
            return KernelBaseExtension::updateProperties(bag);
        }

        void setController(DefaultController* c) { 
            controller->disableAspect();
            controller->disconnect(&models);
            controller->disconnect(&outputs);
            controller->disconnect(&setpoints);
            controller->disconnect(&inputs);
            controller=c;
            controller->writeTo(&outputs);
            controller->readFrom(&setpoints);
            controller->readFrom(&models);
            controller->readFrom(&inputs);
            controller->enableAspect(this);
            //KernelBaseExtension::enable(controller);
        }
            
        void setGenerator(DefaultGenerator* c) { 
            generator->disableAspect();
            generator->disconnect(&models);
            generator->disconnect(&setpoints);
            generator->disconnect(&commands);
            generator->disconnect(&inputs);
            generator=c;
            generator->writeTo(&setpoints);
            generator->readFrom(&models);
            generator->readFrom(&inputs);
            generator->readFrom(&commands);
            c->enableAspect(this);
        }

        void setEstimator(DefaultEstimator* c) { 
            estimator->disableAspect();
            estimator->disconnect(&models);
            estimator->disconnect(&inputs);
            estimator=c;
            estimator->writeTo(&models);
            estimator->readFrom(&inputs);
            estimator->enableAspect(this);
        }
            
        void setSensor(DefaultSensor* c) { 
            sensor->disableAspect();
            sensor->disconnect(&inputs);
            sensor=c;
            sensor->writeTo(&inputs);
            sensor->enableAspect(this);
        }
            
        void setEffector(DefaultEffector* c) { 
            effector->disableAspect();
            effector->disconnect(&outputs);
            effector=c;
            effector->readFrom(&outputs);
            effector->enableAspect(this);
        }

        /**
         * The DefaultControlKernel nameserver.
         */
        static NameServer< DefaultControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;
            
        /**
         * Returns the commands DataObject for this ControlKernel.
         */
        CommandData* getCommands() { return &commands; }

        /**
         * Returns the setpoints DataObject for this ControlKernel.
         */
        SetPointData* getSetpoints() { return &setpoints; }

        /**
         * Returns the models DataObject for this ControlKernel.
         */
        ModelData* getModels() { return &models; }

        /**
         * Returns the inputs DataObject for this ControlKernel.
         */
        InputData* getInputs() { return &inputs; }

        /**
         * Returns the outputs DataObject for this ControlKernel.
         */
        OutputData* getOutputs() { return &outputs; }

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
         * The user specified Data Objects.
         */
        SetPointData setpoints;
        CommandData  commands;
        InputData    inputs;
        ModelData    models;
        OutputData   outputs;

        bool abortState;

    };

    template <class C, class S, class I, class M, class O, class E >
    //DefaultControlKernel<C,S,I,M,O,E>::NameServer<DefaultControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<DefaultControlKernel<C,S,I,M,O,E>*> DefaultControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#endif

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
        : public _Aspect::Port::ReadPort< _SetPointType > ,
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

        typedef typename Aspect::Port::WritePort< _OutputType > Output;
        typedef typename Aspect::Port::ReadPort< _SetPointType > SetPoint;
        typedef typename Aspect::Port::ReadPort< _InputType > Input;
        typedef typename Aspect::Port::ReadPort< _ModelType > Model;

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

        typedef typename Aspect::Port::ReadPort< _CommandType >   Command;
        typedef typename Aspect::Port::ReadPort< _InputType >     Input;
        typedef typename Aspect::Port::ReadPort< _ModelType >  Model ;
        typedef typename Aspect::Port::WritePort< _SetPointType > SetPoint;
            
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

        typedef typename Aspect::Port::ReadPort< _InputType > Input;
        typedef typename Aspect::Port::WritePort< _ModelType > Model;
            
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

        typedef typename Aspect::Port::ReadPort< _OutputType > Output;

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
            
        typedef typename Aspect::Port::WritePort< _InputType > Input;
            
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
              effector(&dummy_effector), sensor(&dummy_sensor), startup(false)
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
             effector(&dummy_effector), sensor(&dummy_sensor), startup(false)
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
            kernelStarted.fire();
            
            startup = true;

            return true;
        }

        virtual void step() 
        {
            if ( isRunning() )
                {
                    if (startup)
                        {
                            startup = false;
                            sensor->componentStartUp();
                            estimator->componentStartUp();
                            generator->componentStartUp();
                            controller->componentStartUp();
                            effector->componentStartUp();
                        }
                            
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
            else
                {
                    // safe stop in abort().
                    if (!startup)
                        {
                            startup = true;
                            sensor->componentShutdown();
                            estimator->componentShutdown();
                            generator->componentShutdown();
                            controller->componentShutdown();
                            effector->componentShutdown();
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

        /**
         * Load a Controller Component into the kernel.
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
        bool isLoaded( const std::string& name ) {
            DefaultController* c;
            if ( (c = DefaultController::nameserver.getObjectByName( name )) )
                return isLoaded(c);
            return false;
        }

        bool loadController(DefaultController* c) {
            if ( isRunning() )
                return false;
            controllers.push_back( c );
            c->writeTo(&outputs);
            c->readFrom(&setpoints);
            c->readFrom(&models);
            c->readFrom(&inputs);
            c->enableAspect(this);
            return true;
        }

        bool unloadController(DefaultController* c) {
            if ( isRunning() )
                return false;
            std::vector<DefaultController*>::iterator itl = std::find( controllers.begin(), controllers.end(), c);
            if ( itl != controllers.end() )
                {
                    controllers.erase( c );
                    c->disableAspect();
                    c->disconnect(&models);
                    c->disconnect(&outputs);
                    c->disconnect(&setpoints);
                    c->disconnect(&inputs);
                    return true;
                }
            return false;
        }

        bool isLoaded(DefaultController* c) {
            return ( std::find(controllers.begin(), controllers.end(), c) != controllers.end() );
        }

        bool selectController(DefaultController* c) { 
            if ( ! isLoaded(c) )
                return false;

            if ( this->isRunning() )
                {
                    controller->componentShutdown();
                    c->componentStartUp();
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
        bool isLoaded( const std::string& name ) {
            DefaultGenerator* c;
            if ( (c = DefaultGenerator::nameserver.getObjectByName( name )) )
                return isLoaded(c);
            return false;
        }

        bool loadGenerator(DefaultGenerator* c) {
            if ( isRunning() )
                return false;
            generators.push_back( c );
            c->writeTo(&setpoints);
            c->readFrom(&models);
            c->readFrom(&inputs);
            c->readFrom(&commands);
            c->enableAspect(this);
            return true;
        }

        bool unloadGenerator(DefaultGenerator* c) {
            if ( isRunning() )
                return false;
            std::vector<DefaultGenerator*>::iterator itl = std::find( generators.begin(), generators.end(), c);
            if ( itl != generators.end() )
                {
                    generators.erase( c );
                    c->disableAspect();
                    c->disconnect(&models);
                    c->disconnect(&setpoints);
                    c->disconnect(&commands);
                    c->disconnect(&inputs);
                    return true;
                }
            return false;
        }

        bool isLoaded(DefaultGenerator* c) {
            return ( std::find(generators.begin(), generators.end(), c) != generators.end() );
        }

        void selectGenerator(DefaultGenerator* c) { 
            if ( ! isLoaded(c) )
                return false;
            if ( this->isRunning() )
                {
                    generator->componentShutdown();
                    c->componentStartUp();
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
        bool isLoaded( const std::string& name ) {
            DefaultEstimator* c;
            if ( (c = DefaultEstimator::nameserver.getObjectByName( name )) )
                return isLoaded(c);
            return false;
        }

        bool loadEstimator(DefaultEstimator* c) {
            if ( isRunning() )
                return false;
            estimators.push_back( c );
            c->writeTo(&models);
            c->readFrom(&inputs);
            c->enableAspect(this);
            return true;
        }

        bool unloadEstimator(DefaultEstimator* c) {
            if ( isRunning() )
                return false;
            std::vector<DefaultEstimator*>::iterator itl = std::find( estimators.begin(), estimators.end(), c);
            if ( itl != estimators.end() )
                {
                    estimators.erase( c );
                    c->disableAspect();
                    c->disconnect(&models);
                    c->disconnect(&inputs);
                    return true;
                }
            return false;
        }

        bool isLoaded(DefaultEstimator* c) {
            return ( std::find(estimators.begin(), estimators.end(), c) != estimators.end() );
        }

        void selectEstimator(DefaultEstimator* c) { 
            if ( ! isLoaded(c) )
                return false;
            if ( this->isRunning() )
                {
                    estimator->componentShutdown();
                    c->componentStartUp();
                }
            estimator=c;
            return true;
        }
            
        /**
         * Load a Sensor Component into the kernel.
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
         * UnLoad a Sensor Component from the kernel.
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
         * Select a Sensor Component from the kernel.
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
         * Query if a Sensor Component is loaded in the kernel.
         *
         * @param  name The name of the Sensor Component to query.
         * @return True if the Sensor Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoaded( const std::string& name ) {
            DefaultSensor* c;
            if ( (c = DefaultSensor::nameserver.getObjectByName( name )) )
                return isLoaded(c);
            return false;
        }

        bool loadSensor(DefaultSensor* c) {
            if ( isRunning() )
                return false;
            sensors.push_back( c );
            c->writeTo(&inputs);
            c->enableAspect(this);
            return true;
        }

        bool unloadSensor(DefaultSensor* c) {
            if ( isRunning() )
                return false;
            std::vector<DefaultSensor*>::iterator itl = std::find( sensors.begin(), sensors.end(), c);
            if ( itl != sensors.end() )
                {
                    sensors.erase( c );
                    c->disableAspect();
                    c->disconnect(&inputs);
                    return true;
                }
            return false;
        }

        bool isLoaded(DefaultSensor* c) {
            return ( std::find(sensors.begin(), sensors.end(), c) != sensors.end() );
        }

        void selectSensor(DefaultSensor* c) { 
            if ( ! isLoaded(c) )
                return false;
            if ( this->isRunning() )
                {
                    sensor->componentShutdown();
                    c->componentStartUp();
                }

            sensor=c;
            return true;
        }
            
        /**
         * Load a Effector Component into the kernel.
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
         * UnLoad a Effector Component from the kernel.
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
         * Select a Effector Component from the kernel.
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
         * Query if a Effector Component is loaded in the kernel.
         *
         * @param  name The name of the Effector Component to query.
         * @return True if the Effector Component is loaded in the kernel,
         *         False otherwise.
         */
        bool isLoaded( const std::string& name ) {
            DefaultEffector* c;
            if ( (c = DefaultEffector::nameserver.getObjectByName( name )) )
                return isLoaded(c);
            return false;
        }

        bool loadEffector(DefaultEffector* c) {
            if ( isRunning() )
                return false;
            effectors.push_back( c );
            c->readFrom(&outputs);
            c->enableAspect(this);
            return true;
        }

        bool unloadEffector(DefaultEffector* c) {
            if ( isRunning() )
                return false;
            std::vector<DefaultEffector*>::iterator itl = std::find( effectors.begin(), effectors.end(), c);
            if ( itl != effectors.end() )
                {
                    effectors.erase( c ); 
                    c->disableAspect();
                    c->disconnect(&outputs);
                    return true;
                }
            return false;
        }

        bool isLoaded(DefaultEffector* c) {
            return ( std::find(effectors.begin(), effectors.end(), c) != effectors.end() );
        }

        bool selectEffector(DefaultEffector* c) { 
            if ( ! isLoaded(c) )
                return false;
            if ( this->isRunning() )
                {
                    effector->componentShutdown();
                    c->componentStartUp();
                }

            effector=c;
            return true;
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

        bool startup;

        std::vector<DefaultController*> controllers;
        std::vector<DefaultGenerator*>  generators;
        std::vector<DefaultEffector*>   effectors;
        std::vector<DefaultEstimator*>  estimators;
        std::vector<DefaultSensor*>     sensors;
    };

    template <class C, class S, class I, class M, class O, class E >
    //DefaultControlKernel<C,S,I,M,O,E>::NameServer<DefaultControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<DefaultControlKernel<C,S,I,M,O,E>*> DefaultControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#endif

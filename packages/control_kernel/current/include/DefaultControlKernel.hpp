#ifndef DEFAULTCONTROLKERNEL_HPP
#define DEFAULTCONTROLKERNEL_HPP

#include "BaseKernel.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

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
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class DefaultControlKernel
        : public detail::BaseKernel< detail::StandardPort< DataObjectLocked<_CommandType> >, 
                                     detail::StandardPort< DataObject<_SetPointType> >, 
                                     detail::StandardPort< DataObject<_InputType> >, 
                                     detail::StandardPort< DataObject<_ModelType> >, 
                                     detail::StandardPort< DataObject<_OutputType> >, 
                                     _Extension >,
          public NameServerRegistrator< DefaultControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:
//         typedef _CommandType CommandType;
//         typedef _SetPointType SetPointType;
//         typedef _InputType InputType;
//         typedef _ModelType ModelType;
//         typedef _OutputType OutputType;
//         typedef _Extension Extension;

        /**
         * Determine here the different DataObject kinds of this kernel.
         */
//         typedef DataObject<SetPointType> SetPointData;
//         typedef DataObjectLocked<CommandType>  CommandData;
//         typedef DataObject<InputType>    InputData;
//         typedef DataObject<ModelType>    ModelData;
//         typedef DataObject<OutputType>   OutputData;

        // XXX TODO : Nameserved DataObject :
        // typedef DataContainer<SetPointType>::default SePointData; 
        // typedef DataContainer<CommandType>::locked CommandData;
        // typedef DataContainer<ModelType>::set_priority ModelType; 

//         typedef Controller<SetPointData, InputData, ModelData, OutputData, CommonAspect> DefaultController;
//         typedef Generator<CommandData, InputData, ModelData, SetPointData, CommonAspect> DefaultGenerator;
//         typedef Estimator<InputData, ModelData, CommonAspect> DefaultEstimator;
//         typedef Effector<OutputData, CommonAspect> DefaultEffector;
//         typedef Sensor<InputData, CommonAspect> DefaultSensor;
            
        /**
         * Set up a control kernel.
         */
        DefaultControlKernel()
        {}

        /**
         * Create a nameserved control kernel.
         */
        DefaultControlKernel(const std::string& name)
            :NameServerRegistrator< DefaultControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {}

        /**
         * The DefaultControlKernel nameserver.
         */
        static NameServer< DefaultControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;
            
    protected:

        virtual void updateComponents()
        {
            // This is called from the KernelBaseFunction
            // one step is one control cycle
            // The figure is a unidirectional graph
            sensor->update();
            estimator->update();
            generator->update();
            controller->update();
            effector->update();
        }
    };

    template <class C, class S, class I, class M, class O, class E >
    //DefaultControlKernel<C,S,I,M,O,E>::NameServer<DefaultControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<DefaultControlKernel<C,S,I,M,O,E>*> DefaultControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#endif

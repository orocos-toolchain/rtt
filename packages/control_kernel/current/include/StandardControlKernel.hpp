#ifndef STANDARDCONTROLKERNEL_HPP
#define STANDARDCONTROLKERNEL_HPP

#include "BaseKernel.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * StandardControlKernel is an example container class with standard Components and user specified DataObjects.
     * This is the main class you should be using as a starting point. It is the most simple implementation of
     * a control kernel, new features will be added here first.
     *
     * The aim is such : 
     * First, create a ControlKernel with al the types of the DataObjects filled in. Then you get access
     * to the standard components which are generated automatically by the ControlKernel. You inherit from
     * these Components to fill in the specific functionality you wish to put in.
     *
     * Extensions : An extension allows the StandardControlKernel to extend the Components with methods and members
     * of arbitrary interfaces. These Component extensions are called 'Aspects'. Furthermore, the extension defines
     * functionality which will be executed after the StandardControlKernel's functionality is done. This allows you
     * to add data reporting, logic control etc to be executed after the control loop has finished.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class StandardControlKernel
        : public detail::BaseKernel< detail::StandardPort< DataObjectLocked<_CommandType> >, 
                                     detail::StandardPort< DataObject<_SetPointType> >, 
                                     detail::StandardPort< DataObject<_InputType> >, 
                                     detail::StandardPort< DataObject<_ModelType> >, 
                                     detail::StandardPort< DataObject<_OutputType> >, 
                                     _Extension >,
          public NameServerRegistrator< StandardControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:
        /**
         * Set up a control kernel.
         */
        StandardControlKernel()
        {}

        /**
         * Create a nameserved control kernel.
         */
        StandardControlKernel(const std::string& name)
            :NameServerRegistrator< StandardControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {}

        /**
         * The StandardControlKernel nameserver.
         */
        static NameServer< StandardControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;
            
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
    //StandardControlKernel<C,S,I,M,O,E>::NameServer<StandardControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<StandardControlKernel<C,S,I,M,O,E>*> StandardControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#endif

#ifndef CASCADECONTROLKERNEL_HPP
#define CASCADECONTROLKERNEL_HPP

#include "BaseKernel.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * @brief CascadeControlKernel is a kernel specifically
     * to be used in conjuction with another kernel
     * (an CascadeInnerControlKernel). No Estimator can
     * be loaded in this kernel.
     *
     * It will re-use the Model and Inputs DataObjects of
     * the inner kernel. The Outputs of this kernel
     * will match the commands of the inner kernel.
     * Hence, no Estimator and Sensor component can be
     * loaded.
     * The model of the inner kernel will be used (for
     * example the CascadeInnerControlKernel).
     * 
     * A CascadeControlKernel always assumes that model
     * and inputs are calculated in the inner kernel it uses,
     * hence it will never have an estimator or sensor 
     * component. It can have an optional Effector component
     * for addressing additional, higher level hardware.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class CascadeControlKernel
        : public detail::BaseKernel< detail::StandardPort< DataObjectPriorityGet<_CommandType> >, 
                                     detail::StandardPort< DataObject<_SetPointType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_InputType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_ModelType> >, 
                                     detail::StandardPort< DataObjectPriorityGet<_OutputType> >, 
                                     _Extension >,
          public NameServerRegistrator< CascadeControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:
        /** 
         * @brief Set up a control kernel which has access to
         * an CascadeInnerKernel for cascading.
         * 
         * @param id The Input DataObject of this kernel
         * (usually the Input DataObject of the inner kernel)
         * @param md The Model DataObject of this kernel
         * (usually the Model DataObject of the inner kernel)
         * @param cd The Output DataObject of this kernel
         * (usually the Command DataObject of the inner kernel)
         * 
         */
        CascadeControlKernel( InputData* id, ModelData* md, OutputData* cd)
        {
            setInputs(id);
            setModels(md);
            setOutputs(cd);
        }

        /**
         * @brief Create a nameserved control kernel.
         *
         * @param id The Input DataObject of this kernel
         * (usually the Input DataObject of the inner kernel)
         * @param md The Model DataObject of this kernel
         * (usually the Model DataObject of the inner kernel)
         * @param cd The Output DataObject of this kernel
         * (usually the Command DataObject of the inner kernel)
         * @param name The name of this kernel. 
         */
        CascadeControlKernel(const std::string& name, InputData* id, ModelData* md, OutputData* cd)
            :NameServerRegistrator< CascadeControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {
            setInputs(id);
            setModels(md);
            setOutputs(cd);
        }

        /**
         * The CascadeControlKernel nameserver.
         */
        static NameServer< CascadeControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;

        bool loadSensor(DefaultEffector* c) { return false;}
        bool loadEstimator(DefaultEffector* c) { return false;}
            
    protected:

        virtual void updateComponents()
        {
            // Only two components can be loaded in this kernel.
            generator->update();
            controller->update();
        }
    };

    template <class C, class S, class I, class M, class O, class E >
    //CascadeControlKernel<C,S,I,M,O,E>::NameServer<CascadeControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<CascadeControlKernel<C,S,I,M,O,E>*> CascadeControlKernel<C,S,I,M,O,E>::nameserver;

    /**
     * @brief CascadeInnerControlKernel is a kernel specifically to be used in conjuction with another kernel
     * (a cascade kernel). This is a fully functional kernel which can also be used standalone.
     *
     * The DataObjects of this kernel are choosen in such a way that another kernel (or thread) can
     * have thread-safe access to them, as long as it has a lower priority. It assumes that it has
     * the highest priority access of all its data objects, meaning, a higher priority thread will
     * never read them.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class CascadeInnerControlKernel
        : public detail::BaseKernel< detail::StandardPort< DataObjectPriorityGet<_CommandType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_SetPointType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_InputType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_ModelType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_OutputType> >, 
                                     _Extension >,
          public NameServerRegistrator< CascadeInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:
        /** 
         * @brief Set up a control kernel which allows access
         * for cascading.
         */
        CascadeInnerControlKernel()
        {
        }

        /**
         * @brief Create a nameserved control kernel.
         */
        CascadeInnerControlKernel(const std::string& name)
            :NameServerRegistrator< CascadeInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {
        }

        /**
         * The CascadeInnerControlKernel nameserver.
         */
        static NameServer< CascadeInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;

    protected:

        virtual void updateComponents()
        {
            sensor->update();
            estimator->update();
            generator->update();
            controller->update();
            effector->update();
        }
    };

    template <class C, class S, class I, class M, class O, class E >
    //CascadeInnerControlKernel<C,S,I,M,O,E>::NameServer<CascadeInnerControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<CascadeInnerControlKernel<C,S,I,M,O,E>*> CascadeInnerControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#endif

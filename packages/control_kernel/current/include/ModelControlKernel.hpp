#ifndef MODELCONTROLKERNEL_HPP
#define MODELCONTROLKERNEL_HPP

#include "BaseKernel.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * @brief ModelControlKernel is a kernel specifically to be used in conjuction with another kernel
     * (an ModelInnerControlKernel ).
     *
     * It will re-use Input DataObjects of the inner kernel. The Outputs of this kernel
     * will match the commands of the inner kernel. Hence, no Effector and Sensor component can be
     * loaded.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class ModelControlKernel
        : public detail::BaseKernel< detail::StandardPort< DataObjectPriorityGet<_CommandType> >, 
                                     detail::StandardPort< DataObject<_SetPointType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_InputType> >, 
                                     detail::StandardPort< DataObjectPriorityGet<_ModelType> >, 
                                     detail::StandardPort< DataObjectPriorityGet<_OutputType> >, 
                                     _Extension >,
          public NameServerRegistrator< ModelControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:
        /** 
         * @brief Set up a control kernel which has access to
         * an InnerKernel for cascading.
         * 
         * @param id The Input DataObject of this kernel
         * (usually the Input DataObject of the inner kernel)
         * @param cd The Output DataObject of this kernel
         * (usually the Command DataObject of the inner kernel)
         * 
         */
        ModelControlKernel( InputData* id, OutputData* cd)
        {
            setInputs(id);
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
        ModelControlKernel(const std::string& name, InputData* id, OutputData* cd)
            :NameServerRegistrator< ModelControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {
            setInputs(id);
            setOutputs(cd);
        }

        /**
         * The ModelControlKernel nameserver.
         */
        static NameServer< ModelControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;

        bool loadEffector(DefaultEffector* c) { return false;}
        bool loadSensor(DefaultEffector* c) { return false;}

        typedef DataObjectPrioritySet<_ModelType> CascadeModelType;
        CascadeModelType* getCascadeModels() const { return &model_copy; }
    protected:

        virtual void updateComponents()
        {
            // Only two components can be loaded in this kernel.
            estimator->update();
            {
                // copy model for cascade (needed because of priority switch)
                _ModelType m_copy;
                getModels()->Get(m_copy);
                getCascadeModels->Set(m_copy);
            }
            generator->update();
            controller->update();
        }
    };

    template <class C, class S, class I, class M, class O, class E >
    //ModelControlKernel<C,S,I,M,O,E>::NameServer<ModelControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<ModelControlKernel<C,S,I,M,O,E>*> ModelControlKernel<C,S,I,M,O,E>::nameserver;

    /**
     * @brief ModelInnerControlKernel is a kernel specifically to be used in conjuction with another kernel
     * (a model kernel). This is a fully functional kernel which can also be used standalone.
     *
     * The DataObjects of this kernel are choosen in such a way that another kernel (or thread) can
     * have thread-safe access to them, as long as it has a lower priority. It assumes that it has
     * the highest priority access of all its data objects, meaning, a higher priority thread will
     * never read them.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class ModelInnerControlKernel
        : public detail::BaseKernel< detail::StandardPort< DataObjectPriorityGet<_CommandType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_SetPointType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_InputType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_ModelType> >, 
                                     detail::StandardPort< DataObjectPrioritySet<_OutputType> >, 
                                     _Extension >,
          public NameServerRegistrator< ModelInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:
        /** 
         * @brief Set up a control kernel which allows access
         * for the ModelControlKernel.
         */
        ModelInnerControlKernel()
        {
        }

        /**
         * @brief Create a nameserved control kernel.
         */
        ModelInnerControlKernel(const std::string& name)
            :NameServerRegistrator< ModelInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {
        }

        /**
         * The ModelInnerControlKernel nameserver.
         */
        static NameServer< ModelInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;

        bool loadEstimator(DefaultEstimator* e) { return false; }

    protected:

        virtual void updateComponents()
        {
            sensor->update();
            generator->update();
            controller->update();
            effector->update();
        }
    };

    template <class C, class S, class I, class M, class O, class E >
    //ModelInnerControlKernel<C,S,I,M,O,E>::NameServer<ModelInnerControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<ModelInnerControlKernel<C,S,I,M,O,E>*> ModelInnerControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#endif

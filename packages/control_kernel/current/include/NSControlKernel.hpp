#ifndef NSCONTROLKERNEL_HPP
#define NSCONTROLKERNEL_HPP

#include "BaseKernel.hpp"
#include "DataServer.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * @brief The NSControlKernel is a more advanced version than the StandardControlKernel.
     *
     * As it has very much in common with the StandardControlKernel, the way of addressing
     * DataObjects is done through nameserving (strings). The basic interface to dataobjects
     * is defined by the DataObjectServer. The Get and Set methods of that class are extended
     * with an extra parameter which denotes the name of the object you need. The kernel
     * also supports the 'Old way' of accessing DataObjects through a Get() method without
     * strings. 
     *
     * The user must defines its DataTypes in such a way that the kernel can distinguish
     * what must be nameserved (and under which name ) and what not. An example is as
     * follows :
     * @verbatim
     *
     * Example : A Container indicating the ServedTypes and the UnServedType of a DataObject.
     * The constructor must contain the names for the ServedTypes.
     *
     * using std::make_pair;
     *
     * struct MyInputTypes 
     *   : public ServedTypes<PosXY, MouseVelocities>, public UnServedType<NormalInputs>
     * {
     *   /**
     *    * Obliged giving of names to all ServedTypes
     *    *\/
     *   InputTypes() 
     *   {
     *       // two objects of PosXY :
     *       this->insert( make_pair(0,"Position1"));
     *       this->insert( make_pair(0,"Position2"));
     *       // one object of MouseVelocities :
     *       this->insert( make_pair(1,"MouseVels"));
     *       // not used, but allowed :
     *       this->insert( make_pair(2,"Forces"));
     *       this->insert( make_pair(3,"Torques"));
     *   }
     * };
     * @endverbatim
     *
     * Next, you can pass this class name as the _InputType parameter of the 
     * NSControlKernel. The UnServedType< T > class can be used to convert
     * your StandardControlKernel components first, with substituting T with the
     * type you used in that kernel.
     *
     * See the manual for more information.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class NSControlKernel
        : public detail::BaseKernel< detail::StandardPort< typename detail::NamesDOFactory<_CommandType>::locked >, 
                                     detail::StandardPort< typename detail::NamesDOFactory<_SetPointType>::fast >, 
                                     detail::StandardPort< typename detail::NamesDOFactory<_InputType>::fast >, 
                                     detail::StandardPort< typename detail::NamesDOFactory<_ModelType>::fast >, 
                                     detail::StandardPort< typename detail::NamesDOFactory<_OutputType>::fast >, 
                                     _Extension >,
          public NameServerRegistrator< NSControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:

        /** 
         * @brief Set up a control kernel. 
         * 
         * @param prefix The namespace prefix of the data objects.
         * 
         */
        NSControlKernel(const std::string& prefix)
            : BaseKernel( prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                          prefix + "::SetPoints", prefix + "::Outputs")
        {}

        /**
         * @brief Create a nameserved control kernel.
         
         * @param prefix The namespace prefix of the data objects.
         * @param name The name of the kernel
         */
        NSControlKernel(const std::string& prefix, const std::string& name)
            : BaseKernel( prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                          prefix + "::SetPoints", prefix + "::Outputs"),
              NameServerRegistrator< NSControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {
        }

        /**
         * @brief The NSControlKernel nameserver.
         */
        static NameServer< NSControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;

        /**
         * @brief Add a DataObject to the Outputs.
         */
        template< class _DataObjecType>
        void addOutputs(_DataObjectType* o, const std::string& name)
        {
            getOutputs()->reg( o, name );
        }
            
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
    NameServer<NSControlKernel<C,S,I,M,O,E>*> NSControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#endif

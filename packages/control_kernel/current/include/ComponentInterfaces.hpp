#ifndef COMPONENTINTERFACES_HPP
#define COMPONENTINTERFACES_HPP

#include <corelib/Property.hpp>
#include "ModuleControlInterface.hpp"

#pragma interface

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    namespace detail 
    {
        class ControlKernelInterface;

        /**
         * @brief An Aspect is a base class of a component, implementing a
         * functionality like reporting, configuration,...
         *
         * A template class for Aspects, which can be used, optionally, by Extensions of the
         * ControlKernel. This interface instructs the Aspect to register itself with a certain
         * Extension. You must use this as a base class for every Aspect you want to provide
         * to a component.
         *
         * The base class of most Extensions is the ExtensionInterface.
         */
        template< class _Extension >
        class ComponentAspectInterface
        {
        public:

            /**
             * The Extension (of the kernel) that this Aspect provides support for.
             */
            typedef _Extension Extension;
        
            /**
             * @brief Initialize the ComponentAspect with the name of the Aspect.
             * Once set, a name can no longer be changed.
             */
            ComponentAspectInterface(const std::string& comp_name )
                : aspectName("Name", "The Name of this Aspect", comp_name) {}

            virtual ~ComponentAspectInterface() {}
        
            /**
             * @brief Instructs the component to enable an aspect so that this aspect can
             * deliver a service to the Extension <ext> of the ControlKernel.
             *
             * This will be called when the component is loaded into the kernel.
             */
            virtual bool enableAspect( Extension* ext ) = 0;

            /**
             * @brief Disable this aspect and no longer use the Extension of the kernel.
             */
            virtual void disableAspect() = 0;

            /**
             * @brief Return the name of this Aspect.
             */
            virtual const std::string& getName() { return aspectName.get(); }

        protected:
            const Property<std::string> aspectName;
        };
    }

    class KernelBaseFunction;

    /**
     * @brief The Base class of each ControlKernel Component
     *
     * The most fundamental aspect of a Component is that it belongs
     * to a ControlKernel and can be notified of the kernel's status.
     * This aspect introduces the ability to detect if the component
     * is placed in a ControlKernel and to return a pointer to this kernel.
     * It is the aspect of the KernelBaseFunction.
     * 
     */
    class ComponentBaseInterface 
        :public ModuleControlInterface,
         public detail::ComponentAspectInterface< KernelBaseFunction >
    {
        friend class KernelBaseFunction;
        using detail::ComponentAspectInterface< KernelBaseFunction >::enableAspect;
    public:
        /**
         * Constructor.
         */
        ComponentBaseInterface(const std::string& name)
            : detail::ComponentAspectInterface< KernelBaseFunction >( name ),
              selected(false), kern(0) {}

        virtual ~ComponentBaseInterface() {}
            
        /**
         * @brief Inspect if this component is placed in a kernel.
         * This is the 'isLoaded()' query actually.
         * 
         * @return true if it is so.
         */
        bool inKernel() { return kern != 0; }
            
        /**
         * @brief Return the kernel this component belongs to.
         * 
         * @return The kernel it belongs to, zero if none.
         */
        KernelBaseFunction* kernel() { return kern; }

        virtual bool enableAspect(KernelBaseFunction* e);

        /**
         * @brief This method is a hook which is called when the component
         * is loaded into the kernel.
         *
         * @return true on success, false otherwise
         */
        virtual bool componentLoaded() { return true;}
            
        /**
         * @brief This method is a hook which is called when a component
         * was unloaded from the kernel.
         */
        virtual void componentUnloaded() {}

        /**
         * @brief This method is a hook which is called when the kernel
         * is started and the component must initialise the data
         * objects with meaningfull data.
         *
         * @return true on success, false otherwise
         */
        virtual bool componentStartup() { return true; }

        /**
         * @brief This method is a hook which is called when the kernel
         * is stopped and the component must return to a safe state.
         */
        virtual void componentShutdown() {}

        /**
         * @brief If the component wants to alter its DataObject,
         * it can do so here.
         *
         * This method is called just before componentLoaded()
         * and allows a component to alter the contents of a 
         * DataObject before it is created :-]. It is used by
         * the StandardControlKernel after the port is created,
         * but before the actual dataobject is created.
         */
        virtual void initDataObject() {}

        virtual void disableAspect();

        /**
         * @brief Query if this component is selected in the kernel.
         */
        bool isSelected() { return selected; }

    protected:
        bool selected;
    private:
        KernelBaseFunction* kern;
    };

    /**
     * @brief The DefaultBase is defined for
     * ControlKernels that do not know of 
     * Component Aspects. It will be used as default
     * template parameter for the components.
     *
     * A Component Aspect is the base class that a Component
     * must have to be able to be queried by the respective Kernel Extension.
     * As a consequence, Extensions themselves define the component base class they 
     * require. That class is passed through by the kernel to the Component.
     * This relieves the burden of figuring it out manually from the shoulders of the user.
     */
    typedef ComponentBaseInterface DefaultBase;
}

#endif

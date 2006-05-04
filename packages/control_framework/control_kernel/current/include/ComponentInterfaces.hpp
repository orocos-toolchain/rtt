/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:24 CEST 2004  ComponentInterfaces.hpp 

                        ComponentInterfaces.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#ifndef COMPONENTINTERFACES_HPP
#define COMPONENTINTERFACES_HPP

#include <corelib/Property.hpp>
#include "DataFlowInterface.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    class ComponentBaseInterface;

    namespace detail 
    {

        /**
         * @brief An Facet is a base class of a component, implementing a
         * functionality like reporting, configuration,...
         *
         * A template class for Facets, which can be used, optionally, by Extensions of the
         * ControlKernel. This interface instructs the Facet to register itself with a certain
         * Extension. You must use this as a base class for every Facet you want to provide
         * to a component.
         *
         * The base class of most Extensions is the ExtensionInterface.
         */
        template< class _Extension >
        class ComponentFacetInterface
        {
        public:

            /**
             * The Extension (of the kernel) that this Facet provides support for.
             */
            typedef _Extension Extension;
        
            /**
             * @brief Initialize the ComponentFacet with the name of the Facet.
             * Once set, a name can no longer be changed.
             */
            ComponentFacetInterface(const std::string& comp_name )
                : FacetName("Name", "The Name of this Facet", comp_name) {}

            virtual ~ComponentFacetInterface() {}
        
            /**
             * @brief Instructs the component to enable an Facet so that this Facet can
             * deliver a service to the Extension \a ext of the ControlKernel.
             *
             * This will be called when the component is loaded into the kernel.
             */
            virtual bool enableFacet( Extension* ext ) = 0;

            /**
             * @brief Disable this Facet and no longer use the Extension of the kernel.
             */
            virtual void disableFacet() = 0;

            /**
             * @brief Return the name of this Facet.
             */
            virtual const std::string& getFacetName() { return FacetName.value(); }

        protected:
            /**
             * Return a pointer to the component base interface.
             */
            ComponentBaseInterface* component() const { return comp; }

            /**
             * Set the pointer to the component base interface.
             */
            void component(ComponentBaseInterface* cbi) { comp = cbi; }
            
            Property<std::string> FacetName;
            ComponentBaseInterface* comp;
        };
    }

    class KernelBaseFunction;
    /**
     * @defgroup compinterf Component Interfaces (Facets)
     * The various interfaces (facets) the Orocos Control Kernel Components may implement.
     * Each Component Interface matches a Control Kernel Extension which 'knows'
     * how to interact with the Component through that interface. 
     * These interfaces are not application specific, but cover Component properties,
     * starting and stopping components and logging functionality.
     */

    /**
     * @brief The Base class of each ControlKernel Component
     *
     * The most fundamental Facet of a Component is that it belongs
     * to a ControlKernel and can be notified of the kernel's status.
     * This Facet introduces the ability to detect if the component
     * is placed in a ControlKernel and to return a pointer to this kernel.
     * It is the Facet of the KernelBaseFunction.
     * @ingroup compinterf
     * 
     */
    class ComponentBaseInterface 
        : public detail::ComponentFacetInterface< KernelBaseFunction >
    {
        friend class KernelBaseFunction;
        using detail::ComponentFacetInterface< KernelBaseFunction >::enableFacet;
    public:
        /**
         * Constructor.
         */
        ComponentBaseInterface(const std::string& name)
            : detail::ComponentFacetInterface< KernelBaseFunction >( name ),
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

        virtual bool enableFacet(KernelBaseFunction* e);

        /**
         * Get the Name of this Component.
         */
        const std::string& getName() {
            return detail::ComponentFacetInterface< KernelBaseFunction >::getFacetName();
        }

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

        virtual void disableFacet();

        /**
         * @brief Shutdown (deselect) this Component.
         *
         * @return true if the component was loaded and could be shutdowned.
         */
        bool shutdown();

        /**
         * @brief Startup (select) this Component.
         *
         * @return true if the component was loaded and could be started.
         */
        bool startup();

        /**
         * @brief Restart (deselect + select) this Component.
         *
         * @return true if the component was loaded and could be restarted.
         */
        bool restart();

        /*
         * @brief Select ( startup ) this Component.
         *
         * @return true if the component was loaded and could be selected
         */
        bool select();

        /**
         * @brief Query if this component is selected in the kernel.
         */
        bool isSelected() { return selected; }

        /**
         * @brief Query if this component is started and running.
         */
        bool isRunning() { return selected; }

        /**
         * This method is called once in each period of the kernel.
         * A Data Flow Component will then by default pull()/calculate()/push() data,
         * while a Process Component may perform its execution flow activity.
         */
        virtual void update() = 0;

    protected:
        bool selected;
    private:
        KernelBaseFunction* kern;
    };

    /**
     * @brief The DefaultBase is defined for
     * ControlKernels that do not know of 
     * Component Facets. It will be used as default
     * template parameter for the components.
     *
     * A Component Facet is the base class that a Component
     * must have to be able to be queried by the respective Kernel Extension.
     * As a consequence, Extensions themselves define the component base class they 
     * require. That class is passed through by the kernel to the Component.
     * This relieves the burden of figuring it out manually from the shoulders of the user.
     */
    typedef ComponentBaseInterface DefaultBase;
}

#endif

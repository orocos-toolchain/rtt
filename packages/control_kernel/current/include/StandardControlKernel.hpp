/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  StandardControlKernel.hpp 

                        StandardControlKernel.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
#ifndef STANDARDCONTROLKERNEL_HPP
#define STANDARDCONTROLKERNEL_HPP

#include <pkgconf/control_kernel.h>
#ifdef OROSEM_CONTROL_KERNEL_OLDKERNEL
#include "BaseKernel.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * @brief StandardControlKernel is an example container class with standard Components and user specified DataObjects.
     *
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
     *
     * @deprecated By the NSControlKernel. This class will probably be removed and the NSControlKernel will be
     * called the StandardControlKernel... in Orocos 0.16.0
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
            this->sensor->update();
            this->estimator->update();
            this->generator->update();
            this->controller->update();
            this->effector->update();
        }
    };

    template <class C, class S, class I, class M, class O, class E >
    //StandardControlKernel<C,S,I,M,O,E>::NameServer<StandardControlKernel<C,S,I,M,O,E>* > nameserver;
    NameServer<StandardControlKernel<C,S,I,M,O,E>*> StandardControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#else

#include "NextGenKernel.hpp"
#include "DataServer.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * @brief The StandardControlKernel is the latest Control Kernel Architecture which supports
     * separate compiled Components.
     *
     * See the Control Kernel Manual for more information.
     */
    template <class _Extension = KernelBaseFunction >
    class StandardControlKernel
        : public detail::BaseKernel< detail::DataObjectLockedC,
                                        detail::DataObjectC,
                                        detail::DataObjectC,
                                        detail::DataObjectC,
                                        detail::DataObjectC,
                                       _Extension >
    {
    public:

        /** 
         * @brief Set up a control kernel. 
         * 
         * @param name The unique name of the kernel.
         * 
         */
        StandardControlKernel(const std::string& name)
            : detail::BaseKernel< detail::DataObjectLockedC,
                                     detail::DataObjectC,
                                     detail::DataObjectC,
                                     detail::DataObjectC,
                                     detail::DataObjectC,
                                     _Extension >( name, name + "::Inputs", name + "::Models", name + "::Commands",
                                                   name + "::SetPoints", name + "::Outputs")
        {
        }

        /**
         * @brief Create a nameserved control kernel.
         
         * @param name The unique name of the kernel
         * @param prefix The prefix to use for the DataObject names. Set prefix
         * to the name of another kernel, to be able to access its data objects.
         */
        StandardControlKernel(const std::string& name, const std::string& prefix)
            : detail::BaseKernel< detail::DataObjectLockedC,
                                  detail::DataObjectC,
                                  detail::DataObjectC,
                                  detail::DataObjectC,
                                  detail::DataObjectC,
                                  _Extension >( name, prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                                                prefix + "::SetPoints", prefix + "::Outputs")
        {
        }

    protected:

        virtual void updateComponents()
        {
            // This is called from the KernelBaseFunction
            // one step is one control cycle
            // The figure is a unidirectional graph
            this->sensor->update();
            this->estimator->update();
            this->generator->update();
            this->controller->update();
            this->effector->update();
        }
    };

                
}

#endif
#endif

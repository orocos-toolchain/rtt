/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  CascadeNSControlKernel.hpp 

                        CascadeNSControlKernel.hpp -  description
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
 
#ifndef CASCADE_NS_CONTROL_KERNEL_HPP
#define CASCADE_NS_CONTROL_KERNEL_HPP

#include "BaseKernel.hpp"
#include "DataServer.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * @brief The CascadeNSControlKernel is a more advanced version than the CascadeControlKernel.
     * @see CascadeControlKernel
     * @see NSControlKernel
     *
     * See the manual for more information.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class CascadeNSControlKernel
        : public detail::BaseKernel< detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC , 
                                     _Extension >,
          public NameServerRegistrator< CascadeNSControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:

        /** 
         * @brief Set up a control kernel which has access to
         * an CascadeKernel for cascading.
         * 
         * @param name The name of this kernel. 
         * 
         */
        CascadeNSControlKernel(const std::string& name)
            : detail::BaseKernel< detail::StandardPort< typename detail::NamesDOFactory<_CommandType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_SetPointType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_InputType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_ModelType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_OutputType>::buffer >,
                                  _Extension >(name, name + "::Inputs", name + "::Models", name + "::Commands",
                                                name + "::SetPoints", name + "::Commands")
        {
        }

        /**
         * @brief Create a nameserved control kernel.
         *
         * @param name The name of this kernel. 
         * @param prefix The prefix to use for the DataObject names. Set prefix
         * to the name of another kernel, to be able to access its data objects.
         */
        CascadeNSControlKernel(const std::string& name, const std::string& prefix)
            : detail::BaseKernel< detail::StandardPort< typename detail::NamesDOFactory<_CommandType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_SetPointType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_InputType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_ModelType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_OutputType>::buffer >,
                                  _Extension >(name, prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                                                prefix + "::SetPoints", prefix + "::Commands"),
              NameServerRegistrator< CascadeNSControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {
        }

        /**
         * @brief The CascadeNSControlKernel nameserver.
         */
        static NameServer< CascadeNSControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;

    protected:

        virtual void updateComponents()
        {
            // If a DataObject is external, 
            // we do not own it and thus will not write to it...
            if ( !this->externalInputs )
                this->sensor->update();
            if ( !this->externalModels )
                this->estimator->update();
            if ( !this->externalSetPoints )
                this->generator->update();
            if ( !this->externalOutputs )
                this->controller->update();
            
            // does not write to a dataobject.
            this->effector->update();
        }
    };

    template <class C, class S, class I, class M, class O, class E >
    NameServer<CascadeNSControlKernel<C,S,I,M,O,E>*> CascadeNSControlKernel<C,S,I,M,O,E>::nameserver;

    /**
     * @brief CascadeNSInnerControlKernel is a kernel specifically to be used in conjuction with another kernel
     * (a cascade kernel). This is a fully functional kernel which can also be used standalone.
     *
     * The DataObjects of this kernel are choosen in such a way that another kernel (or thread) can
     * have thread-safe access to them, as long as it has a lower priority. It assumes that it has
     * the highest priority access of all its data objects, meaning, a higher priority thread will
     * never read them.
     */
    template <class _CommandType, class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Extension = KernelBaseFunction>
    class CascadeNSInnerControlKernel
        : public detail::BaseKernel< detail::StandardPort< typename detail::NamesDOFactory<_CommandType>::priority_get >, 
                                     detail::StandardPort< typename detail::NamesDOFactory<_SetPointType>::fast >, 
                                     detail::StandardPort< typename detail::NamesDOFactory<_InputType>::priority_set >, 
                                     detail::StandardPort< typename detail::NamesDOFactory<_ModelType>::priority_set >, 
                                     detail::StandardPort< typename detail::NamesDOFactory<_OutputType>::priority_get >, 
                                     _Extension >,
          public NameServerRegistrator< CascadeNSInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >        
    {
    public:
        /** 
         * @brief Set up a control kernel which allows access
         * for cascading.
         * @param prefix The namespace prefix of the data objects.
         * 
         */
        CascadeNSInnerControlKernel(const std::string& prefix)
            : detail::BaseKernel< detail::StandardPort< typename detail::NamesDOFactory<_CommandType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_SetPointType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_InputType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_ModelType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_OutputType>::buffer >,
                                  _Extension >( prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                                                prefix + "::SetPoints", prefix + "::Outputs")
        {
        }

        /**
         * @brief Create a nameserved control kernel.
         *
         * @param prefix The namespace prefix of the data objects.
         * @param name The name of the kernel.
         * 
         */
        CascadeNSInnerControlKernel(const std::string& prefix, const std::string& name)
            : detail::BaseKernel< detail::StandardPort< typename detail::NamesDOFactory<_CommandType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_SetPointType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_InputType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_ModelType>::buffer >,
                                  detail::StandardPort< typename detail::NamesDOFactory<_OutputType>::buffer >,
                                  _Extension >( prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                                                prefix + "::SetPoints", prefix + "::Outputs"),
              NameServerRegistrator< CascadeNSInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* >(nameserver,name,this)
        {
        }

        /**
         * The CascadeNSInnerControlKernel nameserver.
         */
        static NameServer< CascadeNSInnerControlKernel<_CommandType, _SetPointType,_InputType, _ModelType, _OutputType, _Extension>* > nameserver;

    protected:

        virtual void updateComponents()
        {
            this->sensor->update();
            this->estimator->update();
            this->generator->update();
            this->controller->update();
            this->effector->update();
        }
    };

    template <class C, class S, class I, class M, class O, class E >
    NameServer<CascadeNSInnerControlKernel<C,S,I,M,O,E>*> CascadeNSInnerControlKernel<C,S,I,M,O,E>::nameserver;

                
}

#endif

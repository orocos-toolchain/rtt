/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  NewControlKernel.hpp 

                        NewControlKernel.hpp -  description
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
 
#ifndef NewCONTROLKERNEL_HPP
#define NewCONTROLKERNEL_HPP

#include "NextGenKernel.hpp"
#include "DataServer.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * @brief The NewControlKernel is a more advanced version than the StandardControlKernel.
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
     *   //
     *   // Obliged giving of names to all ServedTypes
     *   //
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
     * NewControlKernel. The UnServedType< T > class can be used to convert
     * your StandardControlKernel components first, with substituting T with the
     * type you used in that kernel.
     *
     * See the manual for more information.
     */
    template <class _Extension = KernelBaseFunction >
    class NewControlKernel
        : public detail::NextGenKernel< detail::DataObjectLockedC,
                                        detail::DataObjectC,
                                        detail::DataObjectC,
                                        detail::DataObjectC,
                                        detail::DataObjectC,
                                       _Extension >,
          public NameServerRegistrator< NewControlKernel<_Extension>* >        
    {
    public:

        /** 
         * @brief Set up a control kernel. 
         * 
         * @param name The unique name of the kernel.
         * 
         */
        NewControlKernel(const std::string& name)
            : detail::NextGenKernel< detail::DataObjectLockedC,
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
        NewControlKernel(const std::string& name, const std::string& prefix)
            : detail::NextGenKernel< detail::DataObjectLockedC,
                                  detail::DataObjectC,
                                  detail::DataObjectC,
                                  detail::DataObjectC,
                                  detail::DataObjectC,
                                  _Extension >( name, prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                                                prefix + "::SetPoints", prefix + "::Outputs"),
              NameServerRegistrator< NewControlKernel<_Extension>* >(nameserver,name,this)
        {
        }

        /**
         * @brief The NewControlKernel nameserver.
         */
        static NameServer< NewControlKernel<_Extension>* > nameserver;

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

    template < class E >
    NameServer<NewControlKernel<E>*> NewControlKernel<E>::nameserver;

                
}

#endif

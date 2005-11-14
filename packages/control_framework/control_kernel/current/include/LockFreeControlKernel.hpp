/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  LockFreeControlKernel.hpp 
       Erwin Aertbelien  oct. 2005 : small changes to have the same functionality
                                     as StandardControlKernel

                        BufferedControlKernel.hpp -  description
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

#ifndef LOCKFREE_CONTROL_KERNEL_HPP
#define LOCKFREE_CONTROL_KERNEL_HPP

#include "NextGenKernel.hpp"

namespace ORO_ControlKernel
{

    /**
     * @brief LockFreeControlKernel is a kernel for multi-threaded DataObject access.
     * Other kernels or multithreaded components can read/write the DataObjects safely.
     *
     * The DataObjects of this kernel are choosen in such a way that another kernel (or threaded component) can
     * have thread-safe access to them. 
     */
    template <class _Extension = KernelBaseFunction>
    class LockFreeControlKernel
        : public detail::BaseKernel< detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC , 
                                     _Extension >
    {
        typedef detail::BaseKernel< detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC , 
                                     _Extension > Base;
    public:
        /**
         * @brief Create a lock-free control kernel.
         *
         * @param name The name of the kernel.
         * @param prefix The namespace prefix of the data objects.
         * 
         */
        LockFreeControlKernel(const std::string& name, const std::string& prefix)
            : Base( name, prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                    prefix + "::SetPoints", prefix + "::Outputs")
        {
        }

        /**
         * @brief Create a lock-free control kernel.
         *
         * @param name The name of the kernel.
         * 
         */
        LockFreeControlKernel(const std::string& name)
            : Base( name, name + "::Inputs", name + "::Models", name + "::Commands",
                    name + "::SetPoints", name + "::Outputs")
        {
        }
       
        /**
         * Define loadComponent with your kernel and component type
         * in a separate implementation file, allowing to compile
         * smaller units of software.
         */
        template< class ComponentT >
        bool preloadComponent( ComponentT* c);


    protected:
       template<class T>
        struct Updater {
            void operator() (T* t) {
                if ( t->isSelected() )
                    t->update();
            }
        };

        virtual void updateComponents()
        {
            // This is called from the KernelBaseFunction
            // one step is one control cycle
            // The figure is a unidirectional graph
            std::for_each(this->sensors.getValueBegin(), this->sensors.getValueEnd(), Updater<ComponentBaseInterface>() );
            std::for_each(this->estimators.getValueBegin(), this->estimators.getValueEnd(), Updater<ComponentBaseInterface>() );
            std::for_each(this->generators.getValueBegin(), this->generators.getValueEnd(), Updater<ComponentBaseInterface>() );
            std::for_each(this->controllers.getValueBegin(), this->controllers.getValueEnd(), Updater<ComponentBaseInterface>() );
            std::for_each(this->effectors.getValueBegin(), this->effectors.getValueEnd(), Updater<ComponentBaseInterface>() );
        }

    }; // class LockFreeControlKernel


                
}

#endif

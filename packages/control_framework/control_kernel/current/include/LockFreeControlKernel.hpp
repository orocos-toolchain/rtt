/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  BufferedControlKernel.hpp 

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
                
}

#endif

#ifndef BUFFERED_CONTROL_KERNEL_HPP
#define BUFFERED_CONTROL_KERNEL_HPP

#include "NextGenKernel.hpp"

namespace ORO_ControlKernel
{

    /**
     * @brief BufferedControlKernel is a kernel for multi-threaded DataObject access.
     * Other kernels or multithreaded components can read/write the DataObjects safely.
     *
     * The DataObjects of this kernel are choosen in such a way that another kernel (or threaded component) can
     * have thread-safe access to them. 
     */
    template <class _Extension = KernelBaseFunction>
    class BufferedControlKernel
        : public detail::BaseKernel< detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC , 
                                     _Extension >
    {
        typedef detail::BaseKernel< detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC ,
                                     detail::DataObjectBufferC , 
                                     _Extension > Base;
    public:
        /**
         * @brief Create a buffered control kernel.
         *
         * @param name The name of the kernel.
         * @param prefix The namespace prefix of the data objects.
         * 
         */
        BufferedControlKernel(const std::string& name, const std::string& prefix)
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

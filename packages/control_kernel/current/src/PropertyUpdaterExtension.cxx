#include "corelib/KernelPropertyExtension.hpp"

namespace ORO_ControlKernel
{


        bool PropertyComponentInterface::enableAspect( PropertyExtension* ext)
        {
            if (master == 0)
            {
                master = ext;
                master->addComponent( this );
                return true;
            }
            return false;
        }

        void PropertyComponentInterface::disableAspect()
        {
            if (master !=0)
            {
                master->removeComponent( this );
                master = 0;
            }
        }


}


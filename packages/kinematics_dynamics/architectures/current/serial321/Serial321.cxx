#include "kindyn/Serial321.hpp"

namespace ORO_KinDyn
{
    const double Serial321::M_PI_T2;

#ifndef SQRT3d2
    const double Serial321::SQRT3d2;
#endif
#ifndef SQRT3t2
    const double Serial321::SQRT3t2;
#endif

    const double Serial321::EPS_WRIST_ABOVE_BASE;

    const double Serial321::EPS_ARM_EXTENDED;

    const double Serial321::EPS_PARALLEL_WRIST;
    
    const double Serial321::KINEMATICS_EPS;

        Serial321::Serial321()
            : offset(0), eccent( 0 )
        {
            geometrySet( 1, 1, 1, 1, 1, 1);
        }

        Serial321::~Serial321() {}

}

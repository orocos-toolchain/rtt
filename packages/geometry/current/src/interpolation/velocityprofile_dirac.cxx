
#include "geometry/error.h"
#include "geometry/velocityprofile_dirac.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


    void VelocityProfile_Dirac::SetProfile(
                                           double pos1,
                                           double pos2
                                           ) 
    {
        p1 = pos1;
        p2 = pos2;
        t = 0;
    }

    void VelocityProfile_Dirac::
	SetProfileDuration(double pos1,double pos2,double duration)
    {
        SetProfile(pos1,pos2);
        t = duration;
    }

    double VelocityProfile_Dirac::Duration() const {
        return t;
    }

    double VelocityProfile_Dirac::Pos(double time) const {
        if ( t == 0 )
            return time == 0 ? p1 : p2;
        else
            return p1 + (( p2 - p1)/t)*time;
    }

    double VelocityProfile_Dirac::Vel(double time) const {
        if ( t == 0 )
            {
#ifdef HAVE_EXCEPTIONS
            throw Error_MotionPlanning_Incompatible();
#else
            return 0;
#endif
            }
        else
            if ( 0 < time && time < t )
                return (p2-p1) / t;
        return 0;
    }

    double VelocityProfile_Dirac::Acc(double time) const {
#ifdef HAVE_EXCEPTIONS
        throw Error_MotionPlanning_Incompatible();
#endif
        return 0;
    }


#ifdef OROINT_OS_STDIOSTREAM
    void VelocityProfile_Dirac::Write(ostream& os) const {
        os << "DIRACVEL[ ]";
    }
#endif



#ifdef USE_NAMESPACE
}
#endif


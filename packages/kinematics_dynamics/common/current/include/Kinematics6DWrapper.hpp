#ifndef ORO_KINDYN_KINEMATICS_6D_HPP
#define ORO_KINDYN_KINEMATICS_6D_HPP

#include "KinematicsInterface.hpp"
#include <corelib/MultiVector.hpp>

namespace ORO_KinDyn
{
    /**
     * This class wraps some old-school 6DOF kinematics functions
     * to the KinematicsInterface.
     * The old school functions use ORO_CoreLib::Double6D and a
     * plain array for jacobian calculations, the new version uses
     * the JointPositions,... and Jacobian types.
     */
    class Kinematics6DWrapper
        : public KinematicsInterface
    {
    public:
        /**
         * This is a Double6D
         */
        typedef ORO_CoreLib::MultiVector<6,double> WrapperType;
        /**
         * This is the number of elements in the wrapper type.
         */
        enum Size { size = WrapperType::size };
        virtual bool jacobianForward( const WrapperType& q, double J[ 6 ][ 6 ], Singularity& s ) const = 0;
        virtual bool jacobianInverse( const WrapperType& q, double J[ 6 ][ 6 ], Singularity& s ) const = 0;
        virtual bool positionForward( const WrapperType& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const = 0;
        virtual bool positionInverse( const ORO_Geometry::Frame& mp_base, const Configuration conf, WrapperType& q, Singularity& s ) const = 0;
        virtual bool velocityInverse( const ORO_Geometry::Frame& pos_base, const Configuration conf, const ORO_Geometry::Twist& vel_base, WrapperType& q, WrapperType& qdot, Singularity& s ) const = 0;
        virtual bool velocityInverse( const WrapperType& q, const ORO_Geometry::Twist& vel_base, WrapperType& qdot, Singularity& s ) const = 0;
        virtual bool velocityForward( const WrapperType& q, const WrapperType& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const = 0;
        virtual void configurationGet(const WrapperType& q, Configuration& c) const = 0;

        virtual int getNumberOfJoints() const { return size; }
        virtual int maxNumberOfJoints() const { return size; }

        virtual bool jacobianForward( const JointVelocities& q,
                                      Jacobian& jac, Singularity& s ) const {
            if ( q.size() < size )
                return false;
            WrapperType wq( q );
            double wJ[6][size];
	    /*Debugging*/

	    /*
            for(int i1=0; i1 < 6; ++i1)
                for(int i2=0; i2 < size; ++i2)
                    wJ[i1][i2] = jac(i1,i2);
	    return this->jacobianForward( wq, wJ, s);
	    */
	    //	    std::cout << " Kinematics6DWrapper::jacobianForward: q: " << wq <<std::endl;

	    bool ret = this->jacobianForward( wq, wJ, s);
	    
	    for ( unsigned int i = 0;i<6;i++)
	      for(unsigned int j = 0;j<size;j++)
		jac(i,j)=wJ[i][j];
	    
            return ret;
	    
        }
        
        virtual bool jacobianInverse( const JointPositions& q, Jacobian& jac, Singularity& s ) const {
            if ( q.size() < size)
                return false;
            WrapperType wq( q );
            double wJ[6][size];

	    /*Debugging*/
	    /*
            for(int i1=0; i1 < 6; ++i1)
                for(int i2=0; i2 < size; ++i2)
                    wJ[i1][i2] = jac(i1,i2);
            return this->jacobianInverse( wq, wJ, s);
	    */

	    bool ret = this->jacobianInverse( wq, wJ, s);
	    
	    for ( unsigned int i = 0;i<6;i++)
	      for(unsigned int j = 0;j<size;j++)
		jac(i,j)=wJ[i][j];
	    
            return ret;


        }
        virtual bool positionForward( const JointPositions& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const {
            if ( q.size() < size)
                return false;
            WrapperType wq(q);
            return this->positionForward( wq, mp_base, s);
        }
        virtual bool positionInverse( const ORO_Geometry::Frame& mp_base, const Configuration conf, JointPositions& q, Singularity& s ) const {
            if ( q.size() < size)
                return false;
            WrapperType wq;
            bool res = this->positionInverse( mp_base, conf, wq, s);
            if (!res)
                return false;
            wq.getVector( q );
            return true;
        }
        virtual bool velocityForward( const JointPositions& q, const JointVelocities& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const {
            if ( q.size() < size && qdot.size() < size)
                return false;
            WrapperType wqdot(qdot), wq(q);
            return this->velocityForward( wq, wqdot, pos_base, vel_base, s);
        }
        virtual bool velocityInverse( const ORO_Geometry::Frame& pos_base, const Configuration conf, const ORO_Geometry::Twist& vel_base, JointPositions& q, JointVelocities& qdot, Singularity& s ) const {
            if ( q.size() < size && qdot.size() < size)
                return false;
            WrapperType wq, wqdot;
            bool res = this->velocityInverse( pos_base, conf, vel_base, wq, wqdot, s);
            if (!res)
                return false;
            wq.getVector( q );
            wqdot.getVector( qdot );
            return true;
        }
        virtual bool velocityInverse( const JointPositions& q, const ORO_Geometry::Twist& vel_base, JointVelocities& qdot, Singularity& s ) const {
            if ( q.size() < size && qdot.size() < size)
                return false;
            WrapperType wqdot, wq(q);
            bool res = this->velocityInverse( wq, vel_base, wqdot, s);
            if (!res)
                return false;
            wqdot.getVector( qdot );
            return true;
        }
        virtual void configurationGet(const JointPositions& q, Configuration& c) const {
            WrapperType wq(q);
            this->configurationGet( wq, c);
        }
    };
}

#endif

#ifndef KINEMATICSFACTORYKUKA_HPP
#define KINEMATICSFACTORYKUKA_HPP

#include "KinematicsFactory.hpp"
#include "FSZYYDWH.hpp"
//#include <rtstl/rtstreams.hpp>

namespace ORO_KinDyn
{

/**
 * Factory for creating KUKA kinematics objects.
 */
class KinematicsFactoryKuka
    : public KinematicsFactoryStub
{
public:
    /**
     * Constructor
     */
    KinematicsFactoryKuka()
        :KinematicsFactoryStub("Kuka361")
    {
        cout <<"Registering Kuka361 kinematics factory stub..."<<endl;
    }

    virtual ~KinematicsFactoryKuka() {}

    /**
     * Factory method
     * 
     * @return The newly created KUKA kinematics object
     */
    virtual KinematicsInterface* create()
    {
        FSZYYDWH* ki = new FSZYYDWH();
        ki->geometrySet( 1.020,0.480,0.645,0.,0.,0.120 );
        ki->offsetSet(0.);
        ki->eccentricitySet(0.);
        return ki;
    }
    
};


}



#endif

/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  KinematicProcess.cxx 

                        KinematicProcess.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#include "control_kernel/KinematicProcess.hpp"

namespace ORO_ControlKernel
{
    KinematicProcess::KinematicProcess(const std::string& name)
        : Process<Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                  Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                  Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                  Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                  Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                  MakeFacet<PropertyExtension, KernelBaseFunction, ExecutionExtension>::Result>(name),
          kinarch("Architecture", "The Kinematic Architecture name, for example, Kuka160 or Kuka361.", "none"),
          qloc("PostionDataObject",
               "The DataObject category (Inputs, Models,...) in which the joint positions are located.",
               "Inputs"),
          qname("PositionName",
                "The DataObject name in which the joint positions are located.",
                "JointPositions"),
          qdotloc("VelocityDataObject",
                  "The DataObject category (Inputs, Models, Outputs,...) in which"
                  " the joint velocities are located.",
                  "Outputs"),
          qdotname("VelocityName",
                   "The DataObject name in which the joint velocities are located.",
                   "JointVelocities"),
          mykin(0),
          mykincomp(0), qDObj(0), qdotDObj(0)
    {
    }

    const std::string& KinematicProcess::getArchitecture() const {
        return kinarch.get();
    }

    ORO_KinDyn::KinematicsComponent* KinematicProcess::getKinematics() {
        return &mykincomp;
    }

    void KinematicProcess::setAlgorithm( ORO_KinDyn::KinematicsInterface *kin,
                                         const ORO_KinDyn::JointVelocities& _signs /*= ORO_KinDyn::JointVelocities()*/, 
                                         const ORO_KinDyn::JointPositions&  _offsets /*= ORO_KinDyn::JointPositions()*/ ) {
        mykin = kin;
        mykincomp.setKinematics( kin, _signs, _offsets );
    }

    bool KinematicProcess::componentStartup() {

        if ( mykin == 0 )
            return false;

        // they are optional.
        if ( strncmp(qloc.get().c_str(), "Inputs", 5 ) == 0 )
            Input->dObj()->Get(qname, qDObj);
        if ( strncmp(qloc.get().c_str(), "Commands", 5 ) == 0 )
            Command->dObj()->Get(qname, qDObj);
        if ( strncmp(qloc.get().c_str(), "Models", 5 ) == 0 )
            Model->dObj()->Get(qname, qDObj);
        if ( strncmp(qloc.get().c_str(), "SetPoints", 5 ) == 0 )
            SetPoint->dObj()->Get(qname, qDObj);
        if ( strncmp(qloc.get().c_str(), "Outputs", 5 ) == 0 )
            Output->dObj()->Get(qname, qDObj);

        if ( strncmp(qdotloc.get().c_str(), "Inputs", 5 ) == 0 )
            Input->dObj()->Get(qdotname, qdotDObj);
        if ( strncmp(qdotloc.get().c_str(), "Commands", 5 ) == 0 )
            Command->dObj()->Get(qdotname, qdotDObj);
        if ( strncmp(qdotloc.get().c_str(), "Models", 5 ) == 0 )
            Model->dObj()->Get(qdotname, qdotDObj);
        if ( strncmp(qdotloc.get().c_str(), "SetPoints", 5 ) == 0 )
            SetPoint->dObj()->Get(qdotname, qdotDObj);
        if ( strncmp(qdotloc.get().c_str(), "Outputs", 5 ) == 0 )
            Output->dObj()->Get(qdotname, qdotDObj);
        return true;
    }

    void KinematicProcess::update() 
    {
        // read in joint pos and vel, if any.
        if (qDObj && mykin) {
            qDObj->Get( tmppos );
            mykincomp.setPosition(tmppos);
        }
        if (qdotDObj && mykin) {
            qdotDObj->Get(tmppos);
            mykincomp.setVelocity(tmppos);
        }
    }

    void KinematicProcess::exportProperties(ORO_CoreLib::PropertyBag& bag)
    {
        bag.add(&kinarch);
        bag.add(&qloc);
        bag.add(&qname);
        bag.add(&qdotloc);
        bag.add(&qdotname);
    }

    bool KinematicProcess::updateProperties(const ORO_CoreLib::PropertyBag& bag)
    {
        using ORO_CoreLib::Logger;
        Logger::In in("KinematicProcess::updateProperties");
        if (composeProperty(bag, qloc) )
            Logger::log() << Logger::Info << "Read Property "<<qloc.getName()
                          <<" with value "<<qloc.get()<<Logger::nl;
        if (composeProperty(bag, qname) )
            Logger::log() << Logger::Info << "Read Property "<<qname.getName()
                          <<" with value "<<qname.get()<<Logger::nl;
        if (composeProperty(bag, qdotloc) )
            Logger::log() << Logger::Info << "Read Property "<<qdotloc.getName()
                          <<" with value "<<qdotloc.get()<<Logger::nl;
        if (composeProperty(bag, qdotname) )
            Logger::log() << Logger::Info << "Read Property "<<qdotname.getName()
                          <<" with value "<<qdotname.get()<<Logger::nl;
        if (composeProperty(bag, kinarch) && kinarch.get() != "none" )
            {
                mykin = ORO_KinDyn::KinematicsFactory::Create( kinarch );
                if (mykin) {
                    mykincomp.setKinematics( mykin );
                    tmppos.reserve( mykin->maxNumberOfJoints() );
                    tmppos.resize( mykin->getNumberOfJoints() );
                    return true;
                }
                Logger::log() << Logger::Error << "'"<< kinarch
                              << "' kinematic architecture not found."<<Logger::endl;
                return false;
                        
            }
        Logger::log() <<"Property "<< kinarch.getName() <<" not given but required."<<Logger::endl;
        return false;
    }

    ORO_Execution::MethodFactoryInterface* KinematicProcess::createMethodFactory()
    {
        using namespace ORO_Execution;
        using namespace ORO_KinDyn;
        TemplateMethodFactory<KinematicsComponent>* kfact =
            new TemplateMethodFactory<KinematicsComponent>(&mykincomp);

        kfact->add("setDelta",method( &KinematicsComponent::setDelta,
                                      "Change the delta radial movement this class can track "
                                      "inbetween calls to its interface. Defaults to 0.5 radians.",
                                      "deltaRad","Trackable change in position in radians"));
        kfact->add("getDelta",method( &KinematicsComponent::getDelta,
                                      "Get the delta radial movement this class can track "
                                      "inbetween calls to its interface. Defaults to 0.5 radians."));

        kfact->add("setPlanningMode",method( &KinematicsComponent::setPlanningMode,
                                             "Use the kinematics component for planning, do not implicitly update the joint state."));
            
        kfact->add("setTrackingMode",method( &KinematicsComponent::setTrackingMode,
                                             "Use the kinematics component for tracking, try to implicitly update the joint state."));
        kfact->add("isPlanning",method( &KinematicsComponent::isPlanning,
                                        "Returns true if calculations do not implicitly update the joint state."));
        kfact->add("isTracking",method( &KinematicsComponent::isTracking,
                                        "Returns true if calculations implicitly (try to) update the joint state."));
            
        kfact->add("jacobianForward",method( &KinematicsComponent::jacobianForward,
                                             "Calculate the forward Jacobian at a given position.",
                                             "q", "Current position of the robot in radians.",
                                             "jac", "The resulting Jacobian at position q."));
        kfact->add("jacobianInverse",method( &KinematicsComponent::jacobianInverse,
                                             "Calculate the inverse Jacobian at a given position.",
                                             "q", "Current position of the robot in radians.",
                                             "jac", "The resulting Jacobian at position q."));
            
        kfact->add("positionForward", method( &KinematicsComponent::positionForward,
                                              "Calculate the end frame of the robot.",
                                              "q", "Current position of the robot in radians.",
                                              "mp_base", "The resulting endpoint frame at position q."));

        kfact->add("positionInverse", method( &KinematicsComponent::positionInverse,
                                              "Calculate the joint positions of the robot.",
                                              "mp_base", "The current endpoint frame of the robot.",
                                              "q", "Resulting position of the robot in radians."));

        kfact->add("velocityForward", method( &KinematicsComponent::velocityForward,
                                              "Calculate the end frame and end velocity of the robot.",
                                              "q", "Current position of the robot in radians.",
                                              "qdot", "Current velocity of the robot in radians/second.",
                                              "mp_base", "The resulting endpoint frame at position q.",
                                              "vel_base", "The resulting endpoint velocity at position q."));

        kfact->add("velocityInverse", method( &KinematicsComponent::velocityInverse,
                                              "Calculate the joint velocities of the robot.",
                                              "mp_base", "The current endpoint frame of the robot.",
                                              "vel_base", "The current endpoint velocity at position q.",
                                              "q", "Resulting position of the robot joints in radians.",
                                              "qdot", "Resulting velocity of the robot joints in radians/second."));
        kfact->add("velocityInverse", method( &KinematicsComponent::velocityInverse,
                                              "Calculate the joint velocities of the robot.",
                                              "q", "Current position of the robot in radians.",
                                              "vel_base", "The current endpoint velocity at position q.",
                                              "qdot", "Resulting velocity of the robot joints in radians/second."));

        kfact->add("setPosition", method( &KinematicsComponent::setPosition,
                                          "Update component state with new joint positions.",
                                          "qnew", "The new joint positions of the robot."));
        kfact->add("getPosition", method( &KinematicsComponent::getPosition,
                                          "Get current joint positions.",
                                          "qcur", "The current joint positions of the robot."));
        kfact->add("getFrame", method( &KinematicsComponent::getFrame,
                                       "Get current end effector frame."));
        kfact->add("getTwist", method( &KinematicsComponent::getTwist,
                                       "Get current end effector twist."));

        return kfact;
    }

}
